#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <mspReseter.h>
#include <mspProfiler.h>
#include <mspDebugger.h>
#include <mspbase.h>
#include <accel.h>
#include <msp-math.h>

#include <alpaca.h>

// Only for profiling, removable otherwise
#include <ctlflags.h>

// Profiling flags.
#if ENABLE_PRF
__nv uint8_t full_run_started = 0;
__nv uint8_t first_run = 1;
#endif

#ifndef RST_TIME
#define RST_TIME 25000
#endif

// Number of samples to discard before recording training set
#define NUM_WARMUP_SAMPLES 3

#define ACCEL_WINDOW_SIZE 3
#define MODEL_SIZE 16
#define SAMPLE_NOISE_FLOOR 10 // TODO: made up value

// Number of classifications to complete in one experiment
// #define SAMPLES_TO_COLLECT 8
#define SAMPLES_TO_COLLECT 128

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

TASK(1, task_init)
TASK(2, task_selectMode)
TASK(3, task_resetStats)
TASK(4, task_sample)
TASK(5, task_transform)
TASK(6, task_featurize)
TASK(7, task_classify)
TASK(8, task_stats)
TASK(9, task_warmup)
TASK(10, task_train)
TASK(11, task_idle)

/* This is originally done by the compiler */
__nv uint8_t* data_src[3];
__nv uint8_t* data_dest[3];
__nv unsigned data_size[3];

GLOBAL_SB(unsigned, seed_bak);
GLOBAL_SB(uint16_t, pinState_bak);
GLOBAL_SB(unsigned, count_bak);
GLOBAL_SB(unsigned, discardedSamplesCount_bak);
GLOBAL_SB(unsigned, samplesInWindow_bak);
GLOBAL_SB(unsigned, movingCount_bak);
GLOBAL_SB(unsigned, stationaryCount_bak);
GLOBAL_SB(unsigned, totalCount_bak);
GLOBAL_SB(accelReading, window_bak, ACCEL_WINDOW_SIZE);
GLOBAL_SB(unsigned, window_isDirty, ACCEL_WINDOW_SIZE);
GLOBAL_SB(unsigned, trainingSetSize_bak);

void clear_isDirty() {
    memset(&GV(window_isDirty, 0), 0, sizeof(_global_window_isDirty));
}
/* end */

GLOBAL_SB(uint16_t, pinState);
GLOBAL_SB(unsigned, discardedSamplesCount);
GLOBAL_SB(class_t, class);
GLOBAL_SB(unsigned, totalCount);
GLOBAL_SB(unsigned, movingCount);
GLOBAL_SB(unsigned, stationaryCount);
GLOBAL_SB(accelReading, window, ACCEL_WINDOW_SIZE);
GLOBAL_SB(features_t, features);
GLOBAL_SB(features_t, model_stationary, MODEL_SIZE);
GLOBAL_SB(features_t, model_moving, MODEL_SIZE);
GLOBAL_SB(unsigned, trainingSetSize);
GLOBAL_SB(unsigned, samplesInWindow);
GLOBAL_SB(run_mode_t, mode);
GLOBAL_SB(unsigned, seed);
GLOBAL_SB(unsigned, count);

void ACCEL_singleSample_(threeAxis_t_8* result){
    result->x = (GV(seed_bak)*17)%85;
    result->y = (GV(seed_bak)*17*17)%85;
    result->z = (GV(seed_bak)*17*17*17)%85;
    ++GV(seed_bak);
}


void task_init()
{
#if ENABLE_PRF
    full_run_started = 1;
#endif

    GV(pinState) = MODE_IDLE;

    GV(count) = 0;
    GV(seed) = 1;

    TRANSITION_TO(task_selectMode);

}

