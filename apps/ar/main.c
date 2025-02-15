#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <mspReseter.h>
#include "mspProfiler.h"
#include "mspDebugger.h"

#include <ipos.h>
#include <accel.h>

#include <msp-math.h>

#define TSK_SIZ
//#define AUTO_RST
//#define LOG_INFO
//#define RAISE_PIN 1

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
    result->x = (RP(_v_seed)*17)%85;
    result->y = (RP(_v_seed)*17*17)%85;
    result->z = (RP(_v_seed)*17*17*17)%85;
    ++WP(_v_seed);
}

__nv uint8_t pinCont = 0;

void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
  // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
  PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.

#if RAISE_PIN
  P3OUT &=~BIT5;
  P3DIR |=BIT5;
#endif


#if 1
    CSCTL0_H = CSKEY >> 8;                // Unlock CS registers
//  CSCTL1 = DCOFSEL_4 |  DCORSEL;                   // Set DCO to 16MHz
    CSCTL1 = DCOFSEL_6;                   // Set DCO to 8MHz
    CSCTL2 =  SELM__DCOCLK;               // MCLK = DCO
    CSCTL3 = DIVM__1;                     // divide the DCO frequency by 1
    CSCTL0_H = 0;
#endif

#ifdef TSK_SIZ
    uart_init();
    cp_init();
#endif


#ifdef LOG_INFO
    uart_init();
#endif

#ifdef AUTO_RST
    mr_auto_rand_reseter(25000); // every 12 msec the MCU will be reseted
#endif

    threeAxis_t_8 accelID = {0};

}

void task_init()
{
#ifdef TSK_SIZ
       cp_reset();
#endif

    pinCont=1;

        P3OUT |=BIT5;
        P3OUT &=~BIT5;

    WP(_v_pinState) = MODE_IDLE;

    WP(_v_count) = 0;
    WP(_v_seed) = 1;
    os_jump(OFFSET(t_init, t_selectMode));

#ifdef TSK_SIZ
       cp_sendRes("task_init \0");
#endif

}
void task_selectMode()
{
    uint16_t pin_state=1;
    ++WP(_v_count);
    // LOG("count: %u\r\n",RP(_v_count));
    if(RP(_v_count) >= 3) pin_state=2;
    if(RP(_v_count)>=5) pin_state=0;
    // if (RP(_v_count) >= 7) {
    //     PRINTF("TIME end is 65536*%u+%u\r\n",overflow,(unsigned)TBR);
    //     while(1);
    // }
    run_mode_t mode;
    class_t class;

    // Don't re-launch training after finishing training
    if ((pin_state == MODE_TRAIN_STATIONARY ||
                pin_state == MODE_TRAIN_MOVING) &&
            pin_state == RP(_v_pinState)) {
        pin_state = MODE_IDLE;
    } else {
        WP(_v_pinState) = pin_state;
    }

    // LOG("selectMode: 0x%x\r\n", pin_state);

    switch(pin_state) {
        case MODE_TRAIN_STATIONARY:
            WP(_v_discardedSamplesCount) = 0;
            WP(_v_mode) = MODE_TRAIN_STATIONARY;
            WP(_v_class) = CLASS_STATIONARY;
            WP(_v_samplesInWindow) = 0;

            os_jump(OFFSET(t_selectMode, t_warmup));
            break;

        case MODE_TRAIN_MOVING:
            WP(_v_discardedSamplesCount) = 0;
            WP(_v_mode) = MODE_TRAIN_MOVING;
            WP(_v_class) = CLASS_MOVING;
            WP(_v_samplesInWindow) = 0;

            os_jump(OFFSET(t_selectMode, t_warmup));
            break;

        case MODE_RECOGNIZE:
            WP(_v_mode) = MODE_RECOGNIZE;

            os_jump(OFFSET(t_selectMode, t_resetStats));
            break;

        default:
            os_jump(OFFSET(t_selectMode, t_idle));
    }

#ifdef TSK_SIZ
       cp_sendRes("task_selectMode \0");
#endif

}

