#include <stdint.h>
#include <msp430fr5969.h>
#include <ipos.h>

#define ARRSIZE 128
#define FIX_ARRSIZE 2048
__nv volatile uint16_t data[FIX_ARRSIZE] = {0};


void init();
void task0();
void task1();
void task2();
void task3();
void task4();
void task5();

// initialize MSP430
void init()
{
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.

    P3DIR |=BIT5;
    P4DIR |=BIT0;
}

/********************************
            Tasks
********************************/
void task0()
{

    volatile uint16_t i;
    uint16_t x = 0;
    for (i = 0; i < ARRSIZE ; i++)
    {
        x = RVAR(data[i]) +1;
        WVAR(data[i], x );
    }
}


void task1()
{
    volatile uint16_t i;
    for (i = 0; i < ARRSIZE ; i++)
    {
        WVAR(data[i], RVAR(data[i]) +1 );
    }

}

void task2()
{
    volatile uint16_t i;
    for (i = 0; i < ARRSIZE ; i++)
    {
        WVAR(data[i], RVAR(data[i]) +1 );
    }

}

void task3()
{

    volatile uint16_t i;
    for (i = 0; i < ARRSIZE ; i++)
    {
        WVAR(data[i], RVAR(data[i]) +1 );
    }
}

void task4()
{

    volatile uint16_t i;
    for (i = 0; i < ARRSIZE ; i++)
    {
        WVAR(data[i], RVAR(data[i]) +1 );
    }
}
void task5()
{
    volatile uint16_t i;
    for (i = 0; i < ARRSIZE ; i++)
    {
        WVAR(data[i], 0 );
    }

//        P3OUT |=BIT5;
//        P3OUT &=~BIT5;
        P4OUT |=BIT0;
        __delay_cycles(500);
        P4OUT &= ~BIT0;

}




int main(int argc, char const *argv[])
{
    init();
    taskId tasks[] = { {task0,0},
                       {task1,0},
                       {task2,0},
                       {task3,0},
                       {task4,0},
                       {task5,0}};
    os_addTasks(6, tasks );
    os_scheduler();

    return 0;
}
