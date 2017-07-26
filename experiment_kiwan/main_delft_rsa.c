#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

//#include <libwispbase/wisp-base.h>
//#include <wisp-base.h>
#include <libipos-gcc-dma/ipos.h>
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

// #define VERBOSE

//#include "../data/keysize.h"

#define KEY_SIZE_BITS    64
#define DIGIT_BITS       8 // arithmetic ops take 8-bit args produce 16-bit result
#define DIGIT_MASK       0x00ff
#define NUM_DIGITS       (KEY_SIZE_BITS / DIGIT_BITS)

#define PRINT_HEX_ASCII_COLS 8

/** @brief Type large enough to store a product of two digits */
typedef uint16_t digit_t;
//typedef uint8_t digit_t;

typedef struct {
	uint8_t n[NUM_DIGITS]; // modulus
	digit_t e;  // exponent
} pubkey_t;

#if NUM_DIGITS < 2
#error The modular reduction implementation requires at least 2 digits
#endif

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
// #define SHOW_PROGRESS_ON_LED
// #define SHOW_COARSE_PROGRESS_ON_LED

// Blocks are padded with these digits (on the MSD side). Padding value must be
// chosen such that block value is less than the modulus. This is accomplished
// by any value below 0x80, because the modulus is restricted to be above
// 0x80 (see comments below).
static const uint8_t PAD_DIGITS[] = { 0x01 };
#define NUM_PAD_DIGITS (sizeof(PAD_DIGITS) / sizeof(PAD_DIGITS[0]))

// To generate a key pair: see scripts/

// modulus: byte order: LSB to MSB, constraint MSB>=0x80
static __ro_nv const pubkey_t pubkey = {
#include "../data/key64.txt"
};

static __ro_nv const unsigned char PLAINTEXT[] =
#include "../data/plaintext.txt"
;

#define NUM_PLAINTEXT_BLOCKS (sizeof(PLAINTEXT) / (NUM_DIGITS - NUM_PAD_DIGITS) + 1)
#define CYPHERTEXT_SIZE (NUM_PLAINTEXT_BLOCKS * NUM_DIGITS)

#define TASK_NUM 20
#define OFFSET(src, dest) src <= dest ? dest - src : TASK_NUM + dest - src 

enum task_index {
	t_init,
	t_pad,
	t_exp,
	t_mult_block,
	t_mult_block_get_result,
	t_square_base,
	t_square_base_get_result,
	t_print_cyphertext,
	t_mult_mod,
	t_mult,
	t_reduce_digits,
	t_reduce_normalizable,
	t_reduce_normalize,
	t_reduce_n_divisor,
	t_reduce_quotient,
	t_reduce_multiply,
	t_reduce_compare,
	t_reduce_add,
	t_reduce_subtract,
	t_print_product,
};

void task_init();
void task_pad();
void task_exp();
void task_mult_block();
void task_mult_block_get_result();
void task_square_base();
void task_square_base_get_result();
void task_print_cyphertext();
void task_mult_mod();
void task_mult();
void task_reduce_digits();
void task_reduce_normalizable();
void task_reduce_normalize();
void task_reduce_n_divisor();
void task_reduce_quotient();
void task_reduce_multiply();
void task_reduce_compare();
void task_reduce_add();
void task_reduce_subtract();
void task_print_product();

__p digit_t _v_product[32];
__p digit_t _v_exponent;
__p digit_t _v_exponent_next;
__p unsigned _v_block_offset;
__p unsigned _v_message_length;
__p unsigned _v_cyphertext_len;
__p digit_t _v_base[32];
__p digit_t _v_modulus[NUM_DIGITS];
__p digit_t _v_digit;
__p digit_t _v_carry;
__p unsigned _v_reduce;
__p digit_t _v_cyphertext[CYPHERTEXT_SIZE];
__p unsigned _v_offset;
__p digit_t _v_n_div;
//__p task_t* _v_next_task;
__p enum task_index _v_next_task;
__p digit_t _v_product2[32];
__p enum task_index _v_next_task_print;
//__p task_t* _v_next_task_print;
__p digit_t _v_block[32];
__p unsigned _v_quotient;
__p bool _v_print_which;

