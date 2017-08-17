#include <math.h>
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <libwispbase/accel.h>
#include <libchain/chain.h>
#include <libmspbuiltins/builtins.h>
#include <libio/log.h>
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

#define SIZE 16
#define PI2 6.28

unsigned overflow=0;
void __attribute__((interrupt(TIMERB1_VECTOR))) TimerB1_ISR(void){
	TBCTL &= ~(0x0002);
	if(TBCTL && 0x0001){
		overflow++;
		TBCTL |= 0x0004;
		TBCTL |= (0x0002);
		TBCTL &= ~(0x0001);	
	}
//	while(1);
	//  P1OUT ^= 0x01;                            // Toggle P1.0
}
static void init_hw()
{
	msp_watchdog_disable();
	msp_gpio_unlock();
	msp_clock_setup();
}

void init()
{
#ifdef BOARD_MSP_TS430
	TBCTL &= 0xE6FF; //set 12,11 bit to zero (16bit) also 8 to zero (SMCLK)
	TBCTL |= 0x0200; //set 9 to one (SMCLK)
	TBCTL |= 0x00C0; //set 7-6 bit to 11 (divider = 8);
	TBCTL &= 0xFFEF; //set bit 4 to zero
	TBCTL |= 0x0020; //set bit 5 to one (5-4=10: continuous mode)
	TBCTL |= 0x0002; //interrupt enable
#endif
    threeAxis_t_8 accelID = {0};

	init_hw();

#ifdef CONFIG_EDB
	edb_init();
#endif

    INIT_CONSOLE();

    __enable_interrupt();
    PRINTF(".%u.\r\n", curctx->task->idx);
	//PRINTF("init\r\n");
}

struct msg_init_real {
    CHAN_FIELD_ARRAY(float, Xre, SIZE);
    CHAN_FIELD(uint16_t, k);
    CHAN_FIELD(uint16_t, n);
};
struct msg_init_power {
    CHAN_FIELD(uint16_t, k);
};
struct msg_init_im {
    CHAN_FIELD_ARRAY(float, Xim, SIZE);
    CHAN_FIELD(uint16_t, k);
    CHAN_FIELD(uint16_t, n);
};
struct msg_init_disc {
    CHAN_FIELD(uint16_t, n);
};
struct msg_disc_real {
    CHAN_FIELD_ARRAY(float, x, SIZE);
};
struct msg_disc_im {
    CHAN_FIELD_ARRAY(float, x, SIZE);
};
struct msg_self_disc {
    SELF_CHAN_FIELD(uint16_t, n);
};
#define FIELD_INIT_msg_self_disc {\
    SELF_FIELD_INITIALIZER \
}
struct msg_self_real {
    SELF_CHAN_FIELD(uint16_t, n);
    SELF_CHAN_FIELD_ARRAY(float, Xre, SIZE);
};
#define FIELD_INIT_msg_self_real {\
    SELF_FIELD_INITIALIZER, \
    SELF_FIELD_ARRAY_INITIALIZER(16) \
}
struct msg_self_im {
    SELF_CHAN_FIELD(uint16_t, n);
    SELF_CHAN_FIELD_ARRAY(float, Xim, SIZE);
};
#define FIELD_INIT_msg_self_im {\
    SELF_FIELD_INITIALIZER, \
    SELF_FIELD_ARRAY_INITIALIZER(16) \
}
struct msg_real_power {
    CHAN_FIELD_ARRAY(float, Xre, SIZE);
};
struct msg_im_power {
    CHAN_FIELD_ARRAY(float, Xim, SIZE);
};
struct msg_power_real {
    CHAN_FIELD(uint16_t, k);
};
struct msg_power_im {
    CHAN_FIELD(uint16_t, k);
};
struct msg_self_power {
    SELF_CHAN_FIELD(uint16_t, k);
};
#define FIELD_INIT_msg_self_power {\
    SELF_FIELD_INITIALIZER \
}
struct msg_power_end {
    CHAN_FIELD_ARRAY(float, p, SIZE);
};

/*******Declare tasks **********/
TASK(1, dft_init)
TASK(2, discTimeSign)
TASK(3, dft_outer_loop)
TASK(4, dft_real)
TASK(5, dft_im)
TASK(6, dft_power)
TASK(7, dft_end)

/*******Declare channels **********/
CHANNEL(dft_init, dft_real, msg_init_real);
CHANNEL(dft_init, dft_im, msg_init_im);
CHANNEL(dft_init, discTimeSign, msg_init_disc);
CHANNEL(dft_init, dft_power, msg_init_power);

CHANNEL(discTimeSign, dft_real,  msg_disc_real);
CHANNEL(discTimeSign, dft_im,  msg_disc_im);
SELF_CHANNEL(discTimeSign,  msg_self_disc);

SELF_CHANNEL(dft_real,  msg_self_real);
CHANNEL(dft_real, dft_power, msg_real_power);

SELF_CHANNEL(dft_im,  msg_self_im);
CHANNEL(dft_im, dft_power, msg_im_power);

CHANNEL(dft_power, dft_real, msg_power_real);
CHANNEL(dft_power, dft_im, msg_power_im);
SELF_CHANNEL(dft_power, msg_self_power);
CHANNEL(dft_power, dft_end, msg_power_end);

