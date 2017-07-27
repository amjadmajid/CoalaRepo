#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <libipos-gcc-dma/ipos.h>
#include <libwispbase/accel.h>
#include <libmspbuiltins/builtins.h>
#include <libio/log.h>
#include <libmsp/mem.h>
#include <libmsp/periph.h>
#include <libmsp/clock.h>
#include <libmsp/watchdog.h>
#include <libmsp/gpio.h>
#include <libmspmath/msp-math.h>

#ifdef CONFIG_LIBEDB_PRINTF
#include <libedb/edb.h>
#endif
#include "pins.h"

// Number of samples to discard before recording training set
#define NUM_WARMUP_SAMPLES 3

#define ACCEL_WINDOW_SIZE 3
#define MODEL_SIZE 16
#define SAMPLE_NOISE_FLOOR 10 // TODO: made up value

// Number of classifications to complete in one experiment
#define SAMPLES_TO_COLLECT 128

#define TASK_NUM 11
#define OFFSET(src, dest) src <= dest ? dest - src : TASK_NUM + dest - src 

unsigned volatile *timer = &TBCTL;
typedef threeAxis_t_8 accelReading;
typedef accelReading accelWindow[ACCEL_WINDOW_SIZE];

typedef struct {
	unsigned meanmag;
	unsigned stddevmag;
} features_t;

typedef enum {
	CLASS_STATIONARY,
	CLASS_MOVING,
} class_t;


unsigned overflow=0;
__attribute__((interrupt(51))) 
	void TimerB1_ISR(void){
		TBCTL &= ~(0x0002);
		if(TBCTL && 0x0001){
			overflow++;
			TBCTL |= 0x0004;
			TBCTL |= (0x0002);
			TBCTL &= ~(0x0001);	
		}
	}
__attribute__((section("__interrupt_vector_timer0_b1"),aligned(2)))
void(*__vector_timer0_b1)(void) = TimerB1_ISR;

typedef enum {
	MODE_IDLE = 3,
	MODE_TRAIN_STATIONARY = 2,
	MODE_TRAIN_MOVING = 1,
	MODE_RECOGNIZE = 0, // default
} run_mode_t;

enum task_index {
	t_init,
	t_selectMode,
	t_resetStats,
	t_sample,
	t_transform,
	t_featurize,
	t_classify,
	t_stats,
	t_warmup,
	t_train,
	t_idle
};

void task_init();
void task_selectMode();
void task_resetStats();
void task_sample();
void task_transform();
void task_featurize();
void task_classify();
void task_stats();
void task_warmup();
void task_train();
void task_idle();

__p uint16_t _v_pinState;
__p unsigned _v_discardedSamplesCount;
__p run_mode_t _v_class;
__p unsigned _v_totalCount;
__p unsigned _v_movingCount;
__p unsigned _v_stationaryCount;
__p accelReading _v_window[ACCEL_WINDOW_SIZE];
__p features_t _v_features;
__p features_t _v_model_stationary[MODEL_SIZE];
__p features_t _v_model_moving[MODEL_SIZE];
__p unsigned _v_trainingSetSize;
__p unsigned _v_samplesInWindow;
__p run_mode_t _v_mode;
__p unsigned _v_seed;
__p unsigned _v_count;

void ACCEL_singleSample_(threeAxis_t_8* result){
	result->x = (P(_v_seed)*17)%85;
	result->y = (P(_v_seed)*17*17)%85;
	result->z = (P(_v_seed)*17*17*17)%85;
	++P(_v_seed);
}

static void init_hw()
{
	msp_watchdog_disable();
	msp_gpio_unlock();
	msp_clock_setup();
}

void init()
{
#ifdef BOARD_MSP_TS430
	*timer &= 0xE6FF; //set 12,11 bit to zero (16bit) also 8 to zero (SMCLK)
	*timer |= 0x0200; //set 9 to one (SMCLK)
	*timer |= 0x00C0; //set 7-6 bit to 11 (divider = 8);
	*timer &= 0xFFEF; //set bit 4 to zero
	*timer |= 0x0020; //set bit 5 to one (5-4=10: continuous mode)
	*timer |= 0x0002; //interrupt enable
#endif
	threeAxis_t_8 accelID = {0};

	init_hw();

#ifdef CONFIG_EDB
	edb_init();
#endif

	INIT_CONSOLE();

	__enable_interrupt();

	LOG("init: initializing accel\r\n");

	LOG("init: accel hw id: 0x%x\r\n", accelID.x);

	//PRINTF(".%u.\r\n", curctx->task->idx);
}

