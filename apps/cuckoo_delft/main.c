#include <stdio.h>

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <mspReseter.h>
#include "mspProfiler.h"
#include "mspDebugger.h"

#include <ipos.h>


#define TSK_SIZ
#define AUTO_RST
#define LOG_INFO


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

#define OFFSET(src, dest) src <= dest ? dest - src : TASK_NUM + dest - src

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

__nv uint8_t pinRaised=0;

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
#ifdef TSK_SIZ
       cp_reset();
#endif
    unsigned i;
        for (i = 0; i < NUM_BUCKETS ; ++i) {
            WP(_v_filter[i]) = 0;
        }
        WP(_v_insert_count) = 0;
        WP(_v_lookup_count) = 0;
        WP(_v_inserted_count) = 0;
        WP(_v_member_count) = 0;
        WP(_v_key) = init_key;
        WP(_v_next_task) = t_insert;

        os_jump(OFFSET(t_init, t_generate_key));
#ifdef TSK_SIZ
     cp_sendRes("task_init \0");
#endif
}

void task_init_array() {
#ifdef TSK_SIZ
       cp_reset();
#endif
    unsigned i;
        for (i = 0; i < BUFFER_SIZE - 1; ++i) {
            WP(_v_filter[i + P(_v_index)*(BUFFER_SIZE-1)]) = 0;
        }
        ++WP(_v_index);
        if (RP(_v_index) == NUM_BUCKETS/(BUFFER_SIZE-1)) {
//            os_jump(1);
        }
        else {
            os_jump(0);
        }

#ifdef TSK_SIZ
     cp_sendRes("task_init_array \0");
#endif
}

void task_generate_key()
{
#ifdef TSK_SIZ
       cp_reset();
#endif
    // insert pseufo-random integers, for testing
    // If we use consecutive ints, they hash to consecutive DJB hashes...
    // NOTE: we are not using rand(), to have the sequence available to verify
    // that that are no false negatives (and avoid having to save the values).

    uint16_t __cry;

    __cry = (RP(_v_key) + 1) * 17;
    WP(_v_key) = __cry;

    if (P(_v_next_task) >= t_generate_key) {
        os_jump(P(_v_next_task) - t_generate_key);
    }
    else {
        os_jump(TASK_NUM - RP(_v_next_task) + t_generate_key);
    }

#ifdef TSK_SIZ
   cp_sendRes("task_generate_key \0");
#endif
}

void task_calc_indexes()
{
#ifdef TSK_SIZ
       cp_reset();
#endif

    uint16_t __cry;
    __cry = hash_to_fingerprint(RP(_v_key));
    WP(_v_fingerprint) = __cry;

    os_jump(1);

#ifdef TSK_SIZ
    cp_sendRes("task_calc_indexes \0");
#endif
}

void task_calc_indexes_index_1()
{
#ifdef TSK_SIZ
       cp_reset();
#endif

    uint16_t __cry;
    __cry = hash_to_index(RP(_v_key));
    WP(_v_index1) = __cry;

    os_jump(1);

#ifdef TSK_SIZ
    cp_sendRes("task_calc_indexes_index_1 \0");
#endif
}

void task_calc_indexes_index_2()
{
#ifdef TSK_SIZ
       cp_reset();
#endif
    index_t fp_hash = hash_to_index(RP(_v_fingerprint));
    uint16_t __cry;
     __cry = RP(_v_index1) ^ fp_hash;
     WP(_v_index2) = __cry;

    if (P(_v_next_task) >= t_calc_indexes_index_2) {
        os_jump(RP(_v_next_task) - t_calc_indexes_index_2);
    }
    else {
        os_jump(TASK_NUM - RP(_v_next_task) + t_calc_indexes_index_2);
    }

#ifdef TSK_SIZ
    cp_sendRes("task_calc_indexes_index_2 \0");
#endif
}

// This task is redundant.
// Alpaca never needs this but since Chain code had it, leaving it for fair comparison.
void task_insert()
{
#ifdef TSK_SIZ
       cp_reset();
#endif
    WP(_v_next_task) = t_add;
    os_jump(12);

#ifdef TSK_SIZ
   cp_sendRes("task_insert \0");
#endif
}


