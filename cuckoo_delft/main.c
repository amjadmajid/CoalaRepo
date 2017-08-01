#include <stdio.h>

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


#include <ipos.h>
//#include <builtins.h>
//#include <mem.h>
//#include <periph.h>
//#include "pins.h"

#define NUM_BUCKETS 128 // must be a power of 2
#define NUM_INSERTS (NUM_BUCKETS / 4) // shoot for 25% occupancy
#define NUM_LOOKUPS NUM_INSERTS
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

unsigned i;

void task_init()
{

    for (i = 0; i < NUM_BUCKETS ; ++i) {
        WVAR(_v_filter[i], 0);
    }
    WVAR(_v_insert_count, 0);
    WVAR(_v_lookup_count, 0);
    WVAR(_v_inserted_count, 0);
    WVAR(_v_member_count, 0);
    WVAR(_v_key, init_key);
    WVAR(_v_next_task, t_insert);

    os_jump(2);
}

void task_init_array() {
    unsigned i;
    uint16_t __cry =  RVAR(_v_index);
    for (i = 0; i < BUFFER_SIZE - 1; ++i) {
        WVAR(_v_filter[i +__cry*(BUFFER_SIZE-1)], 0);
    }
    ++__cry;
    if (__cry== NUM_BUCKETS/(BUFFER_SIZE-1)) {
        os_jump(1);
    }
    else {
        os_jump(0);
    }
    WVAR(_v_index, __cry);
}

void task_generate_key()
{
    // insert pseufo-random integers, for testing
    // If we use consecutive ints, they hash to consecutive DJB hashes...
    // NOTE: we are not using rand(), to have the sequence available to verify
    // that that are no false negatives (and avoid having to save the values).
    uint16_t * ppt = PVAR(_v_key);
    (*ppt) = ( (*ppt) + 1) * 17;

    if (RVAR(_v_next_task) >= t_generate_key) {
        os_jump(RVAR(_v_next_task) - t_generate_key);
    }
    else {
        os_jump(TASK_NUM - RVAR(_v_next_task) + t_generate_key);
    }
}

void task_calc_indexes()
{
    uint16_t __cry = hash_to_fingerprint(RVAR(_v_key));
    P(_v_fingerprint) = __cry;

    os_jump(1);
}

void task_calc_indexes_index_1()
{
     uint16_t __cry = hash_to_index(RVAR(_v_key));
     WVAR(_v_index1, __cry);
    os_jump(1);
}

void task_calc_indexes_index_2()
{
    index_t fp_hash = hash_to_index(RVAR(_v_fingerprint));
    uint16_t __cry;
     __cry = RVAR(_v_index1) ^ fp_hash;
     WVAR(_v_index2, __cry);

     uint16_t * ppt = PVAR(_v_next_task);
    if ( (*ppt) >= t_calc_indexes_index_2) {
        os_jump((*ppt) - t_calc_indexes_index_2);
    }
    else {
        os_jump(TASK_NUM - (*ppt) + t_calc_indexes_index_2);
    }
}



// This task is redundant.
// Alpaca never needs this but since Chain code had it, leaving it for fair comparison.
void task_insert()
{
    WVAR(_v_next_task, t_add);
    os_jump(12);
}


void task_add()
{
    uint16_t __cry;
    uint16_t __cry_idx = RVAR(_v_index1);
    uint16_t __cry_idx2 = RVAR(_v_index2);
    if (!RVAR(_v_filter[__cry_idx])) {

        WVAR(_v_success, true);
        __cry = RVAR(_v_fingerprint);
        WVAR(_v_filter[__cry_idx], __cry);
        os_jump(2);
        return;
    } else {
        if (!RVAR(_v_filter[__cry_idx2])) {

            WVAR(_v_success, true);
            __cry = RVAR(_v_fingerprint);
            WVAR(_v_filter[__cry_idx2],__cry);
            os_jump(2);
            return;
        } else { // evict one of the two entries
            fingerprint_t fp_victim;
            index_t index_victim;

            if (rand() % 2) {
                index_victim = __cry_idx;
                fp_victim = RVAR(_v_filter[__cry_idx]);
            } else {
                index_victim = __cry_idx2;
                fp_victim = RVAR(_v_filter[__cry_idx2]);
            }

            // Evict the victim
            __cry = RVAR(_v_fingerprint);
            WVAR(_v_filter[P(index_victim)], __cry);
            WVAR(_v_index1, index_victim);
            WVAR(_v_fingerprint,fp_victim);
            WVAR(_v_relocation_count, 0);

            os_jump(1);
            return;
        }
    }
}



