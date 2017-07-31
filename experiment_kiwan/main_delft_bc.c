#include <msp430.h>
#include <stdlib.h>

#include <libio/log.h>
#include <libipos-gcc-dma/ipos.h>
#include <libmspbuiltins/builtins.h>
#include <libmsp/mem.h>
#include <libmsp/periph.h>
#include <libmsp/clock.h>
#include <libmsp/watchdog.h>
#include <libmsp/gpio.h>
#include <libmspmath/msp-math.h>

#ifdef CONFIG_LIBEDB_PRINTF
#include <libedb/edb.h>
#endif


#include "pins.h"

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

unsigned overflow=0;
__attribute__((interrupt(51))) 
void TimerB1_ISR(void){
	TBCTL &= ~(0x0002);
	if(TBCTL && 0x0001){
		overflow++;
		TBCTL |= 0x0004;
		TBCTL |= (0x0002);
		TBCTL &= ~(0x0001);	
	}
}
__attribute__((section("__interrupt_vector_timer0_b1"),aligned(2)))
void(*__vector_timer0_b1)(void) = TimerB1_ISR;

unsigned volatile *timer = &TBCTL;

static void init_hw()
{
	msp_watchdog_disable();
	msp_gpio_unlock();
	msp_clock_setup();
}

void init() {
#ifdef BOARD_MSP_TS430
	*timer &= 0xE6FF; //set 12,11 bit to zero (16bit) also 8 to zero (SMCLK)
	*timer |= 0x0200; //set 9 to one (SMCLK)
	*timer |= 0x00C0; //set 7-6 bit to 11 (divider = 8);
	*timer &= 0xFFEF; //set bit 4 to zero
	*timer |= 0x0020; //set bit 5 to one (5-4=10: continuous mode)
	*timer |= 0x0002; //interrupt enable
#endif
	init_hw();

	INIT_CONSOLE();

	__enable_interrupt();
}

void task_init() {
	LOG("init\r\n");

	P(_v_func) = 0;
	P(_v_n_0) = 0;
	P(_v_n_1) = 0;
	P(_v_n_2) = 0;
	P(_v_n_3) = 0;
	P(_v_n_4) = 0;
	P(_v_n_5) = 0;
	P(_v_n_6) = 0;

	os_jump(OFFSET(t_init, t_select_func));
}

