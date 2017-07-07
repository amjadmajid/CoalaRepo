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


uint16_t __is_varInCrntPag(uint16_t * var);

void __sendPagTemp(uint16_t pagTag);
void __bringPagTemp(uint16_t pagTag);
void __bringPagROM(uint16_t pagTag);
void __sendPagROM(uint16_t pagTag);
void __pagsCommit();


// Memory Access interface

// check var in current page
#define WVAR(var)   ( __is_varInCrntPag( &var) )?\
                    (*(__typeof__(var)*  ( (unsigned int)&var | BIGEN_RAM)))     = var:\
                    ( *(__typeof__(var)* )(__pageSwap(&var)+( ((unsigned int)&var) | BIGEN_RAM) ) ) = var;

                    // check var in current page
#define RVAR(var)   ( __is_varInCrntPag( &var) )?\
                    ( *(__typeof__(var)* )( ((unsigned int)&var) | BIGEN_RAM) ):\
                    ( *(__typeof__(var)* )(__pageSwap(&var)+( ((unsigned int)&var) | BIGEN_RAM) ) );

#endif /* INCLUDE_DATAPROTEC_H_ */
