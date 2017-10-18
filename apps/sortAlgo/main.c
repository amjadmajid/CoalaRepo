#include <msp430.h> 
#include <ipos.h>
#include <mspReseter.h>
#include "mspProfiler.h"
#include "mspDebugger.h"

#define CODEPROFILE 0
#define RAISE_PIN 1
#define PWR_INT_SIM 1

/*
void sortAlgo(int arr[], int arrLen){
    for (int i=0; i< arrLen-1;i++){
        for(int j=i+1; j < arrLen; j++){
            if(arr[i] >  arr[j]) { //ascending order
                int temp = arr[j];
                arr[j] = arr[i];
                arr[i] = temp;
            }
        }
    }
}
*/
///// function Prototypes
void task_finish();
void task_outer_loop();
void task_inner_loop();

////// Global variables
__p unsigned int arr[] = {3,1,4,6,9,5,10,8,16,20,19,40,16,17,2,41,80,100,5,89,66,77,8,3,32,55,8,11,99,65,25,89,3,22,25,121,11,90,74,1,12,39,54,20,22,43,45,90,81,40};
    unsigned int arr2[] = {3,1,4,6,9,5,10,8,16,20,19,40,16,17,2,41,80,100,5,89,66,77,8,3,32,55,8,11,99,65,25,89,3,22,25,121,11,90,74,1,12,39,54,20,22,43,45,90,81,40};

    unsigned int arr_len = 50;
__p unsigned int i =  0;
__p unsigned int j = 1;

__nv unsigned protect = 0;

/////  TASKS
unsigned int in_i, in_j, arr_i, arr_j;
void task_inner_loop()
{
#if CODEPROFILE
    cp_reset();
#endif

#if RAISE_PIN
    protect = 1;
#endif

    in_i = RP(i);
    in_j = RP( j);
    arr_i = RP( arr[ in_i ]);
    arr_j = RP( arr[ in_j ]);


    if( arr_i  > arr_j )
    {
        unsigned int temp = arr_j;
        arr_j =  arr_i;
        arr_i =  temp;
    }

    if( in_j < (arr_len-1) )
    {
        os_jump(0);
    }


    WP( arr[ in_i ] )= arr_i;
    WP( arr[ in_j ]) = arr_j;
    in_j++;
    WP(j) = in_j;
#if CODEPROFILE
    cp_sendRes("task_inner_loop \0");
#endif
}


void task_outer_loop()
{

#if CODEPROFILE
    cp_reset();
#endif
    unsigned int in_i;
    in_i = RP(i);
    in_i++;

    if(in_i < arr_len)
    {
        os_jump(2);
    }

    WP(i)= in_i;
    WP(j)= in_i+1;

#if CODEPROFILE
    cp_sendRes( "task_outer_loop \0");
#endif
}



void task_finish()
{
#if CODEPROFILE
    cp_reset();
#endif

#if RAISE_PIN
    if(protect){
    P3OUT |=BIT5;
    P3OUT &=~BIT5;
    }
    protect=0;
#endif

    unsigned cct;
    for(cct=0; cct< arr_len; cct++)
    {
        arr[cct] =  arr2[cct];
    }


    WVAR(i, 0) ;
    WVAR(j, 1);

#if CODEPROFILE
    cp_sendRes("\ntask_finish \0");
#endif
}

void init()
{
  WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
  // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
  PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.

#if RAISE_PIN
  P3OUT &=~BIT5;
  P3DIR |=BIT5;
#endif

#if 0
  CSCTL0_H = CSKEY >> 8;                // Unlock CS registers
//    CSCTL1 = DCOFSEL_4 |  DCORSEL;      // Set DCO to 16MHz
  CSCTL1 = DCOFSEL_6;                   // Set DCO to 8MHz
  CSCTL2 =  SELM__DCOCLK;               // MCLK = DCO
  CSCTL3 = DIVM__1;                     // divide the DCO frequency by 1
  CSCTL0_H = 0;
#endif

#if CODEPROFILE
  cp_init();
#endif

#if PWR_INT_SIM

  uart_init();
  mr_auto_rand_reseter(13000); // every 13 msec the MCU will be reseted

#endif

}

int main(void) {
    init();

       taskId tasks[] = {  {task_inner_loop,1}, {task_outer_loop,2}, {task_finish,3}};
       //This function should be called only once
       os_addTasks(3, tasks );

       os_scheduler();
       return 0;
}
