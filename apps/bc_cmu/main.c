#include <msp430.h>
#include <stdlib.h>
#include <mspReseter.h>
#include "mspProfiler.h"
#include "mspDebugger.h"
#include <ipos.h>

#define SEED 4L
#define ITER 100
#define CHAR_BIT 8

__nv static char bits[256] =
{
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,  /* 0   - 15  */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 16  - 31  */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 32  - 47  */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 48  - 63  */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 64  - 79  */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 80  - 95  */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 96  - 111 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 112 - 127 */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 128 - 143 */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 144 - 159 */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 160 - 175 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 176 - 191 */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 192 - 207 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 208 - 223 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 224 - 239 */
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8   /* 240 - 255 */
};

#define TASK_NUM 10
#define OFFSET(src, dest) src <= dest ? dest - src : TASK_NUM + dest - src

enum task_index {
    t_init,
    t_select_func,
    t_bit_count,
    t_bitcount,
    t_ntbl_bitcnt,
    t_ntbl_bitcount,
    t_BW_btbl_bitcount,
    t_AR_btbl_bitcount,
    t_bit_shifter,
    t_end
};

void task_init();
void task_select_func();
void task_bit_count();
void task_bitcount();
void task_ntbl_bitcnt();
void task_ntbl_bitcount();
void task_BW_btbl_bitcount();
void task_AR_btbl_bitcount();
void task_bit_shifter();
void task_end();

__nv uint8_t pinCont = 0;


__p unsigned _v_n_0;
__p unsigned _v_n_1;
__p unsigned _v_n_2;
__p unsigned _v_n_3;
__p unsigned _v_n_4;
__p unsigned _v_n_5;
__p unsigned _v_n_6;
__p unsigned _v_func;
__p uint32_t _v_seed;
__p unsigned _v_iter;


void init() {

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.

    P3DIR = BIT5;

//    CSCTL0_H = CSKEY >> 8;                // Unlock CS registers
////  CSCTL1 = DCOFSEL_4 |  DCORSEL;                   // Set DCO to 16MHz
//    CSCTL1 = DCOFSEL_6;                   // Set DCO to 8MHz
//    CSCTL2 =  SELM__DCOCLK;               // MCLK = DCO
//    CSCTL3 = DIVM__1;                     // divide the DCO frequency by 1
//    CSCTL0_H = 0;
//
//  __enable_interrupt();

//    cp_init();
    uart_init();
    mr_auto_rand_reseter(13000); // every 12 msec the MCU will be reseted
}

void task_init() {

    pinCont=1;

    PW(_v_func) = 0;
    PW(_v_n_0) = 0;
    PW(_v_n_1) = 0;
    PW(_v_n_2) = 0;
    PW(_v_n_3) = 0;
    PW(_v_n_4) = 0;
    PW(_v_n_5) = 0;
    PW(_v_n_6) = 0;

    os_jump(OFFSET(t_init, t_select_func));
}

