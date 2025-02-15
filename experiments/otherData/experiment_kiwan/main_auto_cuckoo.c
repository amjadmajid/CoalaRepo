#include <msp430.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <libmspbuiltins/builtins.h>
#include <libio/log.h>
#include <libmsp/mem.h>
#include <libmsp/periph.h>
#include <libmsp/clock.h>
#include <libmsp/watchdog.h>
#include <libmsp/gpio.h>
#include <libipos-gcc-dma/ipos.h>
//#include <libmspmath/msp-math.h>

#ifdef CONFIG_EDB
#include <libedb/edb.h>
#else
#define ENERGY_GUARD_BEGIN()
#define ENERGY_GUARD_END()
#endif

#include "pins.h"

// #define SHOW_PROGRESS_ON_LED
#include <stdint.h>

//#define NUM_BUCKETS 256 // must be a power of 2
#define NUM_BUCKETS 128 // must be a power of 2
//#define NUM_BUCKETS 64 // must be a power of 2
#define MAX_RELOCATIONS 8

void __loop_bound__(unsigned val){};
typedef uint16_t value_t;
typedef uint16_t hash_t;
typedef uint16_t fingerprint_t;
typedef uint16_t index_t; // bucket index

#define NUM_KEYS (NUM_BUCKETS / 4) // shoot for 25% occupancy
#define INIT_KEY 0x1


static void init_hw()
{
	msp_watchdog_disable();
	msp_gpio_unlock();
	msp_clock_setup();
}
__attribute__((always_inline))
void print_filter(fingerprint_t *filter)
{
    unsigned i;
    BLOCK_PRINTF_BEGIN();
    for (i = 0; i < NUM_BUCKETS; ++i) {
        BLOCK_PRINTF("%04x ", filter[i]);
        if (i > 0 && (i + 1) % 8 == 0){
			BLOCK_PRINTF("\r\n");
		}
	}
	BLOCK_PRINTF_END();
}

__attribute__((always_inline))
void log_filter(fingerprint_t *filter)
{
	unsigned i;
	BLOCK_LOG_BEGIN();
	for (i = 0; i < NUM_BUCKETS; ++i) {
		BLOCK_LOG("%04x ", filter[i]);
		if (i > 0 && (i + 1) % 8 == 0)
			BLOCK_LOG("\r\n");
	}
	BLOCK_LOG_END();
}

// TODO: to avoid having to wrap every thing printf macro (to prevent
// a mementos checkpoint in the middle of it, which could be in the
// middle of an EDB energy guard), make printf functions in libio
// and exclude libio from Mementos passes
	__attribute__((always_inline))
void print_stats(unsigned inserts, unsigned members, unsigned total)
{
	    PRINTF("stats: inserts %u members %u total %u\r\n",
	           inserts, members, total);
}

	__attribute__((always_inline))
static hash_t djb_hash(uint8_t* data, unsigned len)
{
	uint32_t hash = 5381;
	unsigned int i;

	for(i = 0; __loop_bound__(2), i < len; data++, i++)
		hash = ((hash << 5) + hash) + (*data);

	return hash & 0xFFFF;
}

	__attribute__((always_inline))
static index_t hash_fp_to_index(fingerprint_t fp)
{
	hash_t hash = djb_hash((uint8_t *)&fp, sizeof(fingerprint_t));
	return hash & (NUM_BUCKETS - 1); // NUM_BUCKETS must be power of 2
}

	__attribute__((always_inline))
static index_t hash_key_to_index(value_t fp)
{
	hash_t hash = djb_hash((uint8_t *)&fp, sizeof(value_t));
	return hash & (NUM_BUCKETS - 1); // NUM_BUCKETS must be power of 2
}

	__attribute__((always_inline))
static fingerprint_t hash_to_fingerprint(value_t key)
{
	return djb_hash((uint8_t *)&key, sizeof(value_t));
}

	__attribute__((always_inline))
static value_t generate_key(value_t prev_key)
{
	// insert pseufo-random integers, for testing
	// If we use consecutive ints, they hash to consecutive DJB hashes...
	// NOTE: we are not using rand(), to have the sequence available to verify
	// that that are no false negatives (and avoid having to save the values).
	return (prev_key + 1) * 17;
}

	__attribute__((always_inline))
