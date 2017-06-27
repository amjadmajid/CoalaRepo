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
    P4OUT |= 0x01; // lit the LED
    burn(wait); // wait
    P4OUT &= ~0x01; // dim the LED
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
    P4OUT &= ~0x01;     // reset the pint state
    PJOUT &= ~0x40;     //  reset the pint state
    P4DIR |= 0x01;      // make Port 1 pin 0 an output
    PJDIR |= 0x40;      // make Port j pin 0 an output
}


/********************************
  Declare global/persistent var
********************************/
DVAR(uint16_t, var1);
DVAR(uint16_t, var2);
DVAR(uint16_t, var3);
DVAR(uint16_t, var4);
DVAR(uint16_t, var5);
DVAR(uint16_t, var6);
DVAR(uint16_t, var7);
DVAR(uint16_t, var8);
DVAR(uint16_t, var9);
DVAR(uint16_t, var10);

/********************************
            Tasks
********************************/

// initialize the persistent var
void task0()
{
    WVAR(var1, 0);
    WVAR(var2, 0);
    WVAR(var3, 0);
    WVAR(var4, 0);
    WVAR(var5, 0);
    WVAR(var6, 0);
    WVAR(var7, 0);
    WVAR(var8, 0);
    WVAR(var9, 0);
    WVAR(var10, 0);
}

// increase the persistent var
void task1()
{
       volatile uint16_t x1 =RVAR(var1);
       volatile uint16_t x2 =RVAR(var2);
       volatile uint16_t x3 =RVAR(var3);
       volatile uint16_t x4 =RVAR(var4);
       volatile uint16_t x5 =RVAR(var5);
       volatile uint16_t x6 =RVAR(var6);
       volatile uint16_t x7 =RVAR(var7);
       volatile uint16_t x8 =RVAR(var8);
       volatile uint16_t x9 =RVAR(var9);
       volatile uint16_t x10 =RVAR(var1);

       x1 +=1;
       x2 +=1;
       x3 +=1;
       x4 +=1;
       x5 +=1;
       x6 +=1;
       x7 +=1;
       x8 +=1;
       x9 +=1;
       x10+=1;

        WVAR(var1, x1);
        WVAR(var2, x2);
        WVAR(var3, x3);
        WVAR(var4, x4);
        WVAR(var5, x5);
        WVAR(var6, x6);
        WVAR(var7, x7);
        WVAR(var8, x8);
        WVAR(var9, x9);
        WVAR(var10,x10);
}

// use the persistent var
void task2()
{
        blinkLed( 50000 );
       volatile uint16_t x1 =RVAR(var1);
       volatile uint16_t x2 =RVAR(var2);
       volatile uint16_t x3 =RVAR(var3);
       volatile uint16_t x4 =RVAR(var4);
       volatile uint16_t x5 =RVAR(var5);
       volatile uint16_t x6 =RVAR(var6);
       volatile uint16_t x7 =RVAR(var7);
       volatile uint16_t x8 =RVAR(var8);
       volatile uint16_t x9 =RVAR(var9);
       volatile uint16_t x10 =RVAR(var1);

       x1 +=1;
       x2 +=1;
       x3 +=1;
       x4 +=1;
       x5 +=1;
       x6 +=1;
       x7 +=1;
       x8 +=1;
       x9 +=1;
       x10+=1;

        WVAR(var1, x1);
        WVAR(var2, x2);
        WVAR(var3, x3);
        WVAR(var4, x4);
        WVAR(var5, x5);
        WVAR(var6, x6);
        WVAR(var7, x7);
        WVAR(var8, x8);
        WVAR(var9, x9);
        WVAR(var10,x10);
        if ( (x1 == x10) && (x2 == x8) && (x4 == x5) )
        {
            uint16_t y = x1;
            while(y--)
                blinkLed( 5000 );
        }
        if(x1 > 0xfff0)
        {
            WVAR(var1, 0);
            WVAR(var2, 0);
            WVAR(var3, 0);
            WVAR(var4, 0);
            WVAR(var5, 0);
            WVAR(var6, 0);
            WVAR(var7, 0);
            WVAR(var8, 0);
            WVAR(var9, 0);
            WVAR(var10, 0);
        }
}



int main(int argc, char const *argv[])
{
    init();
    funcPt init_tasks[] = {task0};
    os_initTasks(1, init_tasks);
    taskId tasks[] = { {task1,0},
                       {task2,0} };
    os_addTasks(2, tasks );
    os_scheduler();
    while(1)
    {
               blinkLed( 5000 );
    }

    return 0;
}
