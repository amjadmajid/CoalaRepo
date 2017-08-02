#include <msp430.h> 
#include <stdint.h>
#include "dataProtec.h"

__p uint32_t rVar_p2 = 0x22222222;
__p uint8_t fillPage_2[999] ={0};

__p uint8_t fillPage_1[999] ={0};
__p uint32_t rVar_p1 = 0x11111111;

uint32_t __temp_temp=0;


volatile uint16_t res1 =0 ;
volatile uint16_t res2 =0;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.

    P3OUT &= ~BIT5;
    P3DIR |=BIT5;

    //******// Check the correctness of the address translation
    __bringCrntPagROM();            // bring the default page

    WP(rVar_p1) = 1;
    WP(rVar_p2) = 2;
    __sendPagTemp( CrntPagHeader );
    __pagsCommit();

    WP(rVar_p1)++;

    __sendPagTemp( CrntPagHeader );
    __pagsCommit();
    WP(rVar_p2) = 5;
    WP(rVar_p1) = *PVAR(rVar_p2);

    __sendPagTemp( CrntPagHeader );
    __pagsCommit();

    RVAR(rVar_p1)++;

    RVAR(rVar_p1)++;
    __sendPagTemp( CrntPagHeader );
    __pagsCommit();


    RVAR(rVar_p1) = 0;
    RVAR(rVar_p1)+=8;
    __sendPagTemp( CrntPagHeader );
    __pagsCommit();


    RVAR(rVar_p2) = 2;
    RVAR(rVar_p1) = 0;
    RVAR(rVar_p1) =     RVAR(rVar_p2) + 8;
    __sendPagTemp( CrntPagHeader );
    __pagsCommit();

    WVAR(rVar_p1, rVar_p1+1 );
    __sendPagTemp( CrntPagHeader );
    __pagsCommit();

    P3OUT |= BIT5;
    PPVAR(rVar_p1, rVar_p2);          // write a value into the RAM page
    P3OUT &= ~BIT5;
//
//    P3OUT |= BIT5;
//    res1 = RVAR(rVar_p1);          // write a value into the RAM page
//    P3OUT &= ~BIT5;
//
//    P3OUT |= BIT5;
//    res1 = RVAR(rVar_p2);          // write a value into the RAM page
//    P3OUT &= ~BIT5;

    __sendPagTemp( CrntPagHeader );
    __pagsCommit();

    __no_operation();
     while(1)
     {
         ;
     }

    return 0;
}
