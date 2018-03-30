/******************************************************************************
 * File: ctlflags.h                                                           *
 *                                                                            *
 * Description: TODO                                                          *
 *                                                                            *
 * Note: this is an internal set of functionalities used by Coala's kernel,   *
 *       do not modify or use externally.                                     *
 *                                                                            *
 * Authors: Amjad Majid, Carlo Delle Donne                                    *
 ******************************************************************************/

#ifndef INCLUDE_CTLFLAGS_H
#define INCLUDE_CTLFLAGS_H


/**
 * Paging control.
 */
#define PAG_FAULT_CNTR 1

/**
 * Scheduling control.
 *
 * Select ONE of the following algorithms.
 */
#define NO_COALESCING 1
#define SLOW_ALG 0
#define FAST_ALG 0
#define FAST_TASK_AWARE_ALG 0
#define VALLEY_ALG 0
#define VALLEY_TASK_AWARE_ALG 0
#define WICKED_ALG 0

#if NO_COALESCING + \
	SLOW_ALG + \
	FAST_ALG + \
	FAST_TASK_AWARE_ALG + \
	VALLEY_ALG + \
	VALLEY_TASK_AWARE_ALG + \
	WICKED_ALG != 1
#error "Select one and only one algorithm"
#endif

/**
 * Logging control.
 */
#define COAL_TSK_SIZ_SEND 0
#define NUM_PWR_INTR_MEM_DUMP 0

/**
 * Profiling control.
 */
#define PROFILE_SCHEDULER 0
#define PROFILE_MEMORY    0

#define COALA_PRF_PORT    3
#define COALA_PRF_SCH_PIN 5
#define COALA_PRF_MEM_PIN 6


#endif /* INCLUDE_CTLFLAGS_H */
