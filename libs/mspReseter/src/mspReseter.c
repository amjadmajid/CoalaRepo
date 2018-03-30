#include <mspReseter.h>

#define __nv  __attribute__((section(".nv_vars")))

#define FIXED  1
#define CAMEL  0
#define TOWARD 0

#if FIXED || CAMEL
    __nv int noise[] = {
    #include "powerPatterns.txt"
    };
#elif TOWARD
    __nv unsigned int noise[] = {
    #include "powerPatterns.txt"
    };
#endif

__nv unsigned int noise_idx = 0;

__nv unsigned reset = 1;


void msp_reseter_reset()
{
    if (reset) {
        reset = 0;
        // Generate reset.
        PMMCTL0 = PMMPW | PMMSWBOR;
    } else {
        reset = 1;
    }
}


void msp_reseter_confirm()
{
    if (reset) {
        reset = 0;
        // Generate reset.
        PMMCTL0 = PMMPW | PMMSWBOR;
    } else {
        reset = 1;
    }
}


void msp_reseter_auto(unsigned int interval)
{
    // Enable timer interrupt.
    TA0CCTL0 = CCIE;

    // Set compare register.
    TA0CCR0 = interval;

    // SMCLK, continuous mode.
    TA0CTL = TASSEL__SMCLK | MC__UP | TACLR;

    // Enable general interrupt.
    __bis_SR_register(GIE);
}


void msp_reseter_auto_rand(unsigned int interval)
{
    long int temp_interval = (long int) interval + noise[noise_idx];
    if (temp_interval < 0) {
        interval = 10;
    } else if (temp_interval > 65535) {
        interval = 0xffff;
    } else {
        interval = temp_interval;
    }

    noise_idx++;
    if (noise_idx == 200) {
        noise_idx = 0;
    }
    
    msp_reseter_auto(interval & 0xfffe);
}


unsigned int calculate_interval(unsigned int clock_freq, unsigned int hit_to_miss_ratio, unsigned int capacitance)
{
    unsigned int current_matrix[] = {
        370, 1280, 2510, 2650,
        240, 745,  1440, 1990,
        200, 560,  1070, 1620,
        170, 480,  890,  1420,
        110, 235,  420,  730
    };

    unsigned int x, y;

    if (clock_freq < 4) {
        x = 0;
    }
    else if (clock_freq < 8) {
        x = 1;
    }
    else if (clock_freq < 16) {
        x = 2;
    }
    else if (clock_freq == 16) {
        x = 3;
    }
    else {
        return 0xffff;
    }

    if (hit_to_miss_ratio < 50) {
        y = 0;
    }
    else if (hit_to_miss_ratio < 66) {
        y = 1;
    }
    else if (hit_to_miss_ratio < 75) {
        y = 2;
    }
    else if (hit_to_miss_ratio < 100) {
        y = 3;
    }
    else if (hit_to_miss_ratio == 100) {
        y = 4;
    }
    else {
        return 0xffff;
    }

    unsigned int current = current_matrix[y * 4 + x];

    unsigned int dt = 1000 * capacitance * (3300 - 2200) / current;

    return (dt / clock_freq);
}


/**
 * Timer A0 interrupt service routine.
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer0_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    // Disable general interrupt.
    __bic_SR_register(GIE);

    __no_operation();
    
    // Generate reset.
    PMMCTL0 = PMMPW | PMMSWBOR;
}
