#include <msp430.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <mspReseter.h>
#include <mspProfiler.h>
#include <mspDebugger.h>
#include <mspbase.h>
#include <DSPLib.h>

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

#define N_SAMPLES     128
#define STAGE1_STEP   (2)
#define STAGE2_STEP   (STAGE1_STEP*2)
#define STAGE3_STEP   (STAGE2_STEP*2)

#if N_SAMPLES == 128
#define TWIDDLE_TABLE msp_cmplx_twiddle_table_128_q15
#elif N_SAMPLES == 256
#define TWIDDLE_TABLE msp_cmplx_twiddle_table_256_q15
#endif

static inline void msp_cmplx_overflow_q15(int16_t *src, bool *overflow);
static inline void msp_cmplx_btfly_auto_q15(int16_t *srcA, int16_t *srcB, const _q15 *coeff, bool *scale, bool *overflow);
static inline void msp_cmplx_btfly_c0_auto_q15(int16_t *srcA, int16_t *srcB, bool *scale, bool *overflow);
static inline void msp_cmplx_btfly_c1_auto_q15(int16_t *srcA, int16_t *srcB, bool *scale, bool *overflow);

// DSPLIB_DATA(fft_array_copy, MSP_ALIGN_FFT_Q15(N_SAMPLES))
// __nv _q15 fft_array_copy[N_SAMPLES];

// Predefined input signal.
#if N_SAMPLES == 128
#include "data_128.h"
#elif N_SAMPLES == 256
#include "data_256.h"
#endif

TASK(1, task_init)
TASK(2, task_sample)
TASK(3, task_fft_prologue)
TASK(4, task_fft_stage_1)
TASK(5, task_fft_stage_2)
TASK(6, task_fft_stage_3)
TASK(7, task_fft_epilogue)
TASK(8, task_done)

/* This is originally done by the compiler */
__nv uint8_t* data_src[N_SAMPLES + 3];
__nv uint8_t* data_dest[N_SAMPLES + 3];
__nv unsigned data_size[N_SAMPLES + 3];
GLOBAL_SB(_q15, fft_array_bak, N_SAMPLES);
GLOBAL_SB(uint16_t, fft_array_isDirty, N_SAMPLES);
GLOBAL_SB(uint16_t, iteration_bak);
GLOBAL_SB(uint16_t, fft_shift_bak);
GLOBAL_SB(bool, fft_scale_bak);
GLOBAL_SB(bool, fft_overflow_bak);
void clear_isDirty() {
    // PRINTF("clear\r\n");
    memset(&GV(fft_array_isDirty, 0), 0, sizeof(_global_fft_array_isDirty));
}
/* end */

GLOBAL_SB(_q15, fft_array, N_SAMPLES);
GLOBAL_SB(uint16_t, iteration);
GLOBAL_SB(uint16_t, fft_shift);
GLOBAL_SB(bool, fft_scale);
GLOBAL_SB(bool, fft_overflow);


static _q15 get_sample(const _q15* ptr, uint16_t sample_idx)
{
	__delay_cycles(20);
	return ptr[sample_idx];
}


void task_init()
{
#if ENABLE_PRF
    full_run_started = 1;
#endif

    PRIV(iteration);

    ++GV(iteration_bak);

    COMMIT(iteration);
    TRANSITION_TO(task_sample);
}


void task_sample()
{
	uint16_t sample_idx;

    const _q15* ptr;
    switch (GV(iteration) % 3) {
        case 1:
            ptr = a1;
            break;
        case 2:
            ptr = a2;
            break;
        case 0:
            ptr = a3;
            break;
        default:
            ptr = a1;
    }

	for (sample_idx = 0; sample_idx < N_SAMPLES; sample_idx++) {
		GV(fft_array, sample_idx) = get_sample(ptr, sample_idx);
	}

	TRANSITION_TO(task_fft_prologue);
}


void task_fft_prologue()
{
    _q15 fft_array_copy[N_SAMPLES];

    uint16_t i, length;

    // Pull array.
    for (i = 0; i < N_SAMPLES; i++) {
        DY_PRIV(fft_array, i);
        fft_array_copy[i] = GV(fft_array_bak, i);
    }

    length = N_SAMPLES / 2;
    
    // Bit reverse the order of the inputs.
    msp_cmplx_bitrev_q15_params paramsBitRev;
    paramsBitRev.length = length;
        
    // Perform bit reversal on source data.
    msp_cmplx_bitrev_q15(&paramsBitRev, fft_array_copy);

    // Push array.
    for (i = 0; i < N_SAMPLES; i++) {
        GV(fft_array_bak, i) = fft_array_copy[i];
        DY_COMMIT(fft_array, i);
    }

    TRANSITION_TO(task_fft_stage_1);
}