void task_select_func() {
	LOG("select func\r\n");

	P(_v_seed) = (uint32_t)SEED; // for test, seed is always the same
	P(_v_iter) = 0;
	LOG("func: %u\r\n", P(_v_func));
	if(P(_v_func) == 0){
		P(_v_func)++;
		os_jump(OFFSET(t_select_func, t_bit_count));
	}
	else if(P(_v_func) == 1){
		P(_v_func)++;
		os_jump(OFFSET(t_select_func, t_bitcount));
	}
	else if(P(_v_func) == 2){
		P(_v_func)++;
		os_jump(OFFSET(t_select_func, t_ntbl_bitcnt));
	}
	else if(P(_v_func) == 3){
		P(_v_func)++;
		os_jump(OFFSET(t_select_func, t_ntbl_bitcount));
	}
	else if(P(_v_func) == 4){
		P(_v_func)++;
		os_jump(OFFSET(t_select_func, t_BW_btbl_bitcount));
	}
	else if(P(_v_func) == 5){
		P(_v_func)++;
		os_jump(OFFSET(t_select_func, t_AR_btbl_bitcount));
	}
	else if(P(_v_func) == 6){
		P(_v_func)++;
		os_jump(OFFSET(t_select_func, t_bit_shifter));
	}
	else{
		os_jump(OFFSET(t_select_func, t_end));
	}
}
void task_bit_count() {
	LOG("bit_count\r\n");
	uint32_t tmp_seed = P(_v_seed);
	P(_v_seed) = tmp_seed + 13;
	unsigned temp = 0;
	if(tmp_seed) do 
		temp++;
	while (0 != (tmp_seed = tmp_seed&(tmp_seed-1)));
	P(_v_n_0) += temp;
	P(_v_iter)++;

	if(P(_v_iter) < ITER){
		os_jump(OFFSET(t_bit_count, t_bit_count));
	}
	else{
		os_jump(OFFSET(t_bit_count, t_select_func));
	}
}
void task_bitcount() {
	LOG("bitcount\r\n");
	uint32_t tmp_seed = P(_v_seed);
	P(_v_seed) = tmp_seed + 13;
	tmp_seed = ((tmp_seed & 0xAAAAAAAAL) >>  1) + (tmp_seed & 0x55555555L);
	tmp_seed = ((tmp_seed & 0xCCCCCCCCL) >>  2) + (tmp_seed & 0x33333333L);
	tmp_seed = ((tmp_seed & 0xF0F0F0F0L) >>  4) + (tmp_seed & 0x0F0F0F0FL);
	tmp_seed = ((tmp_seed & 0xFF00FF00L) >>  8) + (tmp_seed & 0x00FF00FFL);
	tmp_seed = ((tmp_seed & 0xFFFF0000L) >> 16) + (tmp_seed & 0x0000FFFFL);
	P(_v_n_1) += (int)tmp_seed;
	P(_v_iter)++;

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
	LOG("ntbl_bitcnt\r\n");
	uint32_t tmp_seed = P(_v_seed);
	P(_v_n_2) += non_recursive_cnt(tmp_seed);	
	P(_v_seed) = tmp_seed + 13;
	P(_v_iter)++;

	if(P(_v_iter) < ITER){
		os_jump(OFFSET(t_ntbl_bitcnt, t_ntbl_bitcnt));
	}
	else{
		os_jump(OFFSET(t_ntbl_bitcnt, t_select_func));
	}
}
void task_ntbl_bitcount() {
	LOG("ntbl_bitcount\r\n");
	P(_v_n_3) += bits[ (int) (P(_v_seed) & 0x0000000FUL)       ] +
		bits[ (int)((P(_v_seed) & 0x000000F0UL) >> 4) ] +
		bits[ (int)((P(_v_seed) & 0x00000F00UL) >> 8) ] +
		bits[ (int)((P(_v_seed) & 0x0000F000UL) >> 12)] +
		bits[ (int)((P(_v_seed) & 0x000F0000UL) >> 16)] +
		bits[ (int)((P(_v_seed) & 0x00F00000UL) >> 20)] +
		bits[ (int)((P(_v_seed) & 0x0F000000UL) >> 24)] +
		bits[ (int)((P(_v_seed) & 0xF0000000UL) >> 28)];
	uint32_t tmp_seed = P(_v_seed);
	P(_v_seed) = tmp_seed + 13;
	P(_v_iter)++;

	if(P(_v_iter) < ITER){
		os_jump(OFFSET(t_ntbl_bitcount, t_ntbl_bitcount));
	}
	else{
		os_jump(OFFSET(t_ntbl_bitcount, t_select_func));
	}
}
void task_BW_btbl_bitcount() {
	LOG("BW_btbl_bitcount\r\n");
	union 
	{ 
		unsigned char ch[4]; 
		long y; 
	} U; 

	U.y = P(_v_seed); 

	P(_v_n_4) += bits[ U.ch[0] ] + bits[ U.ch[1] ] + 
		bits[ U.ch[3] ] + bits[ U.ch[2] ]; 
	uint32_t tmp_seed = P(_v_seed);
	P(_v_seed) = tmp_seed + 13;
	P(_v_iter)++;

	if(P(_v_iter) < ITER){
		os_jump(OFFSET(t_BW_btbl_bitcount, t_BW_btbl_bitcount));
	}
	else{
		os_jump(OFFSET(t_BW_btbl_bitcount, t_select_func));
	}
}
void task_AR_btbl_bitcount() {
	LOG("AR_btbl_bitcount\r\n");
	unsigned char * Ptr = (unsigned char *) &P(_v_seed) ;
	int Accu ;

	Accu  = bits[ P(*Ptr++) ];
	Accu += bits[ P(*Ptr++) ];
	Accu += bits[ P(*Ptr++) ];
	Accu += bits[ P(*Ptr) ];
	P(_v_n_5)+= Accu;
	uint32_t tmp_seed = P(_v_seed);
	P(_v_seed) = tmp_seed + 13;
	P(_v_iter)++;

	if(P(_v_iter) < ITER){
		os_jump(OFFSET(t_AR_btbl_bitcount, t_AR_btbl_bitcount));
	}
	else{
		os_jump(OFFSET(t_AR_btbl_bitcount, t_select_func));
	}
}
void task_bit_shifter() {
	LOG("bit_shifter\r\n");
	unsigned i, nn;
	uint32_t tmp_seed = P(_v_seed);
	for (i = nn = 0; tmp_seed && (i < (sizeof(long) * CHAR_BIT)); ++i, tmp_seed >>= 1)
		nn += (unsigned)(tmp_seed & 1L);
	P(_v_n_6) += nn;
	tmp_seed = P(_v_seed);
	tmp_seed += 13;
	P(_v_seed) = tmp_seed;

	P(_v_iter)++;

	if(P(_v_iter) < ITER){
		os_jump(OFFSET(t_bit_shifter, t_bit_shifter));
	}
	else{
		os_jump(OFFSET(t_bit_shifter, t_select_func));
	}
}

void task_end() {
	PRINTF("TIME end is 65536*%u+%u\r\n",overflow,(unsigned)TBR);
	LOG("end\r\n");
	PRINTF("%u\r\n", P(_v_n_0));
	PRINTF("%u\r\n", P(_v_n_1));
	PRINTF("%u\r\n", P(_v_n_2));
	PRINTF("%u\r\n", P(_v_n_3));
	PRINTF("%u\r\n", P(_v_n_4));
	PRINTF("%u\r\n", P(_v_n_5));
	PRINTF("%u\r\n", P(_v_n_6));
	exit(0);	
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
