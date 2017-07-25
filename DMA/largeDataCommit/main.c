#include <stdint.h>
#include <msp430fr5969.h>
#include <ipos.h>

#define ARRSIZE 128
#define FIX_ARRSIZE 128
__p volatile unsigned int data[FIX_ARRSIZE] = {0};

volatile unsigned int readVAr = 0;

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

    P3OUT &=~BIT5;
    P3DIR |=BIT5;
    P4DIR |=BIT0;
}

void randomFunc()
{
    volatile unsigned int i;
    for (i = 0; i < ARRSIZE ; i++)
    {
     data[i] = data[i]+1;
    }
}
unsigned int x = 0;
volatile unsigned int i = 0;
/********************************
            Tasks
********************************/
void task0()
{



        P3OUT |= BIT5;
        x = RVAR(data[i]) ;
        P3OUT &=~BIT5;
        x = x+1;
        WVAR(data[i], x );

        randomFunc();

}

void task1()
{

        WVAR(data[100], 100 );
}



void task2()
{
    unsigned int dummy= RVAR(data[100]);
    dummy++;
        GWVAR(data[200], &= , dummy );
}

void task3()
{

    volatile unsigned int i;
    for (i = 0; i < ARRSIZE ; i++)
    {
        WVAR(data[i], i );

    }
}

void task4()
{

    volatile unsigned int dummy;

    P3OUT |= BIT5;
    dummy = RVAR(data[i]);
    P3OUT &=~BIT5;

    dummy++;
     WVAR(data[i],dummy  );

}

void task5()
{

        WVAR(data[2047], RVAR(data[2000]) );
        P3OUT |= BIT5;
        readVAr =  RVAR(data[2000]);
        P3OUT &=~BIT5;

//        P3OUT |=BIT5;
//        P3OUT &=~BIT5;

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

//    taskId tasks[] = { {task0,0}, {task1,0}};
    os_addTasks(6, tasks );
    os_scheduler();

    return 0;
}