static void init_hw()
{
	msp_watchdog_disable();
	msp_gpio_unlock();
	msp_clock_setup();
}
unsigned volatile *timer = &TBCTL;
void init()
{
#ifdef BOARD_MSP_TS430
	*timer &= 0xE6FF; //set 12,11 bit to zero (16bit) also 8 to zero (SMCLK)
	*timer |= 0x0200; //set 9 to one (SMCLK)
	*timer |= 0x00C0; //set 7-6 bit to 11 (divider = 8);
	*timer &= 0xFFEF; //set bit 4 to zero
	*timer |= 0x0020; //set bit 5 to one (5-4=10: continuous mode)
	*timer |= 0x0002; //interrupt enable
#endif
	init_hw();

#ifdef CONFIG_EDB
	edb_init();
#endif
	INIT_CONSOLE();

	__enable_interrupt();

//	PRINTF(".%u.\r\n", curctx->task->idx);
}

void task_init()
{
	int i;
	unsigned message_length = sizeof(PLAINTEXT) - 1; // skip the terminating null byte

	LOG("init\r\n");
	LOG("digit: %u\r\n", sizeof(digit_t));
	LOG("unsigned: %u\r\n",sizeof(unsigned));

	LOG("init: out modulus\r\n");

	// TODO: consider passing pubkey as a structure type
	for (i = 0; i < NUM_DIGITS; ++i) {
		P(_v_modulus[i]) = pubkey.n[i];
		LOG("modulus 0: %x\r\n", P(_v_modulus[0]));
	}

	LOG("init: out exp\r\n");

	P(_v_message_length) = message_length;
	P(_v_block_offset) = 0;
	P(_v_cyphertext_len) = 0;

	LOG("init: done\r\n");

	os_jump(OFFSET(t_pad, t_exp));
}

void task_pad()
{
	int i;

	LOG("pad: len=%u offset=%u\r\n", P(_v_message_length), P(_v_block_offset));

	if (P(_v_block_offset) >= P(_v_message_length)) {
		LOG("pad: message done\r\n");
		os_jump(OFFSET(t_pad, t_print_cyphertext));
		return;
	}

	LOG("process block: padded block at offset=%u: ", P(_v_block_offset));
	for (i = 0; i < NUM_PAD_DIGITS; ++i)
		LOG("%x ", PAD_DIGITS[i]);
	LOG("'");
	for (i = NUM_DIGITS - NUM_PAD_DIGITS - 1; i >= 0; --i)
		LOG("%x ", PLAINTEXT[P(_v_block_offset) + i]);
	LOG("\r\n");

	digit_t zero = 0;
	for (i = 0; i < NUM_DIGITS - NUM_PAD_DIGITS; ++i) {
		P(_v_base[i]) = (P(_v_block_offset) + i < P(_v_message_length)) ? PLAINTEXT[P(_v_block_offset) + i] : 0xFF;
	}
	for (i = NUM_DIGITS - NUM_PAD_DIGITS; i < NUM_DIGITS; ++i) {
		P(_v_base[i]) = 1;
	}
	P(_v_block[0]) = 1;
	for (i = 1; i < NUM_DIGITS; ++i)
		P(_v_block[i]) = 0;

	//P(_v_exponent_next) = P(_v_exponent);
	P(_v_exponent) = pubkey.e;

	P(_v_block_offset) += NUM_DIGITS - NUM_PAD_DIGITS;

	os_jump(OFFSET(t_pad, t_exp));
	return;
}

void task_exp()
{
	LOG("exp: e=%x\r\n", P(_v_exponent));

	// ASSERT: e > 0


	if (P(_v_exponent) & 0x1) {
		P(_v_exponent) >>= 1;
		os_jump(OFFSET(t_exp, t_mult_block));
		return;
	} else {
		P(_v_exponent) >>= 1;
		os_jump(OFFSET(t_exp, t_square_base));
		return;
	}
}

// TODO: is this task strictly necessary? it only makes a call. Can this call
// be rolled into task_exp?
void task_mult_block()
{
	LOG("mult block\r\n");

	// TODO: pass args to mult: message * base
	P(_v_next_task) = t_mult_block_get_result;
	os_jump(OFFSET(t_mult_block, t_mult_mod));
	return;
}

