/*
 * dataProtec.h
 *
 *  Created on: 16 May 2017
 *      Author: amjad
 */

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
                    // read the var from the current page
                    (*(__typeof__(var)*  ( (unsigned int)&var | BIGEN_RAM)))     = var:\
                    // do page swapping and read the var
                    ( *(__typeof__(var)* )(__pageSwap(&var)+( ((unsigned int)&var) | BIGEN_RAM) ) ) = var;

                    // check var in current page
#define RVAR(var)   ( __is_varInCrntPag( &var) )?\
                    // read the var from the current page
                    ( *(__typeof__(var)* )( ((unsigned int)&var) | BIGEN_RAM) ):\
                    // do page swapping and read the var
                    ( *(__typeof__(var)* )(__pageSwap(&var)+( ((unsigned int)&var) | BIGEN_RAM) ) );

#endif /* INCLUDE_DATAPROTEC_H_ */