void task_resetStats()
{
#ifdef TSK_SIZ
       cp_reset();
#endif

    // NOTE: could roll this into selectMode task, but no compelling reason
    // LOG("resetStats\r\n");

    // NOTE: not combined into one struct because not all code paths use both
    WP(_v_movingCount) = 0;
    WP(_v_stationaryCount) = 0;
    WP(_v_totalCount) = 0;

    WP(_v_samplesInWindow) = 0;

    os_jump(OFFSET(t_resetStats, t_sample));

#ifdef TSK_SIZ
       cp_sendRes("task_resetStats \0");
#endif

}

void task_sample()
{
#ifdef TSK_SIZ
       cp_reset();
#endif

    // LOG("sample\r\n");

    accelReading sample;
    ACCEL_singleSample_(&sample);
    WP(_v_window[RP(_v_samplesInWindow)].x) = sample.x;
    WP(_v_window[RP(_v_samplesInWindow)].y) = sample.y;
    WP(_v_window[RP(_v_samplesInWindow)].z) = sample.z;
    ++WP(_v_samplesInWindow);
    // LOG("sample: sample %u %u %u window %u\r\n",
    //         sample.x, sample.y, sample.z, RP(_v_samplesInWindow));

    if (RP(_v_samplesInWindow) < ACCEL_WINDOW_SIZE) {
        os_jump(OFFSET(t_sample, t_sample));
    } else {
        RP(_v_samplesInWindow) = 0;
        os_jump(OFFSET(t_sample, t_transform));
    }

#ifdef TSK_SIZ
       cp_sendRes("task_sample \0");
#endif

}

void task_transform()
{
#ifdef TSK_SIZ
       cp_reset();
#endif

    unsigned i;

    // LOG("transform\r\n");
    accelReading *sample;
    accelReading transformedSample;

    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        if (RP(_v_window[i].x) < SAMPLE_NOISE_FLOOR ||
                RP(_v_window[i].y) < SAMPLE_NOISE_FLOOR ||
                RP(_v_window[i].z) < SAMPLE_NOISE_FLOOR) {

            WP(_v_window[i].x) = (RP(_v_window[i].x) > SAMPLE_NOISE_FLOOR)
                ? RP(_v_window[i].x) : 0;
            WP(_v_window[i].y) = (RP(_v_window[i].y) > SAMPLE_NOISE_FLOOR)
                ? RP(_v_window[i].y) : 0;
            WP(_v_window[i].z) = (RP(_v_window[i].z) > SAMPLE_NOISE_FLOOR)
                ? RP(_v_window[i].z) : 0;
        }
    }
    os_jump(OFFSET(t_transform, t_featurize));

#ifdef TSK_SIZ
       cp_sendRes("task_transform \0");
#endif

}

void task_featurize()
{
#ifdef TSK_SIZ
       cp_reset();
#endif

    accelReading mean, stddev;
    mean.x = mean.y = mean.z = 0;
    stddev.x = stddev.y = stddev.z = 0;
    features_t features;

    // LOG("featurize\r\n");

    int i;
    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        // LOG("featurize: features: x %u y %u z %u \r\n", RP(_v_window[i].x),RP(_v_window[i].y),RP(_v_window[i].z));
        mean.x += RP(_v_window[i].x);
        mean.y += RP(_v_window[i].y);
        mean.z += RP(_v_window[i].z);
    }
    mean.x >>= 2;
    mean.y >>= 2;
    mean.z >>= 2;

    // LOG("featurize: features: mx %u my %u mz %u \r\n", mean.x,mean.y,mean.z);
    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        stddev.x += RP(_v_window[i].x) > mean.x ? RP(_v_window[i].x) - mean.x
            : mean.x - RP(_v_window[i].x);
        stddev.y += RP(_v_window[i].y) > mean.y ? RP(_v_window[i].y) - mean.y
            : mean.y - RP(_v_window[i].y);
        stddev.z += RP(_v_window[i].z) > mean.z ? RP(_v_window[i].z) - mean.z
            : mean.z - RP(_v_window[i].z);
    }
    stddev.x >>= 2;
    stddev.y >>= 2;
    stddev.z >>= 2;

    unsigned meanmag = mean.x*mean.x + mean.y*mean.y + mean.z*mean.z;
    unsigned stddevmag = stddev.x*stddev.x + stddev.y*stddev.y + stddev.z*stddev.z;
    features.meanmag   = sqrt16(meanmag);
    features.stddevmag = sqrt16(stddevmag);
    // LOG("featurize: features: mean %u stddev %u\r\n",
    //         features.meanmag, features.stddevmag);

    switch (RP(_v_mode)) {
        case MODE_TRAIN_STATIONARY:
        case MODE_TRAIN_MOVING:
            WP(_v_features.meanmag) = features.meanmag;
            WP(_v_features.stddevmag) = features.stddevmag;
            os_jump(OFFSET(t_featurize, t_train));
            break;
        case MODE_RECOGNIZE:
            WP(_v_features.meanmag) = features.meanmag;
            WP(_v_features.stddevmag) = features.stddevmag;
            os_jump(OFFSET(t_featurize, t_classify));
            break;
        default:
            // TODO: abort
            break;
    }