void task_mult_block_get_result()
{
	int i;

	LOG("mult block get result: block: ");
	for (i = NUM_DIGITS - 1; i >= 0; --i) { // reverse for printing
		P(_v_block[i]) = P(_v_product[i]);
		LOG("%x ", P(_v_product[i]));
	}
	LOG("\r\n");

	// On last iteration we don't need to square base
	if (P(_v_exponent) > 0) {

		// TODO: current implementation restricts us to send only to the next instantiation
		// of self, so for now, as a workaround, we proxy the value in every instantiation

		os_jump(OFFSET(t_mult_block_get_result, t_square_base));
		return;

	} else { // block is finished, save it
		LOG("mult block get result: cyphertext len=%u\r\n", P(_v_cyphertext_len));

		if (P(_v_cyphertext_len) + NUM_DIGITS <= CYPHERTEXT_SIZE) {

			for (i = 0; i < NUM_DIGITS; ++i) { // reverse for printing
				// TODO: we could save this read by rolling this loop into the
				// above loop, by paying with an extra conditional in the
				// above-loop.
				P(_v_cyphertext[P(_v_cyphertext_len)]) = P(_v_product[i]);
				++P(_v_cyphertext_len);
			}

		} else {
			printf("WARN: block dropped: cyphertext overlow [%u > %u]\r\n",
					P(_v_cyphertext_len) + NUM_DIGITS, CYPHERTEXT_SIZE);
			// carry on encoding, though
		}

		// TODO: implementation limitation: cannot multicast and send to self
		// in the same macro

		LOG("mult block get results: block done, cyphertext_len=%u\r\n", P(_v_cyphertext_len));
		os_jump(OFFSET(t_mult_block_get_result, t_pad));
		return;
	}

}

// TODO: is this task necessary? it seems to act as nothing but a proxy
// TODO: is there opportunity for special zero-copy optimization here
void task_square_base()
{
	LOG("square base\r\n");

	P(_v_next_task) = t_square_base_get_result;
	os_jump(OFFSET(t_square_base, t_mult_mod));
	return;
}

// TODO: is there opportunity for special zero-copy optimization here
void task_square_base_get_result()
{
	int i;
	digit_t b;

	LOG("square base get result\r\n");

	for (i = 0; i < NUM_DIGITS; ++i) {
		LOG("suqare base get result: base[%u]=%x\r\n", i, P(_v_product[i]));
		P(_v_base[i]) = P(_v_product[i]);
	}

	os_jump(OFFSET(t_square_base_get_result, t_exp));
	return;
}

void task_print_cyphertext()
{
	int i, j = 0;
	digit_t c;
	char line[PRINT_HEX_ASCII_COLS];

	LOG("print cyphertext: len=%u\r\n", P(_v_cyphertext_len));

	PRINTF("TIME end is 65536*%u+%u\r\n",overflow,(unsigned)TBR);

	BLOCK_PRINTF_BEGIN();
	BLOCK_PRINTF("Cyphertext:\r\n");
	for (i = 0; i < P(_v_cyphertext_len); ++i) {
		c = P(_v_cyphertext[i]);
		BLOCK_PRINTF("%02x ", c);
		line[j++] = c;
		if ((i + 1) % PRINT_HEX_ASCII_COLS == 0) {
			BLOCK_PRINTF(" ");
			for (j = 0; j < PRINT_HEX_ASCII_COLS; ++j) {
				c = line[j];
				if (!(32 <= c && c <= 127)) // not printable
					c = '.';
				BLOCK_PRINTF("%c", c);
			}
			j = 0;
			BLOCK_PRINTF("\r\n");
		}
	}
	BLOCK_PRINTF_END();

	exit(0);
	//TRANSITION_TO(task_init);
}

// TODO: this task also looks like a proxy: is it avoidable?
void task_mult_mod()
{
	LOG("mult mod\r\n");

	P(_v_digit) = 0;
	P(_v_carry) = 0;

	os_jump(OFFSET(t_mult_mod, t_mult));
	return;
}