void task_selectMode()
{
    PRIV(count);
    PRIV(pinState);

    uint16_t pin_state = 1;
    ++GV(count_bak);

    if(GV(count_bak) >= 3) pin_state = 2;
    if(GV(count_bak) >= 5) pin_state = 0;
    if (GV(count_bak) >= 7) {

#if ENABLE_PRF
        if (full_run_started) {
#if AUTO_RST
            msp_reseter_halt();
#endif
            PRF_APP_END();
            full_run_started = 0;
#if AUTO_RST
            msp_reseter_resume();
#endif
        }
#endif

        COMMIT(count);
        COMMIT(pinState);
        TRANSITION_TO(task_init);
    }

    // Don't re-launch training after finishing training
    if ((pin_state == MODE_TRAIN_STATIONARY ||
                pin_state == MODE_TRAIN_MOVING) &&
            pin_state == GV(pinState_bak)) {
        pin_state = MODE_IDLE;
    } else {
        GV(pinState_bak) = pin_state;
    }

    switch(pin_state) {
        case MODE_TRAIN_STATIONARY:
            GV(discardedSamplesCount) = 0;
            GV(mode) = MODE_TRAIN_STATIONARY;
            GV(class) = CLASS_STATIONARY;
            GV(samplesInWindow) = 0;

            COMMIT(count);
            COMMIT(pinState);
            TRANSITION_TO(task_warmup);
            break;

        case MODE_TRAIN_MOVING:
            GV(discardedSamplesCount) = 0;
            GV(mode) = MODE_TRAIN_MOVING;
            GV(class) = CLASS_MOVING;
            GV(samplesInWindow) = 0;

            COMMIT(count);
            COMMIT(pinState);
            TRANSITION_TO(task_warmup);
            break;

        case MODE_RECOGNIZE:
            GV(mode) = MODE_RECOGNIZE;

            COMMIT(count);
            COMMIT(pinState);
            TRANSITION_TO(task_resetStats);
            break;

        default:
            COMMIT(count);
            COMMIT(pinState);
            TRANSITION_TO(task_idle);
    }
}

void task_resetStats()
{
    // NOTE: could roll this into selectMode task, but no compelling reason

    // NOTE: not combined into one struct because not all code paths use both
    GV(movingCount) = 0;
    GV(stationaryCount) = 0;
    GV(totalCount) = 0;

    GV(samplesInWindow) = 0;

    TRANSITION_TO(task_sample);
}

void task_sample()
{
    PRIV(samplesInWindow);
    PRIV(seed);

    accelReading sample;
    ACCEL_singleSample_(&sample);
    GV(window, _global_samplesInWindow_bak) = sample;
    ++GV(samplesInWindow_bak);

    if (GV(samplesInWindow_bak) < ACCEL_WINDOW_SIZE) {
        COMMIT(samplesInWindow);
        COMMIT(seed);
        TRANSITION_TO(task_sample);
    } else {
        GV(samplesInWindow_bak) = 0;
        COMMIT(samplesInWindow);
        COMMIT(seed);
        TRANSITION_TO(task_transform);
    }
}

