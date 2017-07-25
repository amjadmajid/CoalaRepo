#include <msp430.h> 
#include <stdint.h>
#include <ipos.h>


__p uint32_t volatile arr1[256] = {0};
__p uint32_t volatile arr2[256] = {0};
__p uint32_t volatile temp = 8;

void task_test() {
        *PVAR(arr1[0]) = 0x77777777;
        *PVAR(temp) = 0x11111111;
        *PVAR(temp) =  (*PVAR(arr1[0]))+1;
}


void task_test1() {
        *PVAR(arr1[0]) = 0x77777777;
        *PVAR(temp) = 0x11111111;
        *PVAR(temp) =  (*PVAR(arr1[0]))+1;
}

void init()
{
  WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
  // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
  PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
  P3DIR |=BIT5;
  P1DIR |=BIT0;
}

int main(void) {
    init();

       taskId tasks[] = {  {task_test,0},{task_test1,0}};
       //This function should be called only once
       os_addTasks(2, tasks );

       os_scheduler();
       return 0;
}
