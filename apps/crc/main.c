#include <msp430.h>
#include <ipos.h>
#include "uart-src.h"
#define DATA_LEN 48
#define DEBUG 1

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

    //Configure the UART
#ifdef DEBUG
    // uart init
    uart_init();
#endif

}

void initTask()
{
    WP(cnt) = 0;
    WP(SW_Results) = CRC_Init;

}

void firstByte()
{
    // First input lower byte
    WP(SW_Results) = CCITT_Update(RP(SW_Results), CRC_Input[RP(cnt)] & 0xFF);
}

void secondByte()
{
    // Then input upper byte
    WP(SW_Results) = CCITT_Update(RP(SW_Results), (CRC_Input[RP(cnt)] >> 8) & 0xFF);

    WP(cnt)++;
    if(RP(cnt) < DATA_LEN)
    {
        os_jump(3);
    }
}

void task_finish()
{
    FLASH(); // force a commit after this task

#ifdef DEBUG

    uint16_t n = RP(SW_Results);
    uart_sendHex16(n);
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

    taskId tasks[] = {  {initTask,1},
                        {firstByte,2},
                        {secondByte,3 },
                        {task_finish, 4}
    };
    //This function should be called only once
    os_addTasks(4, tasks );

    os_scheduler();
    return 0;
}



