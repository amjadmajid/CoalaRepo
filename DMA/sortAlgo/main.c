#include <msp430.h> 
#include <libipos-gcc-dma/ipos.h>
#include <libio/log.h>
#include <libmspbuiltins/builtins.h>
#include <libio/log.h>
#include <libmsp/mem.h>
#include <libmsp/periph.h>
#include <libmsp/clock.h>
#include <libmsp/watchdog.h>
#include <libmsp/gpio.h>

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
//DVAR( unsigned int arr[], ={3,1,4,6,9,5,10,8,16,20} );
__p unsigned int arr[] = {3,1,4,6,9,5,10,8,16,20};
     unsigned int arr_len = 10;
__p unsigned int i =  0;
__p unsigned int j = 1;

/////  TASKS
void task_inner_loop()
{

	PRINTF("inner loop %u %u\r\n", P(i), P(j));
	for (unsigned k = 0; k < 10; ++k) {
		PRINTF("arr[%u]: %u\r\n", k, P(arr[k]));
	}

    if( P(arr[P(i)])  > P(arr[P(j)]) )
    {
        unsigned int temp = P(arr[P(j)]);
        P(arr[P(j)]) = P(arr[P(i)]);
        P(arr[P(i)])=  P(temp);
    }

    if(P(j) < arr_len - 1)
    {
        os_jump(0);
    }
    P(j)++;
}

void task_outer_loop()
{
	PRINTF("outer loop %u %u\r\n", P(i), P(j));
	for (unsigned k = 0; k < 10; ++k) {
		PRINTF("arr[%u]: %u\r\n", k, P(arr[k]));
	}
    P(i)++;

    if(P(i) < P(arr_len))
    {
        os_jump(2);
    }
	P(j) = P(i) + 1;
}

void task_finish()
{
	PRINTF("FINISH\r\n");
	for (unsigned k = 0; k < 10; ++k) {
		PRINTF("arr[%u]: %u\r\n", k, P(arr[k]));
	}
	while(1);
    P3OUT |=BIT5;
    P3OUT &=~BIT5;
}

static void init_hw()
{
	msp_watchdog_disable();
	msp_gpio_unlock();
	msp_clock_setup();
}

void init()
{
	//  WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
	// Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
	//  PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
	init_hw();
	INIT_CONSOLE();
//	P3DIR |=BIT5;
//	P1DIR |=BIT0;
	__enable_interrupt();
}

int main(void) {
    init();
	PRINTF("INIT\r\n");

	taskId tasks[] = {  {task_inner_loop,0}, {task_outer_loop,0}, {task_finish,0}};
	//This function should be called only once
	os_addTasks(3, tasks );

	os_scheduler();
	return 0;
}