void task_fft_stage_1()
{
    _q15 fft_array_copy[N_SAMPLES];

    uint16_t length;
    bool scale;                         // scale flag
    bool overflow;                      // overflow flag
    uint16_t i, j;                      // loop counters
    int16_t *srcPtr;                    // local source pointer
    
    length = N_SAMPLES / 2;

    // Pull array.
    for (i = 0; i < N_SAMPLES; i++) {
        DY_PRIV(fft_array, i);
        fft_array_copy[i] = GV(fft_array_bak, i);
    }

    // Check for initial overflow.
    overflow = false;
    for (i = 0 ; i < length; i++) {
        msp_cmplx_overflow_q15(&fft_array_copy[i*2], &overflow);
    }

    // Stage 1.
    scale = overflow;
    overflow = false;

    // Initialize shift result.
    GV(fft_shift) = scale ? 1 : 0;
    
    if (STAGE1_STEP <= length) {
        for (j = 0; j < length; j += STAGE1_STEP) {
            srcPtr = fft_array_copy + j*2;
            msp_cmplx_btfly_c0_auto_q15(&srcPtr[0], &srcPtr[0+STAGE1_STEP], &scale, &overflow);
        }
    }

    // Push flags.
    GV(fft_scale) = scale;
    GV(fft_overflow) = overflow;

    // Push array.
    for (i = 0; i < N_SAMPLES; i++) {
        GV(fft_array_bak, i) = fft_array_copy[i];
        DY_COMMIT(fft_array, i);
    }

    TRANSITION_TO(task_fft_stage_2);
}


void task_fft_stage_2()
{
    PRIV(fft_scale);
    PRIV(fft_overflow);
    PRIV(fft_shift);

    _q15 fft_array_copy[N_SAMPLES];

    uint16_t length;
    bool scale;                         // scale flag
    bool overflow;                      // overflow flag
    uint16_t i, j;                      // loop counters
    int16_t *srcPtr;                    // local source pointer
    
    length = N_SAMPLES / 2;

    // Pull flags.
    scale = GV(fft_scale_bak);
    overflow = GV(fft_overflow_bak);

    // Pull array.
    for (i = 0; i < N_SAMPLES; i++) {
        DY_PRIV(fft_array, i);
        fft_array_copy[i] = GV(fft_array_bak, i);
    }

    // Stage 2.
    scale = overflow;
    overflow = false;
    GV(fft_shift_bak) += scale ? 1 : 0;
    if (STAGE2_STEP <= length) {
        for (j = 0; j < length; j += STAGE2_STEP) {
            srcPtr = fft_array_copy + j*2;
            msp_cmplx_btfly_c0_auto_q15(&srcPtr[0], &srcPtr[0+STAGE2_STEP], &scale, &overflow);
            msp_cmplx_btfly_c1_auto_q15(&srcPtr[2], &srcPtr[2+STAGE2_STEP], &scale, &overflow);
        }
    }

    // Push flags.
    GV(fft_scale_bak) = scale;
    GV(fft_overflow_bak) = overflow;

    // Push array.
    for (i = 0; i < N_SAMPLES; i++) {
        GV(fft_array_bak, i) = fft_array_copy[i];
        DY_COMMIT(fft_array, i);
    }

    COMMIT(fft_scale);
    COMMIT(fft_overflow);
    COMMIT(fft_shift);
    TRANSITION_TO(task_fft_stage_3);
}


void task_fft_stage_3()
{
    PRIV(fft_shift);

    _q15 fft_array_copy[N_SAMPLES];

    uint16_t length;
    bool scale;                         // scale flag
    bool overflow;                      // overflow flag
    uint16_t i, j;                       // loop counters
    uint16_t step;                      // step size
    uint16_t twiddleIndex;              // twiddle table index
    uint16_t twiddleIncrement;          // twiddle table increment
    int16_t *srcPtr;                    // local source pointer
    const _q15 *twiddlePtr;             // twiddle table pointer

    length = N_SAMPLES / 2;

    // Pull flags.
    scale = GV(fft_scale);
    overflow = GV(fft_overflow);

    // Pull array.
    for (i = 0; i < N_SAMPLES; i++) {
        DY_PRIV(fft_array, i);
        fft_array_copy[i] = GV(fft_array_bak, i);
    }

    // Initialize step size, twiddle angle increment and twiddle table pointer.
    step = STAGE3_STEP;
    twiddleIncrement = 2*(*(uint16_t*)TWIDDLE_TABLE)/STAGE3_STEP;
    twiddlePtr = &TWIDDLE_TABLE[DSPLIB_TABLE_OFFSET];
    
    // Stage 3 -> log2(step).
    while (step <= length) {
        // Reset the twiddle angle index.
        twiddleIndex = 0;
        
        // Set scale and overflow flags.
        scale = overflow;
        overflow = false;
        GV(fft_shift_bak) += scale ? 1 : 0;
        
        for (i = 0; i < (step/2); i++) {            
            // Perform butterfly operations on complex pairs.
            for (j = i; j < length; j += step) {
                srcPtr = fft_array_copy + j*2;
                msp_cmplx_btfly_auto_q15(srcPtr, srcPtr + step, &twiddlePtr[twiddleIndex], &scale, &overflow);
            }
            
            // Increment twiddle table index.
            twiddleIndex += twiddleIncrement;
        }
        // Double the step size and halve the increment factor.
        step *= 2;
        twiddleIncrement = twiddleIncrement/2;
    }

    // Push array.
    for (i = 0; i < N_SAMPLES; i++) {
        GV(fft_array_bak, i) = fft_array_copy[i];
        DY_COMMIT(fft_array, i);
    }

    COMMIT(fft_shift);
    TRANSITION_TO(task_fft_epilogue);
}