void task_transform()
{
    unsigned i;

    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        // To mimic stupid compiler behavior,
        // we write a less optimal code..
        DY_PRIV(window, i);
        if (GV(window_bak, i).x < SAMPLE_NOISE_FLOOR) {
            DY_PRIV(window, i);
            if (GV(window_bak, i).x > SAMPLE_NOISE_FLOOR) {
                DY_PRIV(window, i);
                GV(window_bak, i).x = GV(window_bak, i).x;
            }
            else {
                GV(window_bak, i).x = 0;
            }
            DY_COMMIT(window, i);

            DY_PRIV(window, i);
            if (GV(window_bak, i).y > SAMPLE_NOISE_FLOOR) {
                DY_PRIV(window, i);
                GV(window_bak, i).y = GV(window_bak, i).y;
            }
            else {
                GV(window_bak, i).y = 0;
            }
            DY_COMMIT(window, i);

            DY_PRIV(window, i);
            if (GV(window_bak, i).z > SAMPLE_NOISE_FLOOR) {
                DY_PRIV(window, i);
                GV(window_bak, i).z = GV(window_bak, i).z;
            }
            else {
                GV(window_bak, i).z = 0;
            }
            DY_COMMIT(window, i);
        }
        else {
            DY_PRIV(window, i);
            if (GV(window_bak, i).y < SAMPLE_NOISE_FLOOR) {
                DY_PRIV(window, i);
                if (GV(window_bak, i).x > SAMPLE_NOISE_FLOOR) {
                    DY_PRIV(window, i);
                    GV(window_bak, i).x = GV(window_bak, i).x;
                }
                else {
                    GV(window_bak, i).x = 0;
                }
                DY_COMMIT(window, i);

                DY_PRIV(window, i);
                if (GV(window_bak, i).y > SAMPLE_NOISE_FLOOR) {
                    DY_PRIV(window, i);
                    GV(window_bak, i).y = GV(window_bak, i).y;
                }
                else {
                    GV(window_bak, i).y = 0;
                }
                DY_COMMIT(window, i);

                DY_PRIV(window, i);
                if (GV(window_bak, i).z > SAMPLE_NOISE_FLOOR) {
                    DY_PRIV(window, i);
                    GV(window_bak, i).z = GV(window_bak, i).z;
                }
                else {
                    GV(window_bak, i).z = 0;
                }
                DY_COMMIT(window, i);
            }
            else {
                DY_PRIV(window, i);
                if (GV(window_bak, i).z < SAMPLE_NOISE_FLOOR) {
                    DY_PRIV(window, i);
                    if (GV(window_bak, i).x > SAMPLE_NOISE_FLOOR) {
                        DY_PRIV(window, i);
                        GV(window_bak, i).x = GV(window_bak, i).x;
                    }
                    else {
                        GV(window_bak, i).x = 0;
                    }
                    DY_COMMIT(window, i);

                    DY_PRIV(window, i);
                    if (GV(window_bak, i).y > SAMPLE_NOISE_FLOOR) {
                        DY_PRIV(window, i);
                        GV(window_bak, i).y = GV(window_bak, i).y;
                    }
                    else {
                        GV(window_bak, i).y = 0;
                    }
                    DY_COMMIT(window, i);

                    DY_PRIV(window, i);
                    if (GV(window_bak, i).z > SAMPLE_NOISE_FLOOR) {
                        DY_PRIV(window, i);
                        GV(window_bak, i).z = GV(window_bak, i).z;
                    }
                    else {
                        GV(window_bak, i).z = 0;
                    }
                    DY_COMMIT(window, i);
                }
            }
        }
    }
    TRANSITION_TO(task_featurize);
}

void task_featurize()
{
    accelReading mean, stddev;
    mean.x = mean.y = mean.z = 0;
    stddev.x = stddev.y = stddev.z = 0;
    features_t features;

    int i;
    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        mean.x += GV(window, i).x;
        mean.y += GV(window, i).y;
        mean.z += GV(window, i).z;
    }
    mean.x >>= 2;
    mean.y >>= 2;
    mean.z >>= 2;

    accelReading sample;

    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        sample = GV(window, i);
        stddev.x += (sample.x > mean.x) ? (sample.x - mean.x)
            : (mean.x - sample.x);
        stddev.y += (sample.y > mean.y) ? (sample.y - mean.y)
            : (mean.y - sample.y);
        stddev.z += (sample.z > mean.z) ? (sample.z - mean.z)
            : (mean.z - sample.z);
    }
    stddev.x >>= 2;
    stddev.y >>= 2;
    stddev.z >>= 2;

    unsigned meanmag = mean.x*mean.x + mean.y*mean.y + mean.z*mean.z;
    unsigned stddevmag = stddev.x*stddev.x + stddev.y*stddev.y + stddev.z*stddev.z;
    features.meanmag   = sqrt16(meanmag);
    features.stddevmag = sqrt16(stddevmag);

    switch (GV(mode)) {
        case MODE_TRAIN_STATIONARY:
        case MODE_TRAIN_MOVING:
            GV(features) = features;
            TRANSITION_TO(task_train);
            break;
        case MODE_RECOGNIZE:
            GV(features) = features;
            TRANSITION_TO(task_classify);
            break;
        default:
            // TODO: abort
            break;
    }
}

