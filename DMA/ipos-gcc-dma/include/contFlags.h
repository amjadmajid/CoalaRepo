/*
 * contFalgs.h
 *
 *  Created on: 3 Nov 2017
 *      Author: amjad
 */

#ifndef CONTFLAGS_H_
#define CONTFLAGS_H_


/****************************************
        Paging control flags
****************************************/
#define PAG_FAULT_CNTR 1


/****************************************
        Coalescing control flags
****************************************/

// PLEASE SELECT AN ALGORITHIN
#define NO_COALESCING 0
#define SLO_CHNG_ALGO 0
#define FST_CHNG_ALGO 0
#define FST_CHNG_TSK_AWAR_ALGO 1

// algorithm selection flags control
#if     FST_CHNG_ALGO && SLO_CHNG_ALGO || \
        FST_CHNG_ALGO && FST_CHNG_TSK_AWAR_ALGO || \
        FST_CHNG_ALGO && NO_COALESCING || \
        SLO_CHNG_ALGO && FST_CHNG_TSK_AWAR_ALGO || \
        SLO_CHNG_ALGO && NO_COALESCING || \
        FST_CHNG_TSK_AWAR_ALGO && NO_COALESCING
#error "More than one algorithm is enabled"
#endif

#if FST_CHNG_ALGO ==0  && SLO_CHNG_ALGO ==0 && FST_CHNG_TSK_AWAR_ALGO ==0  && NO_COALESCING ==0
#error "No coalescing algorithm is selected"
#endif

// Logging
#define COAL_TSK_SIZ_SEND 1
#define NUM_PWR_INTR__MEM_DUMP 0

#endif /* CONTFLAGS_H_ */
