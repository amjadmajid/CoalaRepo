#include <msp430.h>
#include <stdint.h>

#include <mspReseter.h>
#include <mspProfiler.h>
#include <mspDebugger.h>
#include <mspbase.h>

#include <coala.h>

// Profiling defines and flags.
#define PRF_PORT 3
#define PRF_PIN  4
#define RST_PIN  5
#if RAISE_PIN
__nv uint8_t full_run_started = 0;
__nv uint8_t first_run = 1;
#endif

#ifndef RST_TIME
#define RST_TIME 25000
#endif

#define LENGTH 100

/*
void sortAlgo(int arr[], int arrLen){
    for (int i=0; i< arrLen-1;i++){
        for(int j=i+1; j < arrLen; j++){
            if(arr[i] >  arr[j]) { //ascending order
                int temp = arr[j];
                arr[j] = arr[i];
                arr[i] = temp;
            }
        }
    }
}
*/

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
COALA_TASK(task_init, 5)
COALA_TASK(task_finish, 1)
COALA_TASK(task_outer_loop, 1)
COALA_TASK(task_inner_loop, 1)

// Task-shared protected variables.
COALA_PV(uint16_t, array, LENGTH);
COALA_PV(uint16_t, outer_idx);
COALA_PV(uint16_t, inner_idx);
COALA_PV(uint8_t, iteration);

uint16_t in_i, in_j, arr_i, arr_j;


void task_init()
{
#if RAISE_PIN
    full_run_started = 1;
#endif

    WP(outer_idx) = 0;
    WP(inner_idx) = 1;

    const uint16_t* array_pt;

    ++WP(iteration);
    if (RP(iteration) & 0x01) {
        array_pt = a1;
    } else {
        array_pt = a2;
    }

    uint16_t idx;
    for (idx = 0; idx < LENGTH; idx++) {
        WP(array[idx]) = array_pt[idx];
    }

    coala_next_task(task_inner_loop);
}


void task_inner_loop()
{
    uint16_t i, j, x_i, x_j, temp;

    i = RP(outer_idx);
    j = RP(inner_idx);

    x_i = RP(array[i]);
    x_j = RP(array[j]);

    if (x_i > x_j) {
        temp = x_j;
        x_j =  x_i;
        x_i =  temp;
    }

    WP(array[i]) = x_i;
    WP(array[j]) = x_j;
    ++WP(inner_idx);

    if (RP(inner_idx) < LENGTH) {
        coala_next_task(task_inner_loop);
    } else {
        coala_next_task(task_outer_loop);
    }
}


void task_outer_loop()
{
    ++WP(outer_idx);
    WP(inner_idx) = RP(outer_idx) + 1;

    if (RP(outer_idx) < LENGTH - 1) {
        coala_next_task(task_inner_loop);
    } else {
        coala_next_task(task_finish);
    }
}


void task_finish()
{
#if RAISE_PIN
    if (full_run_started) {
#if AUTO_RST
        msp_reseter_halt();
#endif
        msp_gpio_spike(PRF_PORT, PRF_PIN);
        full_run_started = 0;
        coala_force_commit();
#if AUTO_RST
        msp_reseter_resume();
#endif
    }
#endif

    coala_next_task(task_init);
}


void init()
{
    msp_watchdog_disable();
    msp_gpio_unlock();

#if RAISE_PIN
    msp_gpio_clear(PRF_PORT, 4);
    msp_gpio_clear(PRF_PORT, 5);
    msp_gpio_clear(PRF_PORT, 6);
    msp_gpio_dir_out(PRF_PORT, 4);
    msp_gpio_dir_out(PRF_PORT, 5);
    msp_gpio_dir_out(PRF_PORT, 6);
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
    msp_gpio_set(PRF_PORT, RST_PIN);
}


int main(void)
{
    init();

    coala_init(task_init);

#if RAISE_PIN
    if (first_run) {
        msp_gpio_spike(PRF_PORT, PRF_PIN);
        first_run = 0;
    }
#endif

    coala_run();

    return 0;
}
