#include <msp430.h> 
#include <stdint.h>
#include <ipos.h>
#include "printf.h"

__p uint16_t volatile arr16_1[512] = {0};
__p uint16_t volatile arr16_2[512] = {0};
__p uint16_t volatile temp16 = 16;

__p uint32_t volatile arr1[256] = {0};
__p uint32_t volatile arr2[256] = {0};
__p uint32_t volatile temp = 8;

void task_test32_1() {
        *PVAR(arr1[0]) = 0x77777777;
        *PVAR(temp) = 0x11111111;
        //This syntax is problematic
        WVAR(temp, RVAR(arr1[0]));
}

void task_test32_2() {
        *PVAR(arr1[0]) = 0x77777777;
        *PVAR(temp) = 0x11111111;
        PPVAR(temp,  (arr1[0])) ;
}

void task_test16_1() {
        *PVAR(arr16_1[0]) = 0xaaaa;
        *PVAR(temp16) = 0xbbbb;
        P(temp16) = P(arr16_1[0]);
//        WVAR(temp16, RVAR(arr16_1[0]));
}


void task_test16_2() {
        *PVAR(arr16_1[0]) = 0xaaaa;
        *PVAR(temp16) = 0xbbbb;
        PPVAR(temp16,  (arr16_1[0])) ;
}

void init()
{
  WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
  // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
  PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
  P3DIR |=BIT5;
  P1DIR |=BIT0;
}
volatile uint16_t x = 90;
int main(void) {
    init();
       taskId tasks[] = {  {task_test32_1,0},
                           {task_test32_2,0},
                           {task_test16_1,0},
                           {task_test16_2,0},
       };
       //This function should be called only once
       os_addTasks(4, tasks );

       os_scheduler();
       return 0;
}
