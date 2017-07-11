#include <msp430.h> 
#include <stdint.h>
#include "dataProtec.h"

__p uint16_t rVar_p2 = 0x2222;
__p uint8_t fillPage_2[999] ={0};

__p uint16_t rVar_p1 = 0x1111;
__p uint8_t fillPage_1[999] ={0};




uint16_t res1 =0 ;
uint16_t res2 =0;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer


    res1 = __VAR_PT_IN_RAM(rVar_p1);

    __bringCrntPagROM();

    res1 = __VAR_PT_IN_RAM(rVar_p1);

    res1 = __IS_VAR_IN_CRNT_PAG(rVar_p1);
    res2 = __IS_VAR_IN_CRNT_PAG(rVar_p2);

    __bringCrntPagROM();

    __pageSwap(&rVar_p2);
    __pageSwap(&rVar_p1);

    // read a variable from the current page
    res1 =  RVAR(rVar_p1);
    res1++;
    // Write a variable from the current page
    WVAR(rVar_p1, res1);

    __no_operation();
     while(1)
     {
         ;
     }

	return 0;
}
