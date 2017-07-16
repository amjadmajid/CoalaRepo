#include <msp430.h> 
#include <stdint.h>
#include "dataProtec.h"

__p uint16_t rVar_p2 = 0x2222;
__p uint8_t fillPage_2[999] ={0};

__p uint16_t rVar_p1 = 0x1111;
__p uint8_t fillPage_1[999] ={0};




volatile uint16_t res1 =0 ;
uint16_t res2 =0;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    //******// Check the correctness of the address translation
    __bringCrntPagROM();            // bring the default page
    res1 = RVAR(rVar_p1) + 1;           // read the variable
//    res1++;
    WVAR(rVar_p1, res1);          // write a value into the RAM page

    //******// Check the page swapping, Check the memory to see if the page1 went to its temporary location (0xDF70)
    res2 = RVAR(rVar_p2);           // read the variable
    res2++;
    WVAR(rVar_p2, res2);          // write a value into the RAM page

    //******// Check page swapping caused by writing to a variable
    WVAR(rVar_p1, res2);          // write a value into a page that is not in RAM

    WVAR(rVar_p2, res1);          // write a value into a page that is not in RAM


    //******// Check the commit process
    __pagsCommit();

    __no_operation();
     while(1)
     {
         ;
     }

	return 0;
}
