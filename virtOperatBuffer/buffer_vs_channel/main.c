#include <stdint.h>
#include <msp430fr5969.h>
#include <ipos.h>

// initialize MSP430
void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.

    P3DIR |=BIT5;
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
DVAR(uint16_t, var11);
DVAR(uint16_t, var12);

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
    WVAR(var11, 0);
    WVAR(var12, 0);


}


 void task1()
 {

     P3OUT |=BIT5;
     P3OUT &=~BIT5;

    volatile uint16_t x1 =RVAR(var1);
//    x1 +=1;
    WVAR(var1, x1);


    volatile uint16_t x2 =RVAR(var2);
//    x2 +=1;
    WVAR(var2, x2);

    volatile uint16_t x3 =RVAR(var3);
//    x3 +=1;
    WVAR(var3, x3);


    volatile uint16_t x4 =RVAR(var4);
//    x4 +=1;
    WVAR(var4, x4);


    volatile uint16_t x5 =RVAR(var5);
//    x5 +=1;
    WVAR(var5, x5);

    volatile uint16_t x6 =RVAR(var6);
//    x6 +=1;
    WVAR(var6, x6);

    volatile uint16_t x7 =RVAR(var7);
//    x7 +=1;
    WVAR(var7, x7);


    volatile uint16_t x8 =RVAR(var8);
//    x8 +=1;
    WVAR(var8, x8);

    volatile uint16_t x9 =RVAR(var9);
//    x9 +=1;
    WVAR(var9, x9);


    volatile uint16_t x10 =RVAR(var10);
//    x10 +=1;
    WVAR(var10, x10);

    volatile uint16_t x11 =RVAR(var11);
//    x11 +=1;
    WVAR(var11, x1);

    volatile uint16_t x12 =RVAR(var12);
//    x12 +=1;
    WVAR(var12, x12);

    P3OUT |=BIT5;
    P3OUT &=~BIT5;
    __delay_cycles(10000);
 }

int main(int argc, char const *argv[])
{
    init();
    funcPt init_tasks[] = {task0};
    os_initTasks(1, init_tasks);
    taskId tasks[] = {  {task1,0} };
    os_addTasks(1, tasks );
    os_scheduler();

    return 0;
}