#ifdef TSK_SIZ
       cp_sendRes("task_featurize \0");
#endif

}

void task_classify() {

#ifdef TSK_SIZ
       cp_reset();
#endif

    int move_less_error = 0;
    int stat_less_error = 0;
    int i;
    class_t class;
    long int meanmag;
    long int stddevmag;
    // LOG("classify\r\n");
    meanmag = RP(_v_features.meanmag);
    stddevmag = RP(_v_features.stddevmag);
    // LOG("classify: mean: %u\r\n", meanmag);
    // LOG("classify: stddev: %u\r\n", stddevmag);

    for (i = 0; i < MODEL_SIZE; ++i) {
        long int stat_mean_err = (RP(_v_model_stationary[i].meanmag) > meanmag)
            ? (RP(_v_model_stationary[i].meanmag) - meanmag)
            : (meanmag - RP(_v_model_stationary[i].meanmag));

        long int stat_sd_err = (RP(_v_model_stationary[i].stddevmag) > stddevmag)
            ? (RP(_v_model_stationary[i].stddevmag) - stddevmag)
            : (stddevmag - RP(_v_model_stationary[i].stddevmag));
        // LOG("classify: model_mean: %u\r\n", RP(_v_model_stationary[i].meanmag));
        // LOG("classify: model_stddev: %u\r\n", RP(_v_model_stationary[i].stddevmag));
        // LOG("classify: stat_mean_err: %u\r\n", stat_mean_err);
        // LOG("classify: stat_stddev_err: %u\r\n", stat_sd_err);

        long int move_mean_err = (RP(_v_model_moving[i].meanmag) > meanmag)
            ? (RP(_v_model_moving[i].meanmag) - meanmag)
            : (meanmag - RP(_v_model_moving[i].meanmag));

        long int move_sd_err = (RP(_v_model_moving[i].stddevmag) > stddevmag)
            ? (RP(_v_model_moving[i].stddevmag) - stddevmag)
            : (stddevmag - RP(_v_model_moving[i].stddevmag));
        // LOG("classify: model_mean: %u\r\n", RP(_v_model_moving[i].meanmag));
        // LOG("classify: model_stddev: %u\r\n", RP(_v_model_moving[i].stddevmag));
        // LOG("classify: move_mean_err: %u\r\n", move_mean_err);
        // LOG("classify: move_stddev_err: %u\r\n", move_sd_err);
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

    WP(_v_class) = (move_less_error > stat_less_error) ? CLASS_MOVING : CLASS_STATIONARY;

    // LOG("classify: class 0x%x\r\n", RP(_v_class));

    os_jump(OFFSET(t_classify, t_stats));


#ifdef TSK_SIZ
       cp_sendRes("task_classify \0");
#endif

}

void task_stats()
{

#ifdef TSK_SIZ
       cp_reset();
#endif

    unsigned movingCount = 0, stationaryCount = 0;

    // LOG("stats\r\n");

    ++WP(_v_totalCount);
    // LOG("stats: total %u\r\n", RP(_v_totalCount));

    switch (RP(_v_class)) {
        case CLASS_MOVING:

            ++WP(_v_movingCount);
            // LOG("stats: moving %u\r\n", RP(_v_movingCount));
            break;
        case CLASS_STATIONARY:

            ++WP(_v_stationaryCount);
            // LOG("stats: stationary %u\r\n", RP(_v_stationaryCount));
            break;
    }

    if (RP(_v_totalCount) == SAMPLES_TO_COLLECT) {

        unsigned resultStationaryPct = RP(_v_stationaryCount) * 100 / RP(_v_totalCount);
        unsigned resultMovingPct = RP(_v_movingCount) * 100 / RP(_v_totalCount);

        unsigned sum = RP(_v_stationaryCount) + RP(_v_movingCount);
        // PRINTF("stats: s %u (%u%%) m %u (%u%%) sum/tot %u/%u: %c\r\n",
        //        RP(_v_stationaryCount), resultStationaryPct,
        //        RP(_v_movingCount), resultMovingPct,
        //        RP(_v_totalCount), sum, sum == RP(_v_totalCount) ? 'V' : 'X');
        os_jump(OFFSET(t_stats, t_idle));
    } else {
        os_jump(OFFSET(t_stats, t_sample));
    }

#ifdef TSK_SIZ
       cp_sendRes("task_stats \0");
#endif

}

void task_warmup()
{

#ifdef TSK_SIZ
       cp_reset();
#endif

    threeAxis_t_8 sample;
    // LOG("warmup\r\n");

    if (RP(_v_discardedSamplesCount) < NUM_WARMUP_SAMPLES) {

        ACCEL_singleSample_(&sample);
        ++WP(_v_discardedSamplesCount);
        // LOG("warmup: discarded %u\r\n", RP(_v_discardedSamplesCount));
        os_jump(OFFSET(t_warmup, t_warmup));
    } else {
        WP(_v_trainingSetSize) = 0;
        os_jump(OFFSET(t_warmup, t_sample));
    }

#ifdef TSK_SIZ
       cp_sendRes("task_warmup \0");
#endif

}

void task_train()
{

#ifdef TSK_SIZ
       cp_reset();
#endif

    // LOG("train\r\n");
    unsigned trainingSetSize;;
    unsigned class;

    switch (RP(_v_class)) {
        case CLASS_STATIONARY:
            WP(_v_model_stationary[RP(_v_trainingSetSize)].meanmag) = RP(_v_features.meanmag);
            WP(_v_model_stationary[RP(_v_trainingSetSize)].stddevmag) = RP(_v_features.stddevmag);
            break;
        case CLASS_MOVING:
            WP(_v_model_moving[RP(_v_trainingSetSize)].meanmag) = RP(_v_features.meanmag);
            WP(_v_model_moving[RP(_v_trainingSetSize)].stddevmag) = RP(_v_features.stddevmag);
            break;
    }

    ++WP(_v_trainingSetSize);
    // LOG("train: class %u count %u/%u\r\n", RP(_v_class),
    //         RP(_v_trainingSetSize), MODEL_SIZE);

    if (RP(_v_trainingSetSize) < MODEL_SIZE) {
        os_jump(OFFSET(t_train, t_sample));
    } else {
        //        PRINTF("train: class %u done (mn %u sd %u)\r\n",
        //               class, features.meanmag, features.stddevmag);
        os_jump(OFFSET(t_train, t_idle));
    }

#ifdef TSK_SIZ
       cp_sendRes("task_train \0");
#endif

}

void task_idle() {

    if (pinCont){
        P3OUT |=BIT5;
        P3OUT &=~BIT5;
    }
    pinCont=0;

    PAGCMT();

    os_jump(OFFSET(t_idle, t_selectMode));

#ifdef TSK_SIZ
       cp_sendRes("task_idle \0");
#endif

       while(1);

}

int main(void) {
    init();
    taskId tasks[] = {{task_init, 1,1},
         {task_selectMode, 2,2},
         {task_resetStats, 3,1},
         {task_sample, 4,3},
         {task_transform, 5,1},
         {task_featurize, 6,3},
         {task_classify, 7,5},
         {task_stats, 8,1},
         {task_warmup, 9,2},
         {task_train, 10,2},
         {task_idle, 11,1}};
    //This function should be called only once
    os_addTasks(TASK_NUM, tasks );

    os_scheduler();
    return 0;
}
