/*
 * dataProtec.h
 *
 *  Created on: 16 May 2017
 *      Author: amjad
 */

#include <msp430fr5969.h>
#include <stdint.h>
#include "global.h"

#ifndef INCLUDE_DATAPROTEC_H_
#define INCLUDE_DATAPROTEC_H_

/*
 * Max available memory is 48/2 = 24
 * Page size = 1KB
 * Maximum number of supported pages are 8
 * This code requires linker script modification to put the variables
 * in a known location (the pages locations 0xBF70)
 */



uint8_t* __return_addr_no_check(uint8_t*var) ;
uint8_t* __return_addr_wr_no_check(uint8_t* var) ;
void __sendPagTemp(unsigned int from, unsigned int to);
void __movPag(unsigned int from, unsigned int to);
void __pageSwap(uint8_t * varAddr, uint8_t * dirtyPag, unsigned int *curtPagHdr, unsigned int ramPagsBuf  );


void __pagsCommit();

// Memory access interface

#define CLR_DIRTY_PAGE  0

//#define __VAR_ADDR(var)                 ((unsigned int) (&(var)) )

#define __IS_VAR_IN_CRNT_PAG(var, ramPagsBuf)       ( ( ((unsigned int)var)  >= ramPagsBuf.crntPagHdr ) && ( ((unsigned int)var)   <  (ramPagsBuf.crntPagHdr+PAG_SIZE) ))

#define __VAR_PT_IN_RAM_PG(var, ramPagsBuf)     (  (__typeof__(var)*) (  (((unsigned int)var) - ramPagsBuf.crntPagHdr) + ramPagsBuf.ramPagAddr )  )


#define __PP(wvar, rvar)  __temp_temp = RP(rvar) ; \
                              WP(var) =  __temp_temp

/******************************************
 *        User Memory Interface
 ******************************************/
#define __RP__(var) (__typeof__(var)*) __return_addr_no_check(&var)
#define __WP__(var) (__typeof__(var)*) __return_addr_wr_no_check(&var)

#define WP(var)         ( *__WP__(var) )
#define RP(var)         ( *__RP__(var) )

#define PP(wvar, rvar)  ( __PP(wvar, rvar) )

/******************************************
 *      Compiler Memory Interface
 ******************************************/

#define VAR(var) (__typeof__(var)*) __return_addr(&var)
#define VARW(var) (__typeof__(var)*) __return_addr_wr(&var)
#define P(var) ( *VAR(var) )
#define PW(var) ( *VARW(var) )

#endif /* INCLUDE_DATAPROTEC_H_ */
