void task_init()
{
	LOG("init\r\n");

	P(_v_pinState) = MODE_IDLE;

	P(_v_count) = 0;
	P(_v_seed) = 1;
	os_jump(OFFSET(t_init, t_selectMode));
}
void task_selectMode()
{
	uint16_t pin_state=1;
	++P(_v_count);
	LOG("count: %u\r\n",P(_v_count));
	if(P(_v_count) >= 3) pin_state=2;
	if(P(_v_count)>=5) pin_state=0;
	if (P(_v_count) >= 7) {
		PRINTF("TIME end is 65536*%u+%u\r\n",overflow,(unsigned)TBR);
		while(1);
	}
	run_mode_t mode;
	class_t class;

	// Don't re-launch training after finishing training
	if ((pin_state == MODE_TRAIN_STATIONARY ||
				pin_state == MODE_TRAIN_MOVING) &&
			pin_state == P(_v_pinState)) {
		pin_state = MODE_IDLE;
	} else {
		P(_v_pinState) = pin_state;
	}

	LOG("selectMode: 0x%x\r\n", pin_state);

	switch(pin_state) {
		case MODE_TRAIN_STATIONARY:		
			P(_v_discardedSamplesCount) = 0;
			P(_v_mode) = MODE_TRAIN_STATIONARY;
			P(_v_class) = CLASS_STATIONARY;
			P(_v_samplesInWindow) = 0;

			os_jump(OFFSET(t_selectMode, t_warmup));
			break;

		case MODE_TRAIN_MOVING:
			P(_v_discardedSamplesCount) = 0;
			P(_v_mode) = MODE_TRAIN_MOVING;
			P(_v_class) = CLASS_MOVING;
			P(_v_samplesInWindow) = 0;

			os_jump(OFFSET(t_selectMode, t_warmup));
			break;

		case MODE_RECOGNIZE:
			P(_v_mode) = MODE_RECOGNIZE;

			os_jump(OFFSET(t_selectMode, t_resetStats));
			break;

		default:
			os_jump(OFFSET(t_selectMode, t_idle));
	}
}

void task_resetStats()
{
	// NOTE: could roll this into selectMode task, but no compelling reason
	LOG("resetStats\r\n");

	// NOTE: not combined into one struct because not all code paths use both
	P(_v_movingCount) = 0;
	P(_v_stationaryCount) = 0;
	P(_v_totalCount) = 0;

	P(_v_samplesInWindow) = 0;

	os_jump(OFFSET(t_resetStats, t_sample));
}

void task_sample()
{
	LOG("sample\r\n");

	accelReading sample;
	ACCEL_singleSample_(&sample);
	P(_v_window[P(_v_samplesInWindow)]) = sample;
	++P(_v_samplesInWindow);
	LOG("sample: sample %u %u %u window %u\r\n",
			sample.x, sample.y, sample.z, P(_v_samplesInWindow));

	if (P(_v_samplesInWindow) < ACCEL_WINDOW_SIZE) {
		os_jump(OFFSET(t_sample, t_sample));
	} else {
		P(_v_samplesInWindow) = 0;
		os_jump(OFFSET(t_sample, t_transform));
	}
}

void task_transform()
{
	unsigned i;

	LOG("transform\r\n");
	accelReading *sample;
	accelReading transformedSample;

	for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
		if (P(_v_window[i]).x < SAMPLE_NOISE_FLOOR ||
				P(_v_window[i]).y < SAMPLE_NOISE_FLOOR ||
				P(_v_window[i]).z < SAMPLE_NOISE_FLOOR) {

			P(_v_window[i]).x = (P(_v_window[i]).x > SAMPLE_NOISE_FLOOR)
				? P(_v_window[i]).x : 0;
			P(_v_window[i]).y = (P(_v_window[i]).y > SAMPLE_NOISE_FLOOR)
				? P(_v_window[i]).y : 0;
			P(_v_window[i]).z = (P(_v_window[i]).z > SAMPLE_NOISE_FLOOR)
				? P(_v_window[i]).z : 0;
		}
	}
	os_jump(OFFSET(t_transform, t_featurize));
}