void task_add()
{
#ifdef TSK_SIZ
       cp_reset();
#endif
    uint16_t __cry;
    uint16_t __cry_idx = RP(_v_index1);
    uint16_t __cry_idx2 = RP(_v_index2);
    if (!P(_v_filter[__cry_idx])) {

        WP(_v_success) = true;
        __cry = RP(_v_fingerprint);
        WP(_v_filter[__cry_idx]) = __cry;
        os_jump(2);
        return;
    } else {
        if (!P(_v_filter[__cry_idx2])) {

            WP(_v_success) = true;
            __cry = RP(_v_fingerprint);
            WP(_v_filter[__cry_idx2])  = __cry;
            os_jump(2);
            return;
        } else { // evict one of the two entries
            fingerprint_t fp_victim;
            index_t index_victim;

            if (rand() % 2) {
                index_victim = __cry_idx;
                fp_victim = RP(_v_filter[__cry_idx]);
            } else {
                index_victim = __cry_idx2;
                fp_victim = RP(_v_filter[__cry_idx2]);
            }

            // Evict the victim
            __cry = RP(_v_fingerprint);
            WP(_v_filter[RP(index_victim)])  = __cry;
            WP(_v_index1) = index_victim;
            WP(_v_fingerprint) = fp_victim;
            WP(_v_relocation_count) = 0;

//            os_jump(1);
            return;
        }
    }
#ifdef TSK_SIZ
    cp_sendRes("task_add \0");
#endif
}



void task_relocate()
{
#ifdef TSK_SIZ
       cp_reset();
#endif
    uint16_t __cry;
    fingerprint_t fp_victim = RP(_v_fingerprint);
    index_t fp_hash_victim = hash_to_index(fp_victim);
    index_t index2_victim = RP(_v_index1) ^ fp_hash_victim;

    if (!RP(_v_filter[index2_victim])) { // slot was free
        WP(_v_success) = true;
        WP(_v_filter[index2_victim]) = fp_victim;
//        os_jump(1);
        return;
    } else { // slot was occupied, rellocate the next victim

        if (RP(_v_relocation_count) >= MAX_RELOCATIONS) { // insert failed
            WP(_v_success) = false;
//            os_jump(1);
            return;
        }

        ++WP(_v_relocation_count);
        WP(_v_index1) = index2_victim;
        __cry = RP(_v_filter[index2_victim]);
        WP(_v_fingerprint) = __cry;
        WP(_v_filter[index2_victim]) = fp_victim;
        os_jump(0);
        return;
    }

#ifdef TSK_SIZ
    cp_sendRes("task_relocate \0");
#endif
}


//#if 0

void task_insert_done()
{
#ifdef TSK_SIZ
       cp_reset();
#endif
    uint16_t __cry;
    ++WP(_v_insert_count);
    __cry = RP(_v_inserted_count);
    __cry+= RP(_v_success);
    WP(_v_inserted_count) = __cry;


    if (P(_v_insert_count) < NUM_INSERTS) {
        WP(_v_next_task) = t_insert;
        os_jump(8);
        return;
    } else {
        WP(_v_next_task) = t_lookup;
        WP(_v_key) = init_key;
        os_jump(8);
        return;
    }

#ifdef TSK_SIZ
    cp_sendRes("task_insert_done \0");
#endif
}

void task_lookup()
{
#ifdef TSK_SIZ
       cp_reset();
#endif
    WP(_v_next_task) = t_lookup_search;
    os_jump(8);

#ifdef TSK_SIZ
    cp_sendRes("task_lookup \0");
#endif
}

void task_lookup_search()
{
#ifdef TSK_SIZ
       cp_reset();
#endif
    if (RP(_v_filter[RP(_v_index1)]) == RP(_v_fingerprint)) {
        WP(_v_member) = true;
    } else {

        if (RP(_v_filter[RP(_v_index2)]) == RP(_v_fingerprint)) {
            WP(_v_member) = true;
        }
        else {
            WP(_v_member) = false;
        }
    }

//    os_jump(1);
#ifdef TSK_SIZ
      cp_sendRes("task_lookup_search \0");
#endif
}

void task_lookup_done()
{

#ifdef TSK_SIZ
       cp_reset();
#endif

    uint16_t __cry;
    ++WP(_v_lookup_count);
    __cry = P(_v_member_count) ;
    __cry+= P(_v_member);
    WP(_v_member_count)  = __cry;

    if (P(_v_lookup_count) < NUM_LOOKUPS) {
        WP(_v_next_task) = t_lookup;
        os_jump(5);
        return;
    } else {
//        os_jump(1);
        return;
    }

#ifdef TSK_SIZ
     cp_sendRes("task_lookup_done \0");
#endif
}

void task_print_stats()
{
//    __no_operation();
    pinRaised=1;

}

void task_done()
{
#ifdef TSK_SIZ
       cp_reset();
#endif

    if(pinRaised){
        P3OUT |= BIT5;
        P3OUT &= ~BIT5;
    }
    pinRaised=0;

#ifdef TSK_SIZ
     cp_sendRes("task_done \0");
#endif
}


void init()
{
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    P3OUT &= ~BIT5;
    P3DIR |=BIT5;

#if 0
    CSCTL0_H = CSKEY >> 8;                // Unlock CS registers
//    CSCTL1 = DCOFSEL_4 |  DCORSEL;      // Set DCO to 16MHz
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
int main(void) {
    init();

    taskId tasks[] = {{task_init, 0},
        {task_init_array, 0} ,
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
        {task_done, 0}
    };

    os_addTasks(TASK_NUM, tasks );

    os_scheduler();
    return 0;
}