void task_mult()
{
	int i;
	digit_t a, b, c;
	digit_t dp, p;

	LOG("mult: digit=%u carry=%x\r\n", P(_v_digit), P(_v_carry));

	p = P(_v_carry);
	c = 0;
	for (i = 0; i < NUM_DIGITS; ++i) {
		if (P(_v_digit) - i >= 0 && P(_v_digit) - i < NUM_DIGITS) {
			a = P(_v_base[P(_v_digit)-i]);
			b = P(_v_block[i]);
			dp = a * b;

			c += dp >> DIGIT_BITS;
			p += dp & DIGIT_MASK;

			LOG("mult: i=%u a=%x b=%x p=%x\r\n", i, a, b, p);
		}
	}

	c += p >> DIGIT_BITS;
	p &= DIGIT_MASK;

	LOG("mult: c=%x p=%x\r\n", c, p);
	P(_v_product[P(_v_digit)]) = p;
	P(_v_print_which) = 0;
	P(_v_digit)++;

	if (P(_v_digit) < NUM_DIGITS * 2) {
		P(_v_carry) = c;
		os_jump(OFFSET(t_mult, t_mult));
		return;
	} else {
		P(_v_next_task_print) = t_reduce_digits;
		os_jump(OFFSET(t_mult, t_print_product));
		return;
	}
}

void task_reduce_digits()
{
	int d;

	LOG("reduce: digits\r\n");

	// Start reduction loop at most significant non-zero digit
	d = 2 * NUM_DIGITS;
	do {
		d--;
		LOG("reduce digits: p[%u]=%x\r\n", d, P(_v_product[d]));
	} while (P(_v_product[d]) == 0 && d > 0);

	if (P(_v_product[d]) == 0) {
		LOG("reduce: digits: all digits of message are zero\r\n");
		os_jump(OFFSET(t_reduce_digits, t_init));
		return;
	}
	LOG("reduce: digits: d = %u\r\n", d);

	P(_v_reduce) = d;

	os_jump(OFFSET(t_reduce_digits, t_reduce_normalizable));
	return;
}

void task_reduce_normalizable()
{
	int i;
	unsigned m, n, d;
	bool normalizable = true;

	LOG("reduce: normalizable\r\n");

	// Variables:
	//   m: message
	//   n: modulus
	//   b: digit base (2**8)
	//   l: number of digits in the product (2 * NUM_DIGITS)
	//   k: number of digits in the modulus (NUM_DIGITS)
	//
	// if (m > n b^(l-k)
	//     m = m - n b^(l-k)
	//
	// NOTE: It's temptimg to do the subtraction opportunistically, and if
	// the result is negative, then the condition must have been false.
	// However, we can't do that because under this approach, we must
	// write to the output channel zero digits for digits that turn
	// out to be equal, but if a later digit pair is such that condition
	// is false (p < m), then those rights are invalid, but we have no
	// good way of exluding them from being picked up by the later
	// task. One possiblity is to transmit a flag to that task that
	// tells it whether to include our output channel into its input sync
	// statement. However, this seems less elegant than splitting the
	// normalization into two tasks: the condition and the subtraction.
	//
	// Multiplication by a power of radix (b) is a shift, so when we do the
	// comparison/subtraction of the digits, we offset the index into the
	// product digits by (l-k) = NUM_DIGITS.

	//	d = *READ(P(_v_reduce));

	P(_v_offset) = P(_v_reduce) + 1 - NUM_DIGITS; // TODO: can this go below zero
	LOG("reduce: normalizable: d=%u offset=%u\r\n", P(_v_reduce), P(_v_offset));

	for (i = P(_v_reduce); i >= 0; --i) {

		LOG("normalizable: m[%u]=%x n[%u]=%x\r\n", i, P(_v_product[i]), i - P(_v_offset), P(_v_modulus[i-P(_v_offset)]));

		if (P(_v_product[i]) > P(_v_modulus[i-P(_v_offset)])) {
			break;
		} else if (P(_v_product[i]) < P(_v_modulus[i-P(_v_offset)])) {
			normalizable = false;
			break;
		}
	}

	if (!normalizable && P(_v_reduce) == NUM_DIGITS - 1) {
		LOG("reduce: normalizable: reduction done: message < modulus\r\n");

		// TODO: is this copy avoidable? a 'mult mod done' task doesn't help
		// because we need to ship the data to it.
		os_jump(OFFSET(t_reduce_normalizable, P(_v_next_task)));
		return;
	}

	LOG("normalizable: %u\r\n", normalizable);

	if (normalizable) {
		os_jump(OFFSET(t_reduce_normalizable, t_reduce_normalize));
		return;
	} else {
		os_jump(OFFSET(t_reduce_normalizable, t_reduce_n_divisor));
		return;
	}
}