void task_fft_epilogue()
{
    uint16_t i;

    _q15 fft_array_copy[N_SAMPLES];

    // Pull array.
    for (i = 0; i < N_SAMPLES; i++) {
        DY_PRIV(fft_array, i);
        fft_array_copy[i] = GV(fft_array_bak, i);
    }

    // Initialize split operation params structure.
    msp_split_q15_params paramsSplit;
    paramsSplit.length = N_SAMPLES;
    paramsSplit.twiddleTable = TWIDDLE_TABLE;
    
    // Perform the last stage split operation to obtain N/2 complex FFT results.
    msp_split_q15(&paramsSplit, fft_array_copy);

    // Remove DC component.
    fft_array_copy[0] = 0;

    // Push array computing absolute value.
    for (i = 0; i < N_SAMPLES; i++) {
        if (fft_array_copy[i] >= 0) {
            GV(fft_array_bak, i) = fft_array_copy[i];
        } else {
            GV(fft_array_bak, i) = -fft_array_copy[i];
        }
        DY_COMMIT(fft_array, i);
    }

    TRANSITION_TO(task_done);
}


void task_done()
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

    // Set hardware multiplier to fractional mode.
#if defined(__MSP430_HAS_MPY32__)
    MPY32CTL0 = MPYFRAC | MPYDLYWRTEN;
#endif
}




ENTRY_TASK(task_init)
INIT_FUNC(init)


#define MSP_OVERFLOW_MAX        (INT16_MAX >> 2)
#define MSP_OVERFLOW_MIN        (INT16_MIN >> 2)

/*
 * Abstracted helper function to check for overflow.
 */
static inline void msp_cmplx_overflow_q15(int16_t *src, bool *overflow)
{
    if ((CMPLX_REAL(src) > MSP_OVERFLOW_MAX) || (CMPLX_REAL(src) < MSP_OVERFLOW_MIN)) {
        *overflow = true;
    }
    if ((CMPLX_IMAG(src) > MSP_OVERFLOW_MAX) || (CMPLX_IMAG(src) < MSP_OVERFLOW_MIN)) {
        *overflow = true;
    }
}

/*
 * Abstracted helper functions for a radix-2 butterfly operation. The following
 * operation is performed at each stage:
 *     A = A + coeff*B
 *     B = A - coeff*B
 * 
 * If overflow is detected the result is scaled by two:
 *     A = A/2
 *     B = A/2
 */
static inline void msp_cmplx_btfly_auto_q15(int16_t *srcA, int16_t *srcB, const _q15 *coeff, bool *scale, bool *overflow)
{
    /* Load coefficients. */
    _q15 tempR = CMPLX_REAL(coeff);
    _q15 tempI = CMPLX_IMAG(coeff);
    
    /* Calculate real and imaginary parts of coeff*B. */
    __q15cmpy(&tempR, &tempI, &CMPLX_REAL(srcB), &CMPLX_IMAG(srcB));
    
    /* Scale result if necessary. */
    if (*scale) {
        /* B = (A - coeff*B)/2 */
        CMPLX_REAL(srcB) = (CMPLX_REAL(srcA) - tempR) >> 1;
        CMPLX_IMAG(srcB) = (CMPLX_IMAG(srcA) - tempI) >> 1;
        
        /* A = (A + coeff*B)/2 */
        CMPLX_REAL(srcA) = (CMPLX_REAL(srcA) + tempR) >> 1;
        CMPLX_IMAG(srcA) = (CMPLX_IMAG(srcA) + tempI) >> 1;
    }
    else {
        /* B = A - coeff*B */
        CMPLX_REAL(srcB) = CMPLX_REAL(srcA) - tempR;
        CMPLX_IMAG(srcB) = CMPLX_IMAG(srcA) - tempI;
        
        /* A = A + coeff*B */
        CMPLX_REAL(srcA) = CMPLX_REAL(srcA) + tempR;
        CMPLX_IMAG(srcA) = CMPLX_IMAG(srcA) + tempI;
    }
    
    /* Check for possibility of overflow. */
    if (!*overflow) {
        msp_cmplx_overflow_q15(srcA, overflow);
        msp_cmplx_overflow_q15(srcB, overflow);
    }
}

