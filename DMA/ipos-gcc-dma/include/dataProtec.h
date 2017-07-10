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
#define BIFEN_ROM       0x4400
#define BIGEN_RAM       0x1C00
#define END_RAM         0x23FE
#define PAG_SIZE        0x100  // 1KB
#define NUM_PAG         8
#define PAG_ID_MSK      0xf
#define TOT_PAG_SIZE    (PAG_SIZE * NUM_PAG)
#define PAG_TAG_MSK     0xfc00



uint16_t __is_varInCrntPag(uint16_t * var);

void __sendPagTemp(uint16_t pagTag);
void __bringPagTemp(uint16_t pagTag);
void __bringPagROM(uint16_t pagTag);
void __sendPagROM(uint16_t pagTag);
void __pagsCommit();
void __bringCrntPagROM();

// Memory Access interface

// check var in current page
//#define WVAR(var, val)   ( __is_varInCrntPag( &var) )?\
//                    (*(__typeof__(var)*  ( (unsigned int)(&var)) | BIGEN_RAM)))   = val :\
//                    ( *(__typeof__(var)* )(__pageSwap(&var)+( ((unsigned int)&var) | BIGEN_RAM) ) ) = val;

                                                //TODO make the 6 MSb with 0
#define WVAR(var, val)   *(  (__typeof__(var)*) ( (uint16_t)(&var) | (END_RAM-PAG_SIZE) )  ) =\
                         ( __is_varInCrntPag( &var) ) ? val : __pageSwap(&var)+val

                    // check var in current page
#define RVAR(var)   ( __is_varInCrntPag( &var) )?\
                    ( *(__typeof__(var)* )( ((unsigned int)(&var)) | (END_RAM-PAG_SIZE) ) ):\
                    ( *(__typeof__(var)* )(__pageSwap(&var)+( ((unsigned int)&var) | BIGEN_RAM) ) )

#endif /* INCLUDE_DATAPROTEC_H_ */
















