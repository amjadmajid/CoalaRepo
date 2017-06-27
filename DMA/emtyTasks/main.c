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
            Tasks
********************************/

// initialize the persistent var
void task0()
{
}

// increase the persistent var
void task1()
{

}

void task2()
{

}

void task3()
{

}

void task4()
{

}

void task5()
{

}

void task6()
{

}

void task7()
{

}

void task8()
{

}

void task9()
{

}

void task10()
{

}

void task11()
{

}

void task12()
{
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
