#include <stdio.h>

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <libipos-gcc-dma/ipos.h>
#include <libmspbuiltins/builtins.h>
#include <libio/log.h>
#include <libmsp/mem.h>
#include <libmsp/periph.h>
#include <libmsp/clock.h>
#include <libmsp/watchdog.h>
#include <libmsp/gpio.h>
#ifdef CONFIG_EDB
#include <libedb/edb.h>
#endif

#include "pins.h"

// timer for measuring performance
unsigned volatile *timer = &TBCTL;
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

//unsigned count = 0;

__attribute__((section("__interrupt_vector_timer0_b1"),aligned(2)))
void(*__vector_timer0_b1)(void) = TimerB1_ISR;

#define NUM_INSERTS (NUM_BUCKETS / 4) // shoot for 25% occupancy
#define NUM_LOOKUPS NUM_INSERTS
#define NUM_BUCKETS 128 // must be a power of 2
#define MAX_RELOCATIONS 8
#define BUFFER_SIZE 32

typedef uint16_t value_t;
typedef uint16_t hash_t;
typedef uint16_t fingerprint_t;
typedef uint16_t index_t; // bucket index

typedef struct _insert_count {
	unsigned insert_count;
	unsigned inserted_count;
} insert_count_t;

typedef struct _lookup_count {
	unsigned lookup_count;
	unsigned member_count;
} lookup_count_t;

#define TASK_NUM 15

enum task_index {
	t_init,
	t_init_array,
	t_generate_key,
	t_calc_indexes,
	t_calc_indexes_index_1,

	t_calc_indexes_index_2,
	t_insert,
	t_add,
	t_relocate,
	t_insert_done,

	t_lookup,
	t_lookup_search,
	t_lookup_done,
	t_print_stats,
	t_done
};

void task_init();
void task_init_array();
void task_generate_key();
void task_calc_indexes();
void task_calc_indexes_index_1();
void task_calc_indexes_index_2();
void task_insert();
void task_add();
void task_relocate();
void task_insert_done();
void task_lookup();
void task_lookup_search();
void task_lookup_done();
void task_print_stats();
void task_done();

__p fingerprint_t _v_filter[NUM_BUCKETS];
__p index_t _v_index;
__p value_t _v_key;
//__p task_t* _v_next_task;
__p enum task_index _v_next_task;
__p fingerprint_t _v_fingerprint;
__p value_t _v_index1;
__p value_t _v_index2;
__p value_t _v_relocation_count;
__p value_t _v_insert_count;
__p value_t _v_inserted_count;
__p value_t _v_lookup_count;
__p value_t _v_member_count;
__p bool _v_success;
__p bool _v_member;

static value_t init_key = 0x0001; // seeds the pseudo-random sequence of keys

static hash_t djb_hash(uint8_t* data, unsigned len)
{
	uint16_t hash = 5381;
	unsigned int i;

	for(i = 0; i < len; data++, i++)
		hash = ((hash << 5) + hash) + (*data);


	return hash & 0xFFFF;
}

static index_t hash_to_index(fingerprint_t fp)
{
	hash_t hash = djb_hash((uint8_t *)&fp, sizeof(fingerprint_t));
	return hash & (NUM_BUCKETS - 1); // NUM_BUCKETS must be power of 2
}

static fingerprint_t hash_to_fingerprint(value_t key)
{
	return djb_hash((uint8_t *)&key, sizeof(value_t));
}

void task_init()
{
	unsigned i;
	for (i = 0; i < NUM_BUCKETS ; ++i) {
		P(_v_filter[i]) = 0;
	}
	P(_v_insert_count) = 0;
	P(_v_lookup_count) = 0;
	P(_v_inserted_count) = 0;
	P(_v_member_count) = 0;
	P(_v_key) = init_key;
	P(_v_next_task) = t_insert;
	LOG("init end!!\r\n");
	os_jump(2);
}
void task_init_array() {
	LOG("init array start\r\n");
	unsigned i;
	for (i = 0; i < BUFFER_SIZE - 1; ++i) {
		P(_v_filter[i + P(_v_index)*(BUFFER_SIZE-1)]) = 0;
	}
	++P(_v_index);
	if (P(_v_index) == NUM_BUCKETS/(BUFFER_SIZE-1)) {
		os_jump(1);
	}
	else {
		os_jump(0);
	}
}
void task_generate_key()
{
	LOG("generate key start\r\n");

	// insert pseufo-random integers, for testing
	// If we use consecutive ints, they hash to consecutive DJB hashes...
	// NOTE: we are not using rand(), to have the sequence available to verify
	// that that are no false negatives (and avoid having to save the values).
	P(_v_key) = (P(_v_key) + 1) * 17;
	LOG("generate_key: key: %x\r\n", P(_v_key));
	if (P(_v_next_task) >= t_generate_key) {
		os_jump(P(_v_next_task) - t_generate_key);
	}
	else {
		os_jump(TASK_NUM - P(_v_next_task) + t_generate_key);	
	}
}

