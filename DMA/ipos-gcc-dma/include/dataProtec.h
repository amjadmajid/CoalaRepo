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


uint8_t* __return_addr(uint8_t* var) ;
void __sendPagTemp(unsigned int pagTag);
void __bringPagTemp(unsigned int pagTag);
void __bringPagROM(unsigned int pagTag);
void __sendPagROM(unsigned int pagTag);
unsigned int __pageSwap(unsigned int * varAddr);
void __pagsCommit();
void __bringCrntPagROM();

// Memory access interface

#define DIRTY_PAGE  1      // make it 0 to commit only the dirty pages [This macro does not work with P() ]

#define __VAR_ADDR(var)                 ((unsigned int) (&(var)) )

#define __IS_VAR_IN_CRNT_PAG(var)       ( ( __VAR_ADDR(var)  >= CrntPagHeader ) && \
                                        ( __VAR_ADDR(var)  <  (CrntPagHeader+PAG_SIZE) ))

#define __VAR_PT_IN_RAM(var)            (  (__typeof__(var)*) (  (__VAR_ADDR(var) - CrntPagHeader) + RAM_PAG )  )



#define WVAR(var, val)   if( __IS_VAR_IN_CRNT_PAG(var) )\
                                { \
                                    *__VAR_PT_IN_RAM(var) = val ;\
                                }\
                                else{\
                                    __pageSwap(&(var)) ;\
                                    * __VAR_PT_IN_RAM(var) = val;\
                                    }\
                                    dirtyPag = 1;


#define GWVAR(var, oper,val)  if( __IS_VAR_IN_CRNT_PAG(var) )\
                                { \
                                    *__VAR_PT_IN_RAM(var) oper val ;\
                                }\
                                else{\
                                    __pageSwap(&(var)) ;\
                                    * __VAR_PT_IN_RAM(var) oper val;\
                                    }


#define PVAR(var)   (\
                        (  __IS_VAR_IN_CRNT_PAG( (var) ) ) ? \
                        ( __VAR_PT_IN_RAM(var) ):\
                        ( (  (__typeof__(var)*) ( (( __pageSwap(&(var)) +  __VAR_ADDR(var) ) - CrntPagHeader)  + RAM_PAG  )  )  )\
                    )

#define RVAR(var)  *PVAR(var)

//#define RVAR(var)   (\
//                        (  __IS_VAR_IN_CRNT_PAG( (var) ) ) ? \
//                        ( * __VAR_PT_IN_RAM(var) ):\
//                        ( *(  (__typeof__(var)*) ( (( __pageSwap(&(var)) +  __VAR_ADDR(var) ) - CrntPagHeader)  + RAM_PAG  )  )  )\
//                    )


#define PPVAR(wvar, rvar)  __temp_temp = (*PVAR( (rvar) )); \
                          (*PVAR( (wvar) )) =  __temp_temp

#define OPPVAR(wvar, rvar) if(  __IS_VAR_IN_CRNT_PAG(var)  ) { \
                                __temp_temp =  (*__VAR_PT_IN_RAM(var)) ; \
                                WVAR(wvar, __temp_temp); \
                                }else{\
                                //TODO check if the page is in the buffer if so  *(&var + (0x3000)/2 )
                                // else var




#define VAR(var) (__typeof__(var)*) __return_addr(&var)
#define P(var) ( *VAR(var) )

#endif /* INCLUDE_DATAPROTEC_H_ */
