void task_classify()
{
    int move_less_error = 0;
    int stat_less_error = 0;
    int i;

    long int meanmag;
    long int stddevmag;
    meanmag = GV(features).meanmag;
    stddevmag = GV(features).stddevmag;

    features_t ms, mm;

    for (i = 0; i < MODEL_SIZE; ++i) {
        ms = GV(model_stationary, i);
        mm = GV(model_moving, i);

        long int stat_mean_err = (ms.meanmag > meanmag)
            ? (ms.meanmag - meanmag)
            : (meanmag - ms.meanmag);

        long int stat_sd_err = (ms.stddevmag > stddevmag)
            ? (ms.stddevmag - stddevmag)
            : (stddevmag - ms.stddevmag);

        long int move_mean_err = (mm.meanmag > meanmag)
            ? (mm.meanmag - meanmag)
            : (meanmag - mm.meanmag);

        long int move_sd_err = (mm.stddevmag > stddevmag)
            ? (mm.stddevmag - stddevmag)
            : (stddevmag - mm.stddevmag);

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

    GV(class) = (move_less_error > stat_less_error) ? CLASS_MOVING : CLASS_STATIONARY;

    TRANSITION_TO(task_stats);
}

unsigned resultStationaryPct;
unsigned resultMovingPct;
unsigned sum;

void task_stats()
{
    PRIV(totalCount);
    PRIV(movingCount);
    PRIV(stationaryCount);

    ++GV(totalCount_bak);

    switch (GV(class)) {
        case CLASS_MOVING:
            ++GV(movingCount_bak);
            break;
        case CLASS_STATIONARY:
            ++GV(stationaryCount_bak);
            break;
    }

    if (GV(totalCount_bak) == SAMPLES_TO_COLLECT) {
        resultStationaryPct = GV(stationaryCount_bak) * 100 / GV(totalCount_bak);
        resultMovingPct = GV(movingCount_bak) * 100 / GV(totalCount_bak);
        sum = GV(stationaryCount_bak) + GV(movingCount_bak);
        COMMIT(totalCount);
        COMMIT(stationaryCount);
        COMMIT(movingCount);
        TRANSITION_TO(task_idle);
    } else {
        COMMIT(totalCount);
        COMMIT(stationaryCount);
        COMMIT(movingCount);
        TRANSITION_TO(task_sample);
    }
}

void task_warmup()
{
    PRIV(discardedSamplesCount);
    PRIV(seed);

    threeAxis_t_8 sample;

    if (GV(discardedSamplesCount_bak) < NUM_WARMUP_SAMPLES) {

        ACCEL_singleSample_(&sample);
        ++GV(discardedSamplesCount_bak);
        COMMIT(seed);
        COMMIT(discardedSamplesCount);
        TRANSITION_TO(task_warmup);
    } else {
        GV(trainingSetSize) = 0;
        COMMIT(seed);
        COMMIT(discardedSamplesCount);
        TRANSITION_TO(task_sample);
    }
}

void task_train()
{
    PRIV(trainingSetSize);

    switch (GV(class)) {
        case CLASS_STATIONARY:
            GV(model_stationary, _global_trainingSetSize_bak) = GV(features);
            break;
        case CLASS_MOVING:
            GV(model_moving, _global_trainingSetSize_bak) = GV(features);
            break;
    }

    ++GV(trainingSetSize_bak);

    if (GV(trainingSetSize_bak) < MODEL_SIZE) {
        COMMIT(trainingSetSize);
        TRANSITION_TO(task_sample);
    } else {
        COMMIT(trainingSetSize);
        TRANSITION_TO(task_idle);
    }
}

void task_idle()
{
    TRANSITION_TO(task_selectMode);
}


void init()
{
    msp_watchdog_disable();
    msp_gpio_unlock();

#if ENABLE_PRF
    PRF_INIT();
    PRF_POWER();
#endif

    // msp_clock_set_mclk(CLK_8_MHZ);

#if TSK_SIZ
    uart_init();
    cp_init();
#endif

#if LOG_INFO
    uart_init();
#endif

#if AUTO_RST
    msp_reseter_auto_rand(RST_TIME);
#endif

#if ENABLE_PRF
    if (first_run) {
        PRF_APP_BGN();
        first_run = 0;
    }
#endif
}


INIT_FUNC(init)
ENTRY_TASK(task_init)