void task_calc_indexes()
{
	P(_v_fingerprint) = hash_to_fingerprint(P(_v_key));
	LOG("calc indexes: fingerprint: key %04x fp %04x\r\n", P(_v_key), P(_v_fingerprint));

	os_jump(1);
}

void task_calc_indexes_index_1()
{
	P(_v_index1) = hash_to_index(P(_v_key));
	LOG("calc indexes: index1: key %04x idx1 %u\r\n", P(_v_key), P(_v_index1));
	os_jump(1);
}

void task_calc_indexes_index_2()
{
	index_t fp_hash = hash_to_index(P(_v_fingerprint));
	P(_v_index2) = P(_v_index1) ^ fp_hash;

	LOG("calc indexes: index2: fp hash: %04x idx1 %u idx2 %u\r\n",
			fp_hash, P(_v_index1), P(_v_index2));
	if (P(_v_next_task) >= t_calc_indexes_index_2) {
		os_jump(P(_v_next_task) - t_calc_indexes_index_2);
	}
	else {
		os_jump(TASK_NUM - P(_v_next_task) + t_calc_indexes_index_2);	
	}
}

// This task is redundant.
// Alpaca never needs this but since Chain code had it, leaving it for fair comparison.
void task_insert()
{
	LOG("insert: key %04x\r\n", P(_v_key));
	P(_v_next_task) = t_add; 
	os_jump(12);
}

void task_add()
{
	// Fingerprint being inserted
	LOG("add: fp %04x\r\n", P(_v_fingerprint));

	// index1,fp1 and index2,fp2 are the two alternative buckets
	LOG("add: idx1 %u fp1 %04x\r\n", P(_v_index1), P(_v_filter[P(_v_index1)]));

	if (!P(_v_filter[P(_v_index1)])) {
		LOG("add: filled empty slot at idx1 %u\r\n", P(_v_index1));

		P(_v_success) = true;
		P(_v_filter[P(_v_index1)]) = P(_v_fingerprint);
		os_jump(2);
		return;
	} else {
		LOG("add: fp2 %04x\r\n", P(_v_filter[P(_v_index2)]));
		if (!P(_v_filter[P(_v_index2)])) {
			LOG("add: filled empty slot at idx2 %u\r\n", P(_v_index2));

			P(_v_success) = true;
			P(_v_filter[P(_v_index2)]) = P(_v_fingerprint);
			os_jump(2);
			return;
		} else { // evict one of the two entries
			fingerprint_t fp_victim;
			index_t index_victim;

			if (rand() % 2) {
				index_victim = P(_v_index1);
				fp_victim = P(_v_filter[P(_v_index1)]);
			} else {
				index_victim = P(_v_index2);
				fp_victim = P(_v_filter[P(_v_index2)]);
			}

			LOG("add: evict [%u] = %04x\r\n", index_victim, fp_victim);

			// Evict the victim
			P(_v_filter[P(index_victim)]) = P(_v_fingerprint);
			P(_v_index1) = index_victim;
			P(_v_fingerprint) = fp_victim;
			P(_v_relocation_count) = 0;

			os_jump(1);
			return;
		}
	}
}

void task_relocate()
{
	fingerprint_t fp_victim = P(_v_fingerprint);
	index_t fp_hash_victim = hash_to_index(fp_victim);
	index_t index2_victim = P(_v_index1) ^ fp_hash_victim;

	LOG("relocate: victim fp hash %04x idx1 %u idx2 %u\r\n",
			fp_hash_victim, P(_v_index1), index2_victim);

	LOG("relocate: next victim fp %04x\r\n", P(_v_filter[index2_victim]));


	if (!P(_v_filter[index2_victim])) { // slot was free
		P(_v_success) = true;
		P(_v_filter[index2_victim]) = fp_victim;
		os_jump(1);
		return;
	} else { // slot was occupied, rellocate the next victim

		LOG("relocate: relocs %u\r\n", P(_v_relocation_count));

		if (P(_v_relocation_count) >= MAX_RELOCATIONS) { // insert failed
			PRINTF("relocate: max relocs reached: %u\r\n", P(_v_relocation_count));
			P(_v_success) = false;
			os_jump(1);
			return;
		}

		++P(_v_relocation_count);
		P(_v_index1) = index2_victim;
		P(_v_fingerprint) = P(_v_filter[index2_victim]);
		P(_v_filter[index2_victim]) = fp_victim;
		os_jump(0);
		return;
	}
}