// TODO: consider decomposing into subtasks
void task_reduce_normalize()
{
	digit_t m, n, d, s;
	unsigned borrow;

	LOG("normalize\r\n");

	int i;
	// To call the print task, we need to proxy the values we don't touch
	P(_v_print_which) = 0;

	borrow = 0;
	for (i = 0; i < NUM_DIGITS; ++i) {
		m = P(_v_product[i + P(_v_offset)]);
		n = P(_v_modulus[i]);

		s = n + borrow;
		if (m < s) {
			m += 1 << DIGIT_BITS;
			borrow = 1;
		} else {
			borrow = 0;
		}
		d = m - s;

		LOG("normalize: m[%u]=%x n[%u]=%x b=%u d=%x\r\n",
				i + P(_v_offset), m, i, n, borrow, d);

		P(_v_product[i + P(_v_offset)]) = d;
	}

	// To call the print task, we need to proxy the values we don't touch

	if (P(_v_offset) > 0) { // l-1 > k-1 (loop bounds), where offset=l-k, where l=|m|,k=|n|
		P(_v_next_task_print) = t_reduce_n_divisor;
	} else {
		LOG("reduce: normalize: reduction done: no digits to reduce\r\n");
		// TODO: is this copy avoidable?
		P(_v_next_task_print) = P(_v_next_task);
	}
	os_jump(OFFSET(t_reduce_normalize, t_print_product));
}

void task_reduce_n_divisor()
{
	LOG("reduce: n divisor\r\n");

	// Divisor, derived from modulus, for refining quotient guess into exact value
	P(_v_n_div) = ( P(_v_modulus[NUM_DIGITS - 1])<< DIGIT_BITS) + P(_v_modulus[NUM_DIGITS -2]);

	LOG("reduce: n divisor: n[1]=%x n[0]=%x n_div=%x\r\n", P(_v_modulus[NUM_DIGITS - 1]), P(_v_modulus[NUM_DIGITS -2]), P(_v_n_div));

	os_jump(OFFSET(t_reduce_n_divisor, t_reduce_quotient));
}

void task_reduce_quotient()
{
	digit_t m_n, q;
	uint32_t qn, n_q; // must hold at least 3 digits

	LOG("reduce: quotient: d=%u\r\n", P(_v_reduce));

	// NOTE: we asserted that NUM_DIGITS >= 2, so p[d-2] is safe

	LOG("reduce: quotient: m_n=%x m[d]=%x\r\n", P(_v_modulus[NUM_DIGITS - 1]), P(_v_product[P(_v_reduce)]));

	// Choose an initial guess for quotient
	if (P(_v_product[P(_v_reduce)]) == P(_v_modulus[NUM_DIGITS - 1])) {
		P(_v_quotient) = (1 << DIGIT_BITS) - 1;
	} else {
		P(_v_quotient) = ((P(_v_product[P(_v_reduce)]) << DIGIT_BITS) + P(_v_product[P(_v_reduce) - 1])) / P(_v_modulus[NUM_DIGITS - 1]);
	}

	LOG("reduce: quotient: q0=%x\r\n", q);

	// Refine quotient guess

	// NOTE: An alternative to composing the digits into one variable, is to
	// have a loop that does the comparison digit by digit to implement the
	// condition of the while loop below.
	n_q = ((uint32_t)P(_v_product[P(_v_reduce)]) << (2 * DIGIT_BITS)) + (P(_v_product[P(_v_reduce) - 1]) << DIGIT_BITS) + P(_v_product[P(_v_reduce) - 2]);

	LOG("reduce: quotient: m[d]=%x m[d-1]=%x m[d-2]=%x n_q=%x%x\r\n",
			P(_v_product[P(_v_reduce)]), P(_v_product[P(_v_reduce) - 1]), P(_v_product[P(_v_reduce) - 2]), (uint16_t)((n_q >> 16) & 0xffff), (uint16_t)(n_q & 0xffff));

	LOG("reduce: quotient: n_div=%x q0=%x\r\n", P(_v_n_div), P(_v_quotient));

	P(_v_quotient)++;
	do {
		P(_v_quotient)--;
		qn = mult16(P(_v_n_div), P(_v_quotient));
		//qn = P(_v_n_div) * P(_v_quotient);
		LOG("QN1 = %x\r\n", (uint16_t)((qn >> 16) & 0xffff));
		LOG("QN0 = %x\r\n", (uint16_t)(qn & 0xffff));
		LOG("reduce: quotient: q=%x qn=%x%x\r\n", P(_v_quotient),
				(uint16_t)((qn >> 16) & 0xffff), (uint16_t)(qn & 0xffff));
	} while (qn > n_q);
	// This is still not the final quotient, it may be off by one,
	// which we determine and fix in the 'compare' and 'add' steps.
	LOG("reduce: quotient: q=%x\r\n", P(_v_quotient));

	P(_v_reduce)--;

	os_jump(OFFSET(t_reduce_quotient, t_reduce_multiply));
}

