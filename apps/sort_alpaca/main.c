#include <msp430.h>
#include <stdlib.h>

#include <mspReseter.h>
#include <mspProfiler.h>
#include <mspDebugger.h>
#include <mspbase.h>

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

#define LENGTH 100

const uint16_t a1[LENGTH] = {
      3,   1,   4,   6,   9,   5,  10,   8,  16,  20,
     19,  40,  16,  17,   2,  41,  80, 100,   5,  89,
     66,  77,   8,   3,  32,  55,   8,  11,  99,  65,
     25,  89,   3,  22,  25, 121,  11,  90,  74,   1,
     12,  39,  54,  20,  22,  43,  45,  90,  81,  40,
      3,   1,   4,   6,   9,   5,  10,   8,  16,  20,
     19,  40,  16,  17,   2,  41,  80, 100,   5,  89,
     66,  77,   8,   3,  32,  55,   8,  11,  99,  65,
     25,  89,   3,  22,  25, 121,  11,  90,  74,   1,
     12,  39,  54,  20,  22,  43,  45,  90,  81,  40
};

const uint16_t a2[LENGTH] = {
    121, 177,  50,  55,  32, 173, 164, 132,  17, 174,
     61, 186,  96,  44, 120, 181,  24, 134,  68, 167,
      8,  26, 144, 138, 133,  48,  80,  60,  39,   6,
     86, 126, 154,  42, 150, 113, 105,  52, 139, 175,
     58,  98,  31, 182,  74, 169,   4,  23, 157, 189,
     72, 130,   9,  19,  12,  67,   2,  16,  49,  57,
     69,  94, 145, 136,  99, 152, 198,  59, 153, 127,
     92,  13,  14, 160,  35, 194, 107,  89, 199, 155,
    163, 156,  93, 140, 111,  63,  15,  79,  22, 159,
     65,  78,  81, 122, 135, 180,  76,   3,  38, 102
};

// Tasks.
TASK(1, task_init)
TASK(2, task_finish)
TASK(3, task_outer_loop)
TASK(4, task_inner_loop)

/* This is originally done by the compiler */
__nv uint8_t* data_src[LENGTH + 1];
__nv uint8_t* data_dest[LENGTH + 1];
__nv unsigned data_size[LENGTH + 1];
GLOBAL_SB(uint16_t, array_bak, LENGTH);
GLOBAL_SB(uint16_t, array_isDirty, LENGTH);
GLOBAL_SB(uint16_t, outer_idx_bak);
GLOBAL_SB(uint16_t, inner_idx_bak);
GLOBAL_SB(uint8_t, iteration_bak);
void clear_isDirty() {
    // PRINTF("clear\r\n");
    memset(&GV(array_isDirty, 0), 0, sizeof(_global_array_isDirty));
}
/* end */

// Task-shared protected variables.
GLOBAL_SB(uint16_t, array, LENGTH);
GLOBAL_SB(uint16_t, outer_idx);
GLOBAL_SB(uint16_t, inner_idx);
GLOBAL_SB(uint8_t, iteration);


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


void task_init()
{
#if ENABLE_PRF
    full_run_started = 1;
#endif

    PRIV(iteration);

    GV(outer_idx) = 0;
    GV(inner_idx) = 1;

    const uint16_t* array_pt;

    ++GV(iteration_bak);
    if (GV(iteration_bak) & 0x01) {
        array_pt = a1;
    } else {
        array_pt = a2;
    }

    uint16_t idx;
    for (idx = 0; idx < LENGTH; idx++) {
        GV(array, idx) = array_pt[idx];
    }

    COMMIT(iteration);
    TRANSITION_TO(task_inner_loop);
}


void task_inner_loop()
{
    PRIV(inner_idx);

    uint16_t i, j, x_i, x_j, temp;

    i = GV(outer_idx);
    j = GV(inner_idx_bak);

    DY_PRIV(array, i);
    x_i = GV(array_bak, i);

    DY_PRIV(array, j);
    x_j = GV(array_bak, j);

    if (x_i > x_j) {
        temp = x_j;
        x_j =  x_i;
        x_i =  temp;
    }

    GV(array_bak, i) = x_i;
    DY_COMMIT(array, i);

    GV(array_bak, j) = x_j;
    DY_COMMIT(array, j);

    ++GV(inner_idx_bak);

    if (GV(inner_idx_bak) < LENGTH) {
        COMMIT(inner_idx);
        TRANSITION_TO(task_inner_loop);
    } else {
        COMMIT(inner_idx);
        TRANSITION_TO(task_outer_loop);
    }
}


void task_outer_loop()
{
    PRIV(outer_idx);

    ++GV(outer_idx_bak);
    GV(inner_idx) = GV(outer_idx_bak) + 1;

    if (GV(outer_idx_bak) < LENGTH - 1) {
        COMMIT(outer_idx);
        TRANSITION_TO(task_inner_loop);
    } else {
        COMMIT(outer_idx);
        TRANSITION_TO(task_finish);
    }
}


void task_finish()
{
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

    TRANSITION_TO(task_init);
}


ENTRY_TASK(task_init)
INIT_FUNC(init)