void task_insert_done()
{
#if VERBOSE > 0
	unsigned i;

	LOG("insert done: filter:\r\n");
	for (i = 0; i < NUM_BUCKETS; ++i) {

		LOG("%04x ", P(_v_filter[i]));
		if (i > 0 && (i + 1) % 8 == 0)
			LOG("\r\n");
	}
	LOG("\r\n");
#endif

	++P(_v_insert_count);
	P(_v_inserted_count) += P(_v_success);

	LOG("insert done: insert %u inserted %u\r\n", P(_v_insert_count), P(_v_inserted_count));

	if (P(_v_insert_count) < NUM_INSERTS) {
		P(_v_next_task) = t_insert; 
		os_jump(8);
		return;
	} else {
		P(_v_next_task) = t_lookup; 
		P(_v_key) = init_key;
		os_jump(8);
		return;
	}
}

void task_lookup()
{
	LOG("lookup: key %04x\r\n", P(_v_key));
	P(_v_next_task) = t_lookup_search;
	os_jump(8);
}

void task_lookup_search()
{
	LOG("lookup search: fp %04x idx1 %u idx2 %u\r\n", P(_v_fingerprint), P(_v_index1), P(_v_index2));
	LOG("lookup search: fp1 %04x\r\n", P(_v_filter[P(_v_index1)]));

	if (P(_v_filter[P(_v_index1)]) == P(_v_fingerprint)) {
		P(_v_member) = true;
	} else {
		LOG("lookup search: fp2 %04x\r\n", P(_v_filter[P(_v_index2)]));

		if (P(_v_filter[P(_v_index2)]) == P(_v_fingerprint)) {
			P(_v_member) = true;
		}
		else {
			P(_v_member) = false;
		}
	}

	LOG("lookup search: fp %04x member %u\r\n", P(_v_fingerprint), P(_v_member));

	if (!P(_v_member)) {
		PRINTF("lookup: key %04x not member\r\n", P(_v_fingerprint));
	}

	os_jump(1);
}

void task_lookup_done()
{
	++P(_v_lookup_count);

	P(_v_member_count) += P(_v_member);
	LOG("lookup done: lookups %u members %u\r\n", P(_v_lookup_count), P(_v_member_count));

	if (P(_v_lookup_count) < NUM_LOOKUPS) {
		P(_v_next_task) = t_lookup;
		os_jump(5);
		return;
	} else {
		os_jump(1);
		return;
	}
}

void task_print_stats()
{
	unsigned i;

	PRINTF("REAL TIME end is 65536*%u+%u\r\n",overflow,(unsigned)TBR);
	BLOCK_PRINTF_BEGIN();
	BLOCK_PRINTF("filter:\r\n");
	for (i = 0; i < NUM_BUCKETS; ++i) {
		BLOCK_PRINTF("%04x ", P(_v_filter[i]));
		if (i > 0 && (i + 1) % 8 == 0){
			BLOCK_PRINTF("\r\n");
		}
	}
	BLOCK_PRINTF_END();
	PRINTF("stats: inserts %u members %u total %u\r\n",
			P(_v_inserted_count), P(_v_member_count), NUM_INSERTS);
	os_jump(1);
}

void task_done()
{
	//	count++;
	//	if(count == 5){
	//		count = 0;
	exit(0);
	//	}
	//	TRANSITION_TO(task_init);
}
static void init_hw()
{
	msp_watchdog_disable();
	msp_gpio_unlock();
	msp_clock_setup();
}

void init()
{
	// set timer for measuring time
#ifdef BOARD_MSP_TS430
	*timer &= 0xE6FF; //set 12,11 bit to zero (16bit) also 8 to zero (SMCLK)
	*timer |= 0x0200; //set 9 to one (SMCLK)
	*timer |= 0x00C0; //set 7-6 bit to 11 (divider = 8);
	*timer &= 0xFFEF; //set bit 4 to zero
	*timer |= 0x0020; //set bit 5 to one (5-4=10: continuous mode)
	*timer |= 0x0002; //interrupt enable
#endif
	//	*timer &= ~(0x0020); //set bit 5 to zero(halt!)
	init_hw();

#ifdef CONFIG_EDB
	edb_init();
#endif

	INIT_CONSOLE();

	__enable_interrupt();

	//	PRINTF(".%u.\r\n", curctx->task->idx);
}
int main(void) {
	init();
	//	PRINTF("INIT\r\n");
	//	for (unsigned k = 0; k < arr_len; ++k) {
	//		PRINTF("arr[%u]: %u\r\n", k, arr[k]);
	//	}
	taskId tasks[] = {{task_init, 0},
		{task_init_array, 0},
		{task_generate_key, 0},
		{task_calc_indexes, 0},
		{task_calc_indexes_index_1, 0},
		{task_calc_indexes_index_2, 0},
		{task_insert, 0},
		{task_add, 0},
		{task_relocate, 0},
		{task_insert_done, 0},
		{task_lookup, 0},
		{task_lookup_search, 0},
		{task_lookup_done, 0},
		{task_print_stats, 0},
		{task_done, 0}};

	//This function should be called only once
	os_addTasks(TASK_NUM, tasks );

	os_scheduler();
	return 0;
}