// NOTE: this is multiplication by one digit, hence not re-using mult task
void task_reduce_multiply()
{
	int i;
	digit_t m, n;
	unsigned c, offset;

	LOG("reduce: multiply: d=%x q=%x\r\n", P(_v_reduce) + 1, P(_v_quotient));

	// As part of this task, we also perform the left-shifting of the q*m
	// product by radix^(digit-NUM_DIGITS), where NUM_DIGITS is the number
	// of digits in the modulus. We implement this by fetching the digits
	// of number being reduced at that offset.
	offset = P(_v_reduce) + 1 - NUM_DIGITS;
	LOG("reduce: multiply: offset=%u\r\n", offset);

	// For calling the print task we need to proxy to it values that
	// we do not modify
	for (i = 0; i < offset; ++i) {
		P(_v_product2[i]) = 0;
	}

	// TODO: could convert the loop into a self-edge
	c = 0;
	for (i = offset; i < 2 * NUM_DIGITS; ++i) {

		// This condition creates the left-shifted zeros.
		// TODO: consider adding number of digits to go along with the 'product' field,
		// then we would not have to zero out the MSDs
		m = c;
		if (i < offset + NUM_DIGITS) {
			n = P(_v_modulus[i - offset]);
			LOG("modulus 0: %x\r\n", P(_v_modulus[0]));
			//MC_IN_CH(ch_modulus, task_init, task_reduce_multiply));
			m += P(_v_quotient) * n;
		} else {
			n = 0;
			// TODO: could break out of the loop  in this case (after WRITE)
		}

		LOG("reduce: multiply: n[%u]=%x q=%x c=%x m[%u]=%x\r\n",
				i - offset, n, P(_v_quotient), c, i, m);

		c = m >> DIGIT_BITS;
		m &= DIGIT_MASK;

		P(_v_product2[i]) = m;

	}
	P(_v_print_which) = 1;
	P(_v_next_task_print) = t_reduce_compare;
	os_jump(OFFSET(t_reduce_multiply, t_print_product));
}

void task_reduce_compare()
{
	int i;
	char relation = '=';

	LOG("reduce: compare\r\n");

	// TODO: could transform this loop into a self-edge
	// TODO: this loop might not have to go down to zero, but to NUM_DIGITS
	// TODO: consider adding number of digits to go along with the 'product' field
	for (i = NUM_DIGITS * 2 - 1; i >= 0; --i) {
		LOG("reduce: compare: m[%u]=%x qn[%u]=%x\r\n", i, P(_v_product[i]), i, P(_v_product2[i]));

		if (P(_v_product[i]) > P(_v_product2[i])) {
			relation = '>';
			break;
		} else if (P(_v_product[i]) < P(_v_product2[i])) {
			relation = '<';
			break;
		}
	}

	LOG("reduce: compare: relation %c\r\n", relation);

	if (relation == '<') {
		os_jump(OFFSET(t_reduce_compare, t_reduce_add));
	} else {
		os_jump(OFFSET(t_reduce_compare, t_reduce_subtract));
	}
}

// TODO: this addition and subtraction can probably be collapsed
// into one loop that always subtracts the digits, but, conditionally, also
// adds depending on the result from the 'compare' task. For now,
// we keep them separate for clarity.

