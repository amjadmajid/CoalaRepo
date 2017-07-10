#include <msp430.h> 
#include <stdint.h>
#include "dataProtec.h"

__p uint16_t rVar = 0x9191;
uint16_t * pag1_var =    (uint16_t *) 0xBF7f;
uint16_t * pag2_var =    (uint16_t *) 0xBB7F;

void init()
{
    *pag1_var = 0xabc;
    *pag2_var =  0xcba;

    __bringCrntPagROM();
}

uint16_t readVar()
{
    if( RVAR(*pag1_var) == 0xabc )
    {
        return 1;
    }
    return 0;
}


uint16_t readVarPagSwap()
{
    // check the page tracking table -- it should be clear so no DMA flush

    if( RVAR(*pag2_var) == 0xcba )
    {
        return 1;
    }
    return 0;
}

uint16_t writeVar()
{
    WVAR(*pag2_var, 0xdead );

        // check if the SRAM is modified

        // check if FRAM is not modified

        return 1;

}

uint16_t pagesCommit()
{
    // check if the FRAM is modified

    //
    __pagsCommit();
}
uint16_t res1 ;
uint16_t res2;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    init();

    * (uint16_t *)  0x1ffe   = 0xbeaf;
    //  > Send data from RAM to temp buffer ( 0x400 bytes from 0x1ff0 to 0xDF70 )
    __sendPagTemp( 0x0400 );

    * (uint16_t *)  0xDF7e   = 0xabcd;
    // < bring data from  temp buffer to RAM(0xDF70 --> 0x1ff0 send 0x400 bytes )
    __bringPagTemp(0x0400);

    * (uint16_t *)  0x1ffe   = 0x5555;
    //> Send data from RAM to ROM ( 0x400 bytes from 0x1ff0 to 0x4400 )
    __sendPagROM(0x0400);
    * (uint16_t *)  0xBF7e   = 0x1010;
    // < bring data from ROM to RAM (0x4400 --> 0x1ff0 send 0x400 bytes )
    __bringPagROM(0x0400);

     res1 = readVar() ;
     res2 = readVarPagSwap() ;

     while(1)
     {
         ;
     }

	return 0;
}
