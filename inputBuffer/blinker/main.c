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
  Declare global/persistent var
********************************/
DVAR(uint16_t, var);

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
    P4OUT |= BIT0; // lit the LED
    burn(wait); // wait
    P4OUT &= ~BIT0; // dim the LED
    burn(wait); // wait
}

static void blinkLedJ(uint32_t wait )
{
    PJOUT |= 0x40; // lit the LED
    burn(wait); // wait
    PJOUT &= ~0x40; // dim the LED
    burn(wait); // wait
}

// initialize MSP430
void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
    P4DIR |= 0x01; // make Port 1 pin 0 an output
    PJDIR |= 0x40; // make Port j pin 0 an output
}


/********************************
            Tasks
********************************/

// initialize the persistent var
void task0()
{
    WVAR(var, 1);
}

// increase the persistent var
void task1()
{
    volatile int x  = RVAR(var);
    x++;
    if (x >= 10)
        x = 1;
     WVAR(var, x);

     blinkLedJ( 100000 );
}

// use the persistent var
void task2()
{
    volatile int  x =  RVAR(var);
    while(x--)
        blinkLed( 5000 );
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