void task_reduce_add()
{
	int i, j;
	digit_t m, n, c;
	unsigned offset;

	// Part of this task is to shift modulus by radix^(digit - NUM_DIGITS)
	offset = P(_v_reduce) + 1 - NUM_DIGITS;

	LOG("reduce: add: d=%u offset=%u\r\n", P(_v_reduce) + 1, offset);

	// For calling the print task we need to proxy to it values that
	// we do not modify

	// TODO: coult transform this loop into a self-edge
	c = 0;
	for (i = offset; i < 2 * NUM_DIGITS; ++i) {
		m = P(_v_product[i]);

		// Shifted index of the modulus digit
		j = i - offset;

		if (i < offset + NUM_DIGITS) {
			n = P(_v_modulus[j]);
		} else {
			n = 0;
			j = 0; // a bit ugly, we want 'nan', but ok, since for output only
			// TODO: could break out of the loop in this case (after WRITE)
		}

		P(_v_product[i]) = c + m + n;

		LOG("reduce: add: m[%u]=%x n[%u]=%x c=%x r=%x\r\n", i, m, j, n, c, P(_v_product[i]));

		c = P(_v_product[i]) >> DIGIT_BITS;
		P(_v_product[i]) &= DIGIT_MASK;
	}
	P(_v_print_which) = 0;
	P(_v_next_task_print) = t_reduce_subtract;
	os_jump(OFFSET(t_reduce_add, t_print_product));
}

// TODO: re-use task_reduce_normalize?
void task_reduce_subtract()
{
	LOG("subtract entered!!");
	int i;
	digit_t m, s, qn;
	unsigned borrow, offset;

	// The qn product had been shifted by this offset, no need to subtract the zeros
	offset = P(_v_reduce) + 1 - NUM_DIGITS;

	LOG("reduce: subtract: d=%u offset=%u\r\n", P(_v_reduce) + 1, offset);

	// For calling the print task we need to proxy to it values that
	// we do not modify

	// TODO: could transform this loop into a self-edge
	borrow = 0;
	for (i = 0; i < 2 * NUM_DIGITS; ++i) {
		m = P(_v_product[i]);

		// For calling the print task we need to proxy to it values that we do not modify
		if (i >= offset) {
			qn = P(_v_product2[i]);

			s = qn + borrow;
			if (m < s) {
				m += 1 << DIGIT_BITS;
				borrow = 1;
			} else {
				borrow = 0;
			}
			P(_v_product[i]) = m - s;

			LOG("reduce: subtract: m[%u]=%x qn[%u]=%x b=%u r=%x\r\n",
					i, m, i, qn, borrow, P(_v_product[i]));

		}
	}
	P(_v_print_which) = 0;

	if (P(_v_reduce) + 1 > NUM_DIGITS) {
		P(_v_next_task_print) = t_reduce_quotient;
	} else { // reduction finished: exit from the reduce hypertask (after print)
		LOG("reduce: subtract: reduction done\r\n");

		// TODO: Is it ok to get the next task directly from call channel?
		//       If not, all we have to do is have reduce task proxy it.
		//       Also, do we need a dedicated epilogue task?
		P(_v_next_task_print) = P(_v_next_task);
	}
	os_jump(OFFSET(t_reduce_subtract, t_print_product));
}

// TODO: eliminate from control graph when not verbose
void task_print_product()
{
	int i;

	LOG("print: P=");
	for (i = (NUM_DIGITS * 2) - 1; i >= 0; --i) {
		if(P(_v_print_which)){
			LOG("%x ", P(_v_product2[i]));
		}
		else{
			LOG("%x ", P(_v_product[i]));
		}
	}
	LOG("\r\n");
	LOG("next task is: %u, %u away\r\n", P(_v_next_task_print), OFFSET(t_print_product, P(_v_next_task_print)));
	os_jump(OFFSET(t_print_product, P(_v_next_task_print)));
}

int main(void) {
	init();
	//	PRINTF("INIT\r\n");
	//	for (unsigned k = 0; k < arr_len; ++k) {
	//		PRINTF("arr[%u]: %u\r\n", k, arr[k]);
	//	}
	taskId tasks[] = {{task_init, 0},
		{task_pad, 0},
		{task_exp, 0},
		{task_mult_block, 0},
		{task_mult_block_get_result, 0},
		{task_square_base, 0},
		{task_square_base_get_result, 0},
		{task_print_cyphertext, 0},
		{task_mult_mod, 0},
		{task_mult, 0},
		{task_reduce_digits, 0},
		{task_reduce_normalizable, 0},
		{task_reduce_normalize, 0},
		{task_reduce_n_divisor, 0},
		{task_reduce_quotient, 0},
		{task_reduce_multiply, 0},
		{task_reduce_compare, 0},
		{task_reduce_add, 0},
		{task_reduce_subtract, 0},
		{task_print_product, 0}};
	//This function should be called only once
	os_addTasks(TASK_NUM, tasks );

	os_scheduler();
	return 0;
}
