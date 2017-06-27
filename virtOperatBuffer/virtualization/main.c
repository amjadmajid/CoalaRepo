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
//    WVAR(var1, 0);
//    WVAR(var2, 0);
//    WVAR(var3, 0);
//    WVAR(var4, 0);
//    WVAR(var5, 0);
//    WVAR(var6, 0);
//    WVAR(var7, 0);
//    WVAR(var8, 0);
//    WVAR(var9, 0);
//    WVAR(var10, 0);
}

// increase the persistent var
void task1()
{
//       volatile uint16_t x0 =RVAR(var1);
//       x0 +=1;
//        WVAR(var1, x0);

}

void task2()
{
//       volatile uint16_t x1 =RVAR(var2);
//       x1 +=1;
//        WVAR(var2, x1);

}

void task3()
{
//       volatile uint16_t x1 =RVAR(var3);
//       x1 +=1;
//        WVAR(var3, x1);

}

void task4()
{
//       volatile uint16_t x1 =RVAR(var4);
//       x1 +=1;
//        WVAR(var4, x1);

}

void task5()
{
//       volatile uint16_t x1 =RVAR(var5);
//       x1 +=1;
//        WVAR(var5, x1);

}

void task6()
{
//       volatile uint16_t x1 =RVAR(var6);
//       x1 +=1;
//        WVAR(var6, x1);

}

void task7()
{
//       volatile uint16_t x1 =RVAR(var7);
//       x1 +=1;
//        WVAR(var7, x1);

}

void task8()
{
//       volatile uint16_t x1 =RVAR(var8);
//       x1 +=1;
//        WVAR(var8, x1);

}

void task9()
{
//       volatile uint16_t x1 =RVAR(var9);
//       x1 +=1;
//        WVAR(var9, x1);

}

void task10()
{
//       volatile uint16_t x1 =RVAR(var10);
//       x1 +=1;
//        WVAR(var10, x1);

}

void task11()
{
//       volatile uint16_t x11 =RVAR(var11);
//       x11 +=1;
//        WVAR(var1, x11);

}

void task12()
{
//       volatile uint16_t x1 =RVAR(var12);
//       x1 +=1;
//        WVAR(var12, x1);

        P3OUT |=BIT5;
        P3OUT &=~BIT5;

}




int main(int argc, char const *argv[])
{
    init();
    funcPt init_tasks[] = {task0};
    os_initTasks(1, init_tasks);
    taskId tasks[] = { {task1,0},
                       {task2,0},
                       {task3,0},
                       {task4,0},
                       {task5,0},
                       {task6,0},
                       {task7,0},
                       {task8,0},
                       {task9,0},
                       {task10,0},
                       {task11,0},
                       {task12,0} };
    os_addTasks(12, tasks );
    os_scheduler();

    return 0;
}