void task_relocate()
{
    uint16_t __cry;
    fingerprint_t fp_victim = RVAR(_v_fingerprint);
    index_t fp_hash_victim = hash_to_index(fp_victim);
    index_t index2_victim = RVAR(_v_index1) ^ fp_hash_victim;

    if (!RVAR(_v_filter[index2_victim])) { // slot was free
        WVAR(_v_success, true);
        WVAR(_v_filter[index2_victim],fp_victim);
        os_jump(1);
        return;
    } else { // slot was occupied, rellocate the next victim

        if (RVAR(_v_relocation_count) >= MAX_RELOCATIONS) { // insert failed
            WVAR(_v_success, false);
            os_jump(1);
            return;
        }
        __cry = _v_relocation_count;
        __cry++;
        WVAR(_v_relocation_count, __cry);
        WVAR(_v_index1, index2_victim);
        __cry = RVAR(_v_filter[index2_victim]);
        WVAR(_v_fingerprint, __cry);
        WVAR(_v_filter[index2_victim], fp_victim);
        os_jump(0);
        return;
    }
}


//#if 0

void task_insert_done()
{
    uint16_t __cry;
    __cry = _v_insert_count;
    ++__cry;
    WVAR(_v_insert_count, __cry);
    __cry = RVAR(_v_inserted_count);
    __cry+= RVAR(_v_success);
    WVAR(_v_inserted_count, __cry);


    if (RVAR(_v_insert_count) < NUM_INSERTS) {
        WVAR(_v_next_task, t_insert);
        os_jump(8);
        return;
    } else {
        WVAR(_v_next_task, t_lookup);
        WVAR(_v_key,init_key);
        os_jump(8);
        return;
    }
}

void task_lookup()
{

    WVAR(_v_next_task, t_lookup_search);
    os_jump(8);
}

void task_lookup_search()
{

    if (RVAR(_v_filter[RVAR(_v_index1)]) == RVAR(_v_fingerprint)) {
        WVAR(_v_member, true);
    } else {

        if (RVAR(_v_filter[P(_v_index2)]) == RVAR(_v_fingerprint)) {
            WVAR(_v_member, true);
        }
        else {
            WVAR(_v_member, false);
        }
    }

    os_jump(1);
}

void task_lookup_done()
{
    uint16_t __cry;
    __cry = RVAR(_v_lookup_count);
    __cry++;
    WVAR(_v_lookup_count, __cry);
    __cry = RVAR(_v_member_count) ;
    __cry+= RVAR(_v_member);
    WVAR(_v_member_count, __cry);

    if (RVAR(_v_lookup_count) < NUM_LOOKUPS) {
        WVAR(_v_next_task, t_lookup);
        os_jump(5);
        return;
    } else {
        os_jump(1);
        return;
    }
}

void task_print_stats()
{
    __no_operation();
;
}

void task_done()
{

;

}

//#endif

void init()
{
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    CSCTL0_H = CSKEY >> 8;                // Unlock CS registers
//    CSCTL1 = DCOFSEL_4 |  DCORSEL;      // Set DCO to 16MHz
    CSCTL1 = DCOFSEL_6;                   // Set DCO to 8MHz
    CSCTL2 =  SELM__DCOCLK;               // MCLK = DCO
    CSCTL3 = DIVM__1;                     // divide the DCO frequency by 1
    CSCTL0_H = 0;

    srand(0);

    __enable_interrupt();

}
int main(void) {
    init();

    taskId tasks[] = {{task_init, 1},
        {task_init_array, 2} ,
        {task_generate_key, 3},
        {task_calc_indexes, 4},
        {task_calc_indexes_index_1, 5},
        {task_calc_indexes_index_2, 6},
        {task_insert, 7},
        {task_add, 8},
        {task_relocate, 9},
        {task_insert_done, 10},
        {task_lookup, 11},
        {task_lookup_search, 12},
        {task_lookup_done, 13},
        {task_print_stats, 14},
        {task_done, 15}
    };

    os_addTasks(TASK_NUM, tasks );

    os_scheduler();
    return 0;
}