/*
 * Simplified radix-2 butterfly operation for e^(-2*pi*(0/4)). This abstracted
 * helper function takes advantage of the fact the the twiddle coefficients are
 * positive and negative one for a multiplication by e^(-2*pi*(0/4)). The
 * following operation is performed at each stage:
 *     A = A + (1+0j)*B
 *     B = A - (1+0j)*B
 * 
 * If overflow is detected the result is scaled by two:
 *     A = A/2
 *     B = A/2
 */
static inline void msp_cmplx_btfly_c0_auto_q15(int16_t *srcA, int16_t *srcB, bool *scale, bool *overflow)
{
    int16_t tempR = CMPLX_REAL(srcB);
    int16_t tempI = CMPLX_IMAG(srcB);
    
    /* Scale result if necessary. */
    if (*scale) {
        /* B = (A - (1+0j)*B)/2 */
        CMPLX_REAL(srcB) = (CMPLX_REAL(srcA) - tempR) >> 1;
        CMPLX_IMAG(srcB) = (CMPLX_IMAG(srcA) - tempI) >> 1;
        
        /* A = (A + (1+0j)*B)/2 */
        CMPLX_REAL(srcA) = (CMPLX_REAL(srcA) + tempR) >> 1;
        CMPLX_IMAG(srcA) = (CMPLX_IMAG(srcA) + tempI) >> 1;
    }
    else {
        /* B = A - coeff*B */
        CMPLX_REAL(srcB) = CMPLX_REAL(srcA) - tempR;
        CMPLX_IMAG(srcB) = CMPLX_IMAG(srcA) - tempI;
        
        /* A = A + coeff*B */
        CMPLX_REAL(srcA) = CMPLX_REAL(srcA) + tempR;
        CMPLX_IMAG(srcA) = CMPLX_IMAG(srcA) + tempI;
    }
    
    /* Check for possibility of overflow. */
    if (!*overflow) {
        msp_cmplx_overflow_q15(srcA, overflow);
        msp_cmplx_overflow_q15(srcB, overflow);
    }
}

/*
 * Simplified radix-2 butterfly operation for e^(-2*pi*(1/4)). This abstracted
 * helper function takes advantage of the fact the the twiddle coefficients are
 * positive and negative one for a multiplication by e^(-2*pi*(1/4)).  The
 * following operation is performed at each stage:
 *     A = A + (0-1j)*B
 *     B = A - (0-1j)*B
 * 
 * If overflow is detected the result is scaled by two:
 *     A = A/2
 *     B = A/2
 */
static inline void msp_cmplx_btfly_c1_auto_q15(int16_t *srcA, int16_t *srcB, bool *scale, bool *overflow)
{
    int16_t tempR = CMPLX_REAL(srcB);
    int16_t tempI = CMPLX_IMAG(srcB);
    
    /* Scale result if necessary. */
    if (*scale) {
        /* B = (A - (0-1j)*B)/2 */
        CMPLX_REAL(srcB) = (CMPLX_REAL(srcA) - tempI) >> 1;
        CMPLX_IMAG(srcB) = (CMPLX_IMAG(srcA) + tempR) >> 1;
        
        /* A = (A + (0-1j)*B)/2 */
        CMPLX_REAL(srcA) = (CMPLX_REAL(srcA) + tempI) >> 1;
        CMPLX_IMAG(srcA) = (CMPLX_IMAG(srcA) - tempR) >> 1;
    }
    else {
        /* B = A - coeff*B */
        CMPLX_REAL(srcB) = CMPLX_REAL(srcA) - tempI;
        CMPLX_IMAG(srcB) = CMPLX_IMAG(srcA) + tempR;
        
        /* A = A + coeff*B */
        CMPLX_REAL(srcA) = CMPLX_REAL(srcA) + tempI;
        CMPLX_IMAG(srcA) = CMPLX_IMAG(srcA) - tempR;
    }
    
    /* Check for possibility of overflow. */
    if (!*overflow) {
        msp_cmplx_overflow_q15(srcA, overflow);
        msp_cmplx_overflow_q15(srcB, overflow);
    }
}