void task_featurize()
{
	accelReading mean, stddev;
	mean.x = mean.y = mean.z = 0;
	stddev.x = stddev.y = stddev.z = 0;
	features_t features;

	LOG("featurize\r\n");

	int i;
	for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
		LOG("featurize: features: x %u y %u z %u \r\n", P(_v_window[i]).x,P(_v_window[i]).y,P(_v_window[i]).z);
		mean.x += P(_v_window[i]).x;
		mean.y += P(_v_window[i]).y;
		mean.z += P(_v_window[i]).z;
	}
	mean.x >>= 2;
	mean.y >>= 2;
	mean.z >>= 2;

	LOG("featurize: features: mx %u my %u mz %u \r\n", mean.x,mean.y,mean.z);
	for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
		stddev.x += P(_v_window[i]).x > mean.x ? P(_v_window[i]).x - mean.x
			: mean.x - P(_v_window[i]).x;
		stddev.y += P(_v_window[i]).y > mean.y ? P(_v_window[i]).y - mean.y
			: mean.y - P(_v_window[i]).y;
		stddev.z += P(_v_window[i]).z > mean.z ? P(_v_window[i]).z - mean.z
			: mean.z - P(_v_window[i]).z;
	}
	stddev.x >>= 2;
	stddev.y >>= 2;
	stddev.z >>= 2;

	unsigned meanmag = mean.x*mean.x + mean.y*mean.y + mean.z*mean.z;
	unsigned stddevmag = stddev.x*stddev.x + stddev.y*stddev.y + stddev.z*stddev.z;
	features.meanmag   = sqrt16(meanmag);
	features.stddevmag = sqrt16(stddevmag);
	LOG("featurize: features: mean %u stddev %u\r\n",
			features.meanmag, features.stddevmag);

	switch (P(_v_mode)) {
		case MODE_TRAIN_STATIONARY:
		case MODE_TRAIN_MOVING:
			P(_v_features.meanmag) = features.meanmag;
			P(_v_features.stddevmag) = features.stddevmag;
			os_jump(OFFSET(t_featurize, t_train));
			break;
		case MODE_RECOGNIZE:
			P(_v_features.meanmag) = features.meanmag;
			P(_v_features.stddevmag) = features.stddevmag;
			os_jump(OFFSET(t_featurize, t_classify));
			break;
		default:
			// TODO: abort
			break;
	}
}

void task_classify() {
	int move_less_error = 0;
	int stat_less_error = 0;
	int i;
	class_t class;
	long int meanmag;
	long int stddevmag;
	LOG("classify\r\n");
	meanmag = P(_v_features.meanmag);
	stddevmag = P(_v_features.stddevmag);
	LOG("classify: mean: %u\r\n", meanmag);
	LOG("classify: stddev: %u\r\n", stddevmag);

	for (i = 0; i < MODEL_SIZE; ++i) {
		long int stat_mean_err = (P(_v_model_stationary[i].meanmag) > meanmag)
			? (P(_v_model_stationary[i].meanmag) - meanmag)
			: (meanmag - P(_v_model_stationary[i].meanmag));

		long int stat_sd_err = (P(_v_model_stationary[i].stddevmag) > stddevmag)
			? (P(_v_model_stationary[i].stddevmag) - stddevmag)
			: (stddevmag - P(_v_model_stationary[i].stddevmag));
		LOG("classify: model_mean: %u\r\n", P(_v_model_stationary[i].meanmag));
		LOG("classify: model_stddev: %u\r\n", P(_v_model_stationary[i].stddevmag));
		LOG("classify: stat_mean_err: %u\r\n", stat_mean_err);
		LOG("classify: stat_stddev_err: %u\r\n", stat_sd_err);

		long int move_mean_err = (P(_v_model_moving[i].meanmag) > meanmag)
			? (P(_v_model_moving[i].meanmag) - meanmag)
			: (meanmag - P(_v_model_moving[i].meanmag));

		long int move_sd_err = (P(_v_model_moving[i].stddevmag) > stddevmag)
			? (P(_v_model_moving[i].stddevmag) - stddevmag)
			: (stddevmag - P(_v_model_moving[i].stddevmag));
		LOG("classify: model_mean: %u\r\n", P(_v_model_moving[i].meanmag));
		LOG("classify: model_stddev: %u\r\n", P(_v_model_moving[i].stddevmag));
		LOG("classify: move_mean_err: %u\r\n", move_mean_err);
		LOG("classify: move_stddev_err: %u\r\n", move_sd_err);
		if (move_mean_err < stat_mean_err) {
			move_less_error++;
		} else {
			stat_less_error++;
		}

		if (move_sd_err < stat_sd_err) {
			move_less_error++;
		} else {
			stat_less_error++;
		}
	}

	P(_v_class) = (move_less_error > stat_less_error) ? CLASS_MOVING : CLASS_STATIONARY;

	LOG("classify: class 0x%x\r\n", P(_v_class));

	os_jump(OFFSET(t_classify, t_stats));
}

