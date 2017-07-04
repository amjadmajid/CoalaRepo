#include <stdint.h>
#include <msp430fr5969.h>
#include <ipos.h>

//#define ARRSIZE 50
#define ARRSIZE 8
#define FIX_ARRSIZE 128
__v volatile uint16_t data[FIX_ARRSIZE] = {0};

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
    for (i = 0; i < ARRSIZE ; i++)
    {
        data[i] += 1;
    }
}


void task1()
{
    volatile uint16_t i;
    for (i = 0; i < ARRSIZE ; i++)
    {
        data[i] += 1;
    }

}

void task2()
{
    volatile uint16_t i;
    for (i = 0; i < ARRSIZE ; i++)
    {
        data[i] += 1;
    }

}

void task3()
{

    volatile uint16_t i;
    for (i = 0; i < ARRSIZE ; i++)
    {
        data[i] += 1;
    }
}

void task4()
{

    volatile uint16_t i;
    for (i = 0; i < ARRSIZE ; i++)
    {
        data[i] += 1;
    }
}
void task5()
{

//    if(data[1] == data[50])
//    {
//        P4OUT |= BIT0;
//        __delay_cycles(500);
//        P4OUT &= ~BIT0;
//    }

    volatile uint16_t i;
    for (i = 0; i < ARRSIZE ; i++)
    {
        data[i] = 0;
    }

        P3OUT |=BIT5;
        P3OUT &=~BIT5;

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