static bool insert(fingerprint_t *filter, value_t key)
{
	fingerprint_t fp1, fp2, fp_victim, fp_next_victim;
	index_t index_victim, fp_hash_victim;
	unsigned relocation_count = 0;

	fingerprint_t fp = hash_to_fingerprint(key);

	index_t index1 = hash_key_to_index(key);

	index_t fp_hash = hash_fp_to_index(fp);
	index_t index2 = index1 ^ fp_hash;

	LOG("insert: key %04x fp %04x h %04x i1 %u i2 %u\r\n",
			key, fp, fp_hash, index1, index2);

	fp1 = filter[index1];
	LOG("insert: fp1 %04x\r\n", fp1);
	if (!fp1) { // slot 1 is free
		filter[index1] = fp;
	} else {
		fp2 = filter[index2];
		LOG("insert: fp2 %04x\r\n", fp2);
		if (!fp2) { // slot 2 is free
			filter[index2] = fp;
		} else { // both slots occupied, evict
			if (rand() & 0x80) { // don't use lsb because it's systematic
				index_victim = index1;
				fp_victim = fp1;
			} else {
				index_victim = index2;
				fp_victim = fp2;
			}

			LOG("insert: evict [%u] = %04x\r\n", index_victim, fp_victim);
			filter[index_victim] = fp; // evict victim

			do { // relocate victim(s)
				__loop_bound__(8);
				fp_hash_victim = hash_fp_to_index(fp_victim);
				index_victim = index_victim ^ fp_hash_victim;

				fp_next_victim = filter[index_victim];
				filter[index_victim] = fp_victim;

				LOG("insert: moved %04x to %u; next victim %04x\r\n",
						fp_victim, index_victim, fp_next_victim);

				fp_victim = fp_next_victim;
			} while (fp_victim && ++relocation_count < MAX_RELOCATIONS);

			if (fp_victim) {
				//PRINTF("insert: lost fp %04x\r\n", fp_victim);
				return false;
			}
		}
	}

	return true;
}

	__attribute__((always_inline))
static bool lookup(fingerprint_t *filter, value_t key)
{
	fingerprint_t fp = hash_to_fingerprint(key);

	index_t index1 = hash_key_to_index(key);

	index_t fp_hash = hash_fp_to_index(fp);
	index_t index2 = index1 ^ fp_hash;

	LOG("lookup: key %04x fp %04x h %04x i1 %u i2 %u\r\n",
			key, fp, fp_hash, index1, index2);

	return filter[index1] == fp || filter[index2] == fp;
}

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
void init()
{
		TBCTL &= 0xE6FF; //set 12,11 bit to zero (16bit) also 8 to zero (SMCLK)
		TBCTL |= 0x0200; //set 9 to one (SMCLK)
		TBCTL |= 0x00C0; //set 7-6 bit to 11 (divider = 8);
		TBCTL &= 0xFFEF; //set bit 4 to zero
		TBCTL |= 0x0020; //set bit 5 to one (5-4=10: continuous mode)
		TBCTL |= 0x0002; //interrupt enable*/
	//	TBCTL &= ~(0x0020);
	init_hw();
#ifdef CONFIG_EDB
	edb_init();
#endif

	INIT_CONSOLE();

	__enable_interrupt();
	EIF_PRINTF(".%u.\r\n", 0);
}

int main()
{
	unsigned i;
	value_t key;
	static fingerprint_t filter[NUM_BUCKETS];

	for (i = 0; i < NUM_BUCKETS; ++i)
		filter[i] = 0;

	key = INIT_KEY;
	unsigned inserts = 0;
	for (i = 0; i < NUM_KEYS; ++i) {
		key = generate_key(key);
		bool success = insert(filter, key);
		LOG("insert: key %04x success %u\r\n", key, success);
		if (!success)
			log_filter(filter);

		inserts += success;

	}
	LOG("inserts/total: %u/%u\r\n", inserts, NUM_KEYS);

	key = INIT_KEY;
	unsigned members = 0;
	for (i = 0; i < NUM_KEYS; ++i) {
		key = generate_key(key);
		bool member = lookup(filter, key);
		LOG("lookup: key %04x member %u\r\n", key, member);
		if (!member) {
			fingerprint_t fp = hash_to_fingerprint(key);
			PRINTF("lookup: key %04x fp %04x not member\r\n", key, fp);
		}
		members += member;
	}
	LOG("members/total: %u/%u\r\n", members, NUM_KEYS);

	PRINTF("REAL TIME end is 65536*%u+%u\r\n",overflow,(unsigned)TBR);
	print_filter(filter);
	print_stats(inserts, members, NUM_KEYS);
	exit(0);
	return 0;
}
