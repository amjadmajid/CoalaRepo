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



void __sendPagTemp(unsigned int pagTag);
void __bringPagTemp(unsigned int pagTag);
void __bringPagROM(unsigned int pagTag);
void __sendPagROM(unsigned int pagTag);
unsigned int __pageSwap(unsigned int * varAddr);
void __pagsCommit();
void __bringCrntPagROM();

extern unsigned int CrntPagHeader;  // Holds the address of the first byte of a page

// Memory access interface

// TODO send the page to temp buffer only if we wrote to it
// #define __VAR_TAG(var)               ((uint16_t) (&(var)) )

#define __VAR_ADDR(var)                 ((unsigned int) (&(var)) )

#define __IS_VAR_IN_CRNT_PAG(var)       ( ( __VAR_ADDR(var)  >= CrntPagHeader ) && \
                                        ( __VAR_ADDR(var)  <  (CrntPagHeader+PAG_SIZE) ))

#define __VAR_PT_IN_RAM(var)            (  (__typeof__(var)*) (  (__VAR_ADDR(var) - CrntPagHeader) + RAM_PAG )  )




#define WVAR(var, val)  if( __IS_VAR_IN_CRNT_PAG(var) )\
                                { \
                                    *__VAR_PT_IN_RAM(var) = val ;\
                                }\
                                else{\
                                    __pageSwap(&(var)) ;\
                                    * __VAR_PT_IN_RAM(var) = val;\
                                    }


#define GWVAR(var, oper,val)  if( __IS_VAR_IN_CRNT_PAG(var) )\
                                { \
                                    *__VAR_PT_IN_RAM(var) oper val ;\
                                }\
                                else{\
                                    __pageSwap(&(var)) ;\
                                    * __VAR_PT_IN_RAM(var) oper val;\
                                    }


#define RVAR(var)   (\
                        (  __IS_VAR_IN_CRNT_PAG(var) ) ? \
                        ( * __VAR_PT_IN_RAM(var) ):\
                        ( *(  (__typeof__(var)*) ( (( __pageSwap(&(var)) +  __VAR_ADDR(var) ) - CrntPagHeader)  + RAM_PAG  )  )  )\
                    )


#define PVAR(var)   (\
                        (  __IS_VAR_IN_CRNT_PAG(var) ) ? \
                        ( __VAR_PT_IN_RAM(var) ):\
                        ( (  (__typeof__(var)*) ( (( __pageSwap(&(var)) +  __VAR_ADDR(var) ) - CrntPagHeader)  + RAM_PAG  )  )  )\
                    )

#define PPVAR(wvar, rvar)  __typeof__(rvar) __var##__var = (*PVAR(rvar)); \
                          (*PVAR(wvar)) = __var##__var


#define VAR(var) (__typeof__(var)*) __return_addr(&var)
#define P(var) (*((__typeof__(var)*) __return_addr(&var)))

#endif /* INCLUDE_DATAPROTEC_H_ */
















