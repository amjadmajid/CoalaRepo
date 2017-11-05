#include <msp430.h>
#include <mspReseter.h>
#include "mspProfiler.h"
#include "mspDebugger.h"
#include <ipos.h>

//#define TSK_SIZ
//#define AUTO_RST
//#define LOG_INFO

#define DATA_LEN 48
__nv uint8_t pinCont = 0;
const unsigned int CRC_Init = 0xFFFF;
const unsigned int CRC_Input[] = {
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691
};


__p unsigned int SW_Results=0;                    // Holds results
__p unsigned int cnt = 0 ;

// Tasks declarations
void initTask();
void firstByte();
void secondByte();
void task_finish();
unsigned int CCITT_Update(unsigned int init, unsigned int input);
void init();


void init()
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;

    P3OUT &=~BIT5;
    P3DIR |=BIT5;


#if 0
    CSCTL0_H = CSKEY >> 8;                // Unlock CS registers
//    CSCTL1 = DCOFSEL_4 |  DCORSEL;                   // Set DCO to 16MHz
    CSCTL1 = DCOFSEL_6;                   // Set DCO to 8MHz
    CSCTL2 =  SELM__DCOCLK;               // MCLK = DCO
    CSCTL3 = DIVM__1;                     // divide the DCO frequency by 1
    CSCTL0_H = 0;
#endif


#ifdef TSK_SIZ
    cp_init();
#endif

#ifdef LOG_INFO
    uart_init();
#endif

#ifdef AUTO_RST
    mr_auto_rand_reseter(13000); // every 12 msec the MCU will be reseted
#endif

}

void initTask()
{
#ifdef TSK_SIZ
       cp_reset();
#endif

    pinCont=1;
    WP(cnt) = 0;
    WP(SW_Results) = CRC_Init;

#ifdef TSK_SIZ
     cp_sendRes("initTask \0");
#endif

}

void firstByte()
{
#ifdef TSK_SIZ
       cp_reset();
#endif

    // First input lower byte
    WP(SW_Results) = CCITT_Update(RP(SW_Results), CRC_Input[RP(cnt)] & 0xFF);

#ifdef TSK_SIZ
     cp_sendRes("firstByte \0");
#endif

}

void secondByte()
{

#ifdef TSK_SIZ
       cp_reset();
#endif

    // Then input upper byte
    WP(SW_Results) = CCITT_Update(RP(SW_Results), (CRC_Input[RP(cnt)] >> 8) & 0xFF);

    WP(cnt)++;
    if(RP(cnt) < DATA_LEN)
    {
        os_jump(3);
    }

#ifdef TSK_SIZ
     cp_sendRes("secondByte \0");
#endif
}

void task_finish()
{
#ifdef TSK_SIZ
       cp_reset();
#endif

    PAGCMT(); // force a commit after this task


    if (pinCont){
        __disable_interrupt();
        P3OUT |=BIT5;
        P3OUT &=~BIT5;
    }
    pinCont=0;
    __enable_interrupt();

#ifdef TSK_SIZ
     cp_sendRes("task_finish \0");
#endif

}

// Software algorithm - CCITT CRC16 code
unsigned int CCITT_Update(unsigned int init, unsigned int input)
{
    unsigned int CCITT = (unsigned char) (init >> 8) | (init << 8);
    CCITT ^= input;
    CCITT ^= (unsigned char) (CCITT & 0xFF) >> 4;
    CCITT ^= (CCITT << 8) << 4;
    CCITT ^= ((CCITT & 0xFF) << 4) << 1;
    return CCITT;
}

int main(void) {
    init();

    taskId tasks[] = {  {initTask,1, 1},
                        {firstByte,2, 1},
                        {secondByte,3, 1 },
                        {task_finish, 4, 1}
    };
    //This function should be called only once
    os_addTasks(4, tasks );

    os_scheduler();
    return 0;
}