void dft_init() {
    unsigned n = 0;
    unsigned k = 0;
	srand(0);
    CHAN_OUT1(uint16_t, k, k, CH(dft_init, dft_real));
    CHAN_OUT1(uint16_t, k, k, CH(dft_init, dft_im));
    CHAN_OUT1(uint16_t, k, k, CH(dft_init, dft_power));
    CHAN_OUT1(uint16_t, n, n, CH(dft_init, discTimeSign));
    CHAN_OUT1(uint16_t, n, n, CH(dft_init, dft_real));
    CHAN_OUT1(uint16_t, n, n, CH(dft_init, dft_im));
	for (unsigned i = 0; i < SIZE; ++i) {
		float zero = 0;
    	CHAN_OUT1(float, Xre[i], zero, CH(dft_init, dft_real));
    	CHAN_OUT1(float, Xim[i], zero, CH(dft_init, dft_im));
	}

    TRANSITION_TO(discTimeSign);
}

void discTimeSign()
{
    // Get the input for the task
    unsigned int in_n = *CHAN_IN2(uint16_t, n, CH(dft_init, discTimeSign), SELF_CH(discTimeSign));
	//PRINTF("discTimeSign: %u\r\n", in_n);
	// Generate random discrete-time signal x in range (-1,+1)
	float in_x_n = ((2.0 * rand()) / RAND_MAX) - 1.0;
	//float in_x_n = 0.5;

	CHAN_OUT1(float, x[in_n], in_x_n, CH(discTimeSign, dft_real));
	CHAN_OUT1(float, x[in_n], in_x_n, CH(discTimeSign, dft_im));

	in_n++;

	if(in_n < SIZE){
		CHAN_OUT1(uint16_t, n, in_n, SELF_CH(discTimeSign));
		TRANSITION_TO(discTimeSign);
	}else{
		in_n = 0;
		CHAN_OUT1(uint16_t, n, in_n, SELF_CH(discTimeSign));
		TRANSITION_TO(dft_outer_loop);
	}
}
void dft_outer_loop() {
	// Get the input for the task

	TRANSITION_TO(dft_real);
}

void dft_real() {
	// Get the input for the task
	unsigned int in_k = *CHAN_IN2(unsigned int, k, CH(dft_init, dft_real), CH(dft_power, dft_real));
	unsigned int in_n = *CHAN_IN2(unsigned int, n, CH(dft_init, dft_real), SELF_CH(dft_real));
	float in_Xre_k = *CHAN_IN2(float, Xre[in_k], CH(dft_init, dft_real), SELF_CH(dft_real));
	float in_x_n = *CHAN_IN1(float, x[in_n], CH(discTimeSign, dft_real));

	in_Xre_k += in_x_n * cosf(in_n * in_k * PI2 / SIZE);
//	PRINTF("dftRead: %d\r\n", in_Xre_k);
	// commit to the buffer
	CHAN_OUT1(float, Xre[in_k], in_Xre_k, SELF_CH(dft_real));

	in_n++;

	if(in_n < SIZE){
		CHAN_OUT1(uint16_t, n, in_n, SELF_CH(dft_real));
		TRANSITION_TO(dft_real);
	}else{
		in_n = 0;
		CHAN_OUT1(uint16_t, n, in_n, SELF_CH(dft_real));
		CHAN_OUT1(float, Xre[in_k], in_Xre_k, CH(dft_real, dft_power));
		TRANSITION_TO(dft_im);
	}
}

void dft_im() {
	// Get the input for the task
	unsigned int in_k = *CHAN_IN2(unsigned int, k, CH(dft_init, dft_im), CH(dft_power, dft_im));
	unsigned int in_n = *CHAN_IN2(unsigned int, n, CH(dft_init, dft_im), SELF_CH(dft_im));
	float in_Xim_k = *CHAN_IN2(float, Xim[in_k], CH(dft_init, dft_im), SELF_CH(dft_im));
	float in_x_n = *CHAN_IN1(float, x[in_n], CH(discTimeSign, dft_im));

	in_Xim_k -= in_x_n * sinf(in_n * in_k * PI2 / SIZE);
//	PRINTF("dftIm: %d\r\n", in_Xim_k);
	CHAN_OUT1(float, Xim[in_k], in_Xim_k, SELF_CH(dft_im));

	in_n++;

	if(in_n < SIZE){
		CHAN_OUT1(uint16_t, n, in_n, SELF_CH(dft_im));
		TRANSITION_TO(dft_im);
	}else{
		in_n = 0;
		CHAN_OUT1(uint16_t, n, in_n, SELF_CH(dft_im));
		CHAN_OUT1(float, Xim[in_k], in_Xim_k, CH(dft_im, dft_power));
		TRANSITION_TO(dft_power);
	}
}

void dft_power() {
	//    task_prologue();
	unsigned int k = *CHAN_IN2(unsigned int, k, CH(dft_init, dft_power), SELF_CH(dft_power));
	float xre_k = *CHAN_IN1(float, Xre[k], CH(dft_real, dft_power));
	float xim_k = *CHAN_IN1(float, Xim[k], CH(dft_im, dft_power));

	float p_k = xre_k * xre_k + xim_k * xim_k;
	CHAN_OUT1(float, p[k], p_k, CH(dft_power, dft_end));
	k++;
	CHAN_OUT3(unsigned, k, k, CH(dft_power, dft_real), CH(dft_power, dft_im), SELF_CH(dft_power));
	if(k < SIZE){
		TRANSITION_TO(dft_outer_loop);
	}else{
		TRANSITION_TO(dft_end);
	}
}

void dft_end() {
	PRINTF("TIME end is 65536*%u+%u\r\n",overflow,(unsigned)TBR);
	for (unsigned i = 0; i < SIZE; ++i) {
		float p_k = *CHAN_IN1(float, p[i], CH(dft_power, dft_end));
		PRINTF("P[%u]=%u\r\n", i, (unsigned)(p_k*1000000));
	}	
}

ENTRY_TASK(dft_init)
INIT_FUNC(init)
