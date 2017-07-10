/*
 * dataProtec.h
 *
 *  Created on: 16 May 2017
 *      Author: amjad
 */

#include <msp430fr5969.h>
#include <stdint.h>

#ifndef INCLUDE_DATAPROTEC_H_
#define INCLUDE_DATAPROTEC_H_


#define __nv  __attribute__((section(".nv_vars")))
#define __p  __attribute__((section(".p_vars")))


/*
 * Max available memory is 48/2 = 24
 * Page size = 1KB
 * Maximum number of supported pages are 8
 * the tags (6 MSb)
 * 0000 01 (0x0400) (1KB)
 * 0000 10 (0x0800) (2KB)
 * 0000 11 (0x0c00) (3KB)
 * 0001 00 (0x1000) (4KB)
 * 0001 01 (0x1400) (5KB)
 * 0001 10 (0x1800) (6KB)
 * 0001 11 (0x1c00) (7KB)
 * 0010 00 (0x2000) (8KB)
 */


// The location of any page is given by (BIGEN_ROM + pag_tag - PAG_SIZE)
// The TEMP location of any page is given by ( END_ROM - TOT_PAG_SIZE + pag_tag - PAG_SIZE )
#define END_ROM         0xFF7F
#define END_RAM         0x23FF
#define TAG_SIZE        6
#define PAG_ADDR_SIZE   (16 - TAG_SIZE)
#define NUM_PAG         8
#define PAG_SIZE        0x0400  // 1KB
#define MS6B            0xfc00
#define RAM_PAG         (END_RAM - PAG_SIZE)
#define TOT_PAG_SIZE    (PAG_SIZE * NUM_PAG)
#define PAG_SIZE_W      (PAG_SIZE/2)  //1KB
#define BIGEN_ROM       ( END_ROM - TOT_PAG_SIZE - TOT_PAG_SIZE  ) // 0xBF70


void __sendPagTemp(uint16_t pagTag);
void __bringPagTemp(uint16_t pagTag);
void __bringPagROM(uint16_t pagTag);
void __sendPagROM(uint16_t pagTag);
void __pagsCommit();
void __bringCrntPagROM();


//TODO check what happened when page size changed
uint16_t __pagsInTemp[NUM_PAG] = {0};
__nv uint16_t __persis_pagsInTemp[NUM_PAG] = {0};

// 1KB
// TODO extra NV copy
uint16_t CrntPagTag = BIGEN_ROM;
__nv uint8_t pageCommit = 0;


// Memory access interface

// TODO send the page to temp buffer only if we wrote to it
#define __VAR_TAG(var)  				((uint16_t) (&(var))   &  MS6B )

#define __VAR_ADDR(var)					((uint16_t) (&(var))   & ~MS6B )

#define __IS_VAR_IN_CRNT_PAG(var)     	( ( __VAR_TAG(var)  >= CrntPagTag ) && \
                         				( __VAR_TAG(var)  <  CrntPagTag+PAG_SIZE))

#define __VAR_PT_IN_RAM(var)			(  (__typeof__(var)*) (  __VAR_ADDR(var) + RAM_PAG )  )

#define WVAR(var, val)  * __VAR_PT_IN_RAM(var) =\
                        (   __IS_VAR_IN_CRNT_PAG(var)  ) ? val : __pageSwap(&var)+val

#define RVAR(var)   (  __IS_VAR_IN_CRNT_PAG(var) ) ? \
                        ( * __VAR_PT_IN_RAM(var) ):\
                    	( *(  (__typeof__(var)*) ( __pageSwap(&var) + ( __VAR_ADDR(var) + RAM_PAG ) )  ) )

#endif /* INCLUDE_DATAPROTEC_H_ */
