void task_select_func() {

    PW(_v_seed) = (uint32_t)SEED; // for test, seed is always the same
    PW(_v_iter) = 0;
    if(P(_v_func) == 0){
        PW(_v_func)++;
        os_jump(OFFSET(t_select_func, t_bit_count));
    }
    else if(P(_v_func) == 1){
        PW(_v_func)++;
        os_jump(OFFSET(t_select_func, t_bitcount));
    }
    else if(P(_v_func) == 2){
        PW(_v_func)++;
        os_jump(OFFSET(t_select_func, t_ntbl_bitcnt));
    }
    else if(P(_v_func) == 3){
        PW(_v_func)++;
        os_jump(OFFSET(t_select_func, t_ntbl_bitcount));
    }
    else if(P(_v_func) == 4){
        PW(_v_func)++;
        os_jump(OFFSET(t_select_func, t_BW_btbl_bitcount));
    }
    else if(P(_v_func) == 5){
        PW(_v_func)++;
        os_jump(OFFSET(t_select_func, t_AR_btbl_bitcount));
    }
    else if(P(_v_func) == 6){
        PW(_v_func)++;
        os_jump(OFFSET(t_select_func, t_bit_shifter));
    }
    else{
        os_jump(OFFSET(t_select_func, t_end));
    }
}
void task_bit_count() {
    uint32_t tmp_seed = P(_v_seed);
    PW(_v_seed) = tmp_seed + 13;
    unsigned temp = 0;
    if(tmp_seed) do
        temp++;
    while (0 != (tmp_seed = tmp_seed&(tmp_seed-1)));
    PW(_v_n_0) += temp;
    PW(_v_iter)++;

    if(P(_v_iter) < ITER){
        os_jump(OFFSET(t_bit_count, t_bit_count));
    }
    else{
        os_jump(OFFSET(t_bit_count, t_select_func));
    }
}
void task_bitcount() {
    uint32_t tmp_seed = P(_v_seed);
    PW(_v_seed) = tmp_seed + 13;
    tmp_seed = ((tmp_seed & 0xAAAAAAAAL) >>  1) + (tmp_seed & 0x55555555L);
    tmp_seed = ((tmp_seed & 0xCCCCCCCCL) >>  2) + (tmp_seed & 0x33333333L);
    tmp_seed = ((tmp_seed & 0xF0F0F0F0L) >>  4) + (tmp_seed & 0x0F0F0F0FL);
    tmp_seed = ((tmp_seed & 0xFF00FF00L) >>  8) + (tmp_seed & 0x00FF00FFL);
    tmp_seed = ((tmp_seed & 0xFFFF0000L) >> 16) + (tmp_seed & 0x0000FFFFL);
    PW(_v_n_1) += (int)tmp_seed;
    PW(_v_iter)++;

    if(P(_v_iter) < ITER){
        os_jump(OFFSET(t_bitcount, t_bitcount));
    }
    else{
        os_jump(OFFSET(t_bitcount, t_select_func));
    }
}
int recursive_cnt(uint32_t x){
    int cnt = bits[(int)(x & 0x0000000FL)];

    if (0L != (x >>= 4))
        cnt += recursive_cnt(x);

    return cnt;
}
int non_recursive_cnt(uint32_t x){
    int cnt = bits[(int)(x & 0x0000000FL)];

    while (0L != (x >>= 4)) {
        cnt += bits[(int)(x & 0x0000000FL)];
    }

    return cnt;
}
void task_ntbl_bitcnt() {
    uint32_t tmp_seed = P(_v_seed);
    PW(_v_n_2) += non_recursive_cnt(tmp_seed);
    PW(_v_seed) = tmp_seed + 13;
    PW(_v_iter)++;

    if(P(_v_iter) < ITER){
        os_jump(OFFSET(t_ntbl_bitcnt, t_ntbl_bitcnt));
    }
    else{
        os_jump(OFFSET(t_ntbl_bitcnt, t_select_func));
    }
}
void task_ntbl_bitcount() {
    uint16_t __cry = P(_v_seed);
    PW(_v_n_3) += bits[ (int) (__cry & 0x0000000FUL)       ] +
        bits[ (int)((__cry & 0x000000F0UL) >> 4) ] +
        bits[ (int)((__cry & 0x00000F00UL) >> 8) ] +
        bits[ (int)((__cry & 0x0000F000UL) >> 12)] +
        bits[ (int)((__cry & 0x000F0000UL) >> 16)] +
        bits[ (int)((__cry & 0x00F00000UL) >> 20)] +
        bits[ (int)((__cry & 0x0F000000UL) >> 24)] +
        bits[ (int)((__cry & 0xF0000000UL) >> 28)];
    uint32_t tmp_seed = P(_v_seed);
    PW(_v_seed) = tmp_seed + 13;
    PW(_v_iter)++;

    if(P(_v_iter) < ITER){
        os_jump(OFFSET(t_ntbl_bitcount, t_ntbl_bitcount));
    }
    else{
        os_jump(OFFSET(t_ntbl_bitcount, t_select_func));
    }
}
void task_BW_btbl_bitcount() {
    union
    {
        unsigned char ch[4];
        long y;
    } U;

    U.y = P(_v_seed);

    PW(_v_n_4) += bits[ U.ch[0] ] + bits[ U.ch[1] ] +
        bits[ U.ch[3] ] + bits[ U.ch[2] ];
    uint32_t tmp_seed = P(_v_seed);
    PW(_v_seed) = tmp_seed + 13;
    PW(_v_iter)++;

    if(P(_v_iter) < ITER){
        os_jump(OFFSET(t_BW_btbl_bitcount, t_BW_btbl_bitcount));
    }
    else{
        os_jump(OFFSET(t_BW_btbl_bitcount, t_select_func));
    }
}
void task_AR_btbl_bitcount() {
    unsigned char * Ptr = (unsigned char *) &P(_v_seed) ;
    int Accu ;

    Accu  = bits[ P(*Ptr++) ];
    Accu += bits[ P(*Ptr++) ];
    Accu += bits[ P(*Ptr++) ];
    Accu += bits[ P(*Ptr) ];
    PW(_v_n_5)+= Accu;
    uint32_t tmp_seed = P(_v_seed);
    PW(_v_seed) = tmp_seed + 13;
    PW(_v_iter)++;

    if(P(_v_iter) < ITER){
        os_jump(OFFSET(t_AR_btbl_bitcount, t_AR_btbl_bitcount));
    }
    else{
        os_jump(OFFSET(t_AR_btbl_bitcount, t_select_func));
    }
}
void task_bit_shifter() {
    unsigned i, nn;
    uint32_t tmp_seed = P(_v_seed);
    for (i = nn = 0; tmp_seed && (i < (sizeof(long) * CHAR_BIT)); ++i, tmp_seed >>= 1)
        nn += (unsigned)(tmp_seed & 1L);
    PW(_v_n_6) += nn;
    tmp_seed = P(_v_seed);
    tmp_seed += 13;
    PW(_v_seed) = tmp_seed;

    PW(_v_iter)++;

    if(P(_v_iter) < ITER){
        os_jump(OFFSET(t_bit_shifter, t_bit_shifter));
    }
    else{
        os_jump(OFFSET(t_bit_shifter, t_select_func));
    }

//    PAGCMT(); // force page commit on the next task switch
}

void task_end() {
    __no_operation();

    if (pinCont){
        P3OUT |=BIT5;
        P3OUT &=~BIT5;
    }
    pinCont=0;

}

int main(void) {
    init();
    taskId tasks[] = {{task_init, 0},
        {task_select_func, 0},
        {task_bit_count, 0},
        {task_bitcount, 0},
        {task_ntbl_bitcnt, 0},
        {task_ntbl_bitcount, 0},
        {task_BW_btbl_bitcount, 0},
        {task_AR_btbl_bitcount, 0},
        {task_bit_shifter, 0},
        {task_end, 0}};

    //This function should be called only once
    os_addTasks(TASK_NUM, tasks );

    os_scheduler();
    return 0;
}