void task_stats()
{
	unsigned movingCount = 0, stationaryCount = 0;

	LOG("stats\r\n");

	++P(_v_totalCount);
	LOG("stats: total %u\r\n", P(_v_totalCount));

	switch (P(_v_class)) {
		case CLASS_MOVING:

			++P(_v_movingCount);
			LOG("stats: moving %u\r\n", P(_v_movingCount));
			break;
		case CLASS_STATIONARY:

			++P(_v_stationaryCount);
			LOG("stats: stationary %u\r\n", P(_v_stationaryCount));
			break;
	}

	if (P(_v_totalCount) == SAMPLES_TO_COLLECT) {

		unsigned resultStationaryPct = P(_v_stationaryCount) * 100 / P(_v_totalCount);
		unsigned resultMovingPct = P(_v_movingCount) * 100 / P(_v_totalCount);

		unsigned sum = P(_v_stationaryCount) + P(_v_movingCount);
		PRINTF("stats: s %u (%u%%) m %u (%u%%) sum/tot %u/%u: %c\r\n",
		       P(_v_stationaryCount), resultStationaryPct,
		       P(_v_movingCount), resultMovingPct,
		       P(_v_totalCount), sum, sum == P(_v_totalCount) ? 'V' : 'X');
		os_jump(OFFSET(t_stats, t_idle));
	} else {
		os_jump(OFFSET(t_stats, t_sample));
	}
}

void task_warmup()
{
	threeAxis_t_8 sample;
	LOG("warmup\r\n");

	if (P(_v_discardedSamplesCount) < NUM_WARMUP_SAMPLES) {

		ACCEL_singleSample_(&sample);
		++P(_v_discardedSamplesCount);
		LOG("warmup: discarded %u\r\n", P(_v_discardedSamplesCount));
		os_jump(OFFSET(t_warmup, t_warmup));
	} else {
		P(_v_trainingSetSize) = 0;
		os_jump(OFFSET(t_warmup, t_sample));
	}
}

void task_train()
{
	LOG("train\r\n");
	unsigned trainingSetSize;;
	unsigned class;

	switch (P(_v_class)) {
		case CLASS_STATIONARY:
			P(_v_model_stationary[P(_v_trainingSetSize)].meanmag) = P(_v_features.meanmag);
			P(_v_model_stationary[P(_v_trainingSetSize)].stddevmag) = P(_v_features.stddevmag);
			break;
		case CLASS_MOVING:
			P(_v_model_moving[P(_v_trainingSetSize)].meanmag) = P(_v_features.meanmag);
			P(_v_model_moving[P(_v_trainingSetSize)].stddevmag) = P(_v_features.stddevmag);
			break;
	}

	++P(_v_trainingSetSize);
	LOG("train: class %u count %u/%u\r\n", P(_v_class),
			P(_v_trainingSetSize), MODEL_SIZE);

	if (P(_v_trainingSetSize) < MODEL_SIZE) {
		os_jump(OFFSET(t_train, t_sample));
	} else {
		//        PRINTF("train: class %u done (mn %u sd %u)\r\n",
		//               class, features.meanmag, features.stddevmag);
		os_jump(OFFSET(t_train, t_idle));
	}
}

void task_idle() {
	LOG("idle\r\n");

	os_jump(OFFSET(t_idle, t_selectMode));
}

int main(void) {
	init();
	taskId tasks[] = {{task_init, 0},
		 {task_selectMode, 0},
		 {task_resetStats, 0},
		 {task_sample, 0},
		 {task_transform, 0},
		 {task_featurize, 0},
		 {task_classify, 0},
		 {task_stats, 0},
		 {task_warmup, 0},
		 {task_train, 0},
		 {task_idle, 0}};
	//This function should be called only once
	os_addTasks(TASK_NUM, tasks );

	os_scheduler();
	return 0;
}
