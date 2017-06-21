#include <stdint.h>
#include <msp430fr5969.h>
#include <ipos.h>

/********************************
    functions prototyping
********************************/
void init();
void task0();
void task1();
void task2();
static void blinkLed(uint32_t wait );
static void burn( uint32_t iters);

/********************************
  Declare global variables
********************************/
__v uint16_t var1;
__v uint16_t var2;
__v uint16_t var3;
__v uint16_t var4;
__v uint16_t var5;

/********************************
        Helper functions
********************************/
//delay
volatile uint32_t work_x;
static void burn( uint32_t iters)
{
    uint32_t iter = iters;
    while(iter--)
        work_x++;
}

//blink an led once
static void blinkLed(uint32_t wait )
{
    P1OUT |= BIT0; // lit the LED
    burn(wait); // wait
    P1OUT &= ~BIT0; // dim the LED
    burn(wait); // wait
}

static void blinkLedJ(uint32_t wait )
{
    P4OUT |= BIT6; // lit the LED
    burn(wait); // wait
    P4OUT &= ~BIT6; // dim the LED
    burn(wait); // wait
}


// initialize MSP430
void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
    P1DIR |= BIT0; // make Port 1 pin 0 an output
    P4DIR |= BIT6; // make Port j pin 0 an output
}


/********************************
            Tasks
********************************/

void task0()
{
   var1 = 0;
   var2 = 2;
   var3 = 3;
   var4 = 4;
   var5 = 5;
}

void task1()
{
    var1++;
    if (var1 >= 10)
        var1 = 1;

    blinkLedJ( 20000 );

}

void task2()
{
    volatile int  x = var1;
    while(x--){
       if( (var2==2) && (var3==3) && (var4==4) && (var5 == 5) )
        blinkLed( 20000 );
    }
}



int main(int argc, char const *argv[])
{
    init();
    funcPt init_tasks[] = {task0}; //
    os_initTasks(1, init_tasks);     // initTasks must be removed and replaced with blocking the tasks
    taskId tasks[] = {{task1, 0},
                      {task2, 0}};
    os_addTasks(2, tasks );
    os_scheduler();
    return 0;
}
