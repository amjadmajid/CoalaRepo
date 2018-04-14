#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

#include <mspReseter.h>
#include <mspProfiler.h>
#include <mspDebugger.h>
#include <mspbase.h>
#include <mspprintf.h>

#include <alpaca.h>

// Only for profiling, removable otherwise
#include <ctlflags.h>

// Profiling flags.
#if ENABLE_PRF
__nv uint8_t full_run_started = 0;
__nv uint8_t first_run = 1;
#endif

#ifndef RST_TIME
#define RST_TIME 25000
#endif


#define N_NODES   25
#define Q_SIZE    4 * N_NODES

#define INFINITY  0xFFFF
#define UNDEFINED 0xFFFF

typedef struct {
	uint16_t dist;
	uint16_t prev;
} node_t;

typedef struct {
	uint16_t node;
	uint16_t dist;
	uint16_t prev;
} queue_t;

#include "data.h"

// Tasks.
TASK(1, task_init)
TASK(2, task_init_list)
TASK(3, task_select_nearest_node)
TASK(4, task_find_shorter_path)
TASK(5, task_done)

// Task-shared protected variables.
GLOBAL_SB(node_t, node_list, N_NODES);
GLOBAL_SB(queue_t, queue, Q_SIZE); // largest size?
GLOBAL_SB(uint16_t, deq_idx);
GLOBAL_SB(uint16_t, enq_idx);
GLOBAL_SB(uint16_t, node_idx);
GLOBAL_SB(uint16_t, src_node);
GLOBAL_SB(queue_t, nearest_node);

/* This is originally done by the compiler */
__nv uint8_t* data_src[3];
__nv uint8_t* data_dest[3];
__nv unsigned data_size[3];
GLOBAL_SB(node_t, node_list_bak, N_NODES);
GLOBAL_SB(uint16_t, node_list_isDirty, N_NODES);
GLOBAL_SB(uint16_t, deq_idx_bak);
GLOBAL_SB(uint16_t, enq_idx_bak);
GLOBAL_SB(uint16_t, node_idx_bak);
GLOBAL_SB(uint16_t, src_node_bak);
void clear_isDirty() {
    // PRINTF("clear\r\n");
    memset(&GV(node_list_isDirty, 0), 0, sizeof(_global_node_list_isDirty));
}
/* end */


void task_init()
{
#if ENABLE_PRF
    full_run_started = 1;
#endif

    PRIV(enq_idx);
	
    GV(deq_idx) = 0;
    GV(enq_idx_bak) = 0;

    // Enqueue.
	GV(queue, 0).node = GV(src_node);
	GV(queue, 0).dist = 0;
	GV(queue, 0).prev = UNDEFINED;
	++GV(enq_idx_bak);
	// LOG("E: %u, D: %u\n", GV(enq_idx_bak), GV(deq_idx));

	COMMIT(enq_idx);
	TRANSITION_TO(task_init_list);
}


void task_init_list()
{
	PRIV(src_node);

    uint16_t i, sn;

    for (i = 0; i < N_NODES; i++) {
    	GV(node_list, i).dist = INFINITY;
    	GV(node_list, i).prev = UNDEFINED;
    }

    sn = GV(src_node_bak);
    GV(node_list, sn).dist = 0;
    GV(node_list, sn).prev = UNDEFINED;

    sn++;
    // Test nodes 0, 1, 2, 3.
    if (sn < 4) {
    	GV(src_node_bak) = sn;
    } else {
    	GV(src_node_bak) = 0;
    }

	COMMIT(src_node);
    TRANSITION_TO(task_select_nearest_node);
}


void task_select_nearest_node()
{
	PRIV(deq_idx);
	
	uint16_t i = GV(deq_idx_bak);

	if (GV(enq_idx) != i) {
		
		// Dequeue nearest node.
		GV(nearest_node) = GV(queue, i);
		i++;
		if (i < Q_SIZE) {
			GV(deq_idx_bak) = i;
		} else {
			GV(deq_idx_bak) = 0;
		}
		// LOG("E: %u, D: %u\n", GV(enq_idx), GV(deq_idx_bak));

		GV(node_idx) = 0;

		COMMIT(deq_idx);
		TRANSITION_TO(task_find_shorter_path);
	} else {
		COMMIT(deq_idx);
		TRANSITION_TO(task_done);
	}
}


void task_find_shorter_path()
{
	PRIV(node_idx);
	PRIV(enq_idx);

	uint16_t cost, node, dist, nearest_dist, i;

	node = GV(nearest_node).node;
	i = GV(node_idx_bak);
	cost = adj_matrix[node][i];

	if (cost != INFINITY) {
		nearest_dist = GV(nearest_node).dist;
		DY_PRIV(node_list, i);
		dist = GV(node_list_bak, i).dist;
		if (dist == INFINITY || dist > (cost + nearest_dist)) {
			GV(node_list_bak, i).dist = nearest_dist + cost;
			GV(node_list_bak, i).prev = node;
			DY_COMMIT(node_list, i);

			// Enqueue.
			uint16_t j = GV(enq_idx_bak);
			if (j == (GV(deq_idx) - 1)) {
				LOG("QUEUE IS FULL!");
			}
		    GV(queue, j).node = i;
		    GV(queue, j).dist = nearest_dist + cost;
		    GV(queue, j).prev = node;
			j++;
			if (j < Q_SIZE) {
				GV(enq_idx_bak) = j;
			} else {
				GV(enq_idx_bak) = 0;
			}
		    // LOG("E: %u, D: %u\n", GV(enq_idx_bak), GV(deq_idx));
		}
	}

	if (++GV(node_idx_bak) < N_NODES) {
		COMMIT(node_idx);
		COMMIT(enq_idx);
		TRANSITION_TO(task_find_shorter_path);
	} else {
		COMMIT(node_idx);
		COMMIT(enq_idx);
		TRANSITION_TO(task_select_nearest_node);
	}
}


void task_done()
{
    __no_operation();

#if ENABLE_PRF
    if (full_run_started) {
#if AUTO_RST
        msp_reseter_halt();
#endif
		// Print shortest path from src_node to dst_node
		// (print backwards to avoid recursion).
        // uint16_t dst_node = 5;
        // LOG("(%u)", dst_node);
        // __delay_cycles(1000);
        // uint16_t p = GV(node_list, dst_node).prev;
        // while (p != UNDEFINED) {
        // 	LOG(" <- (%u)", p);
        // 	__delay_cycles(1000);
        // 	p = GV(node_list, p).prev;
        // }
        // LOG("\n");

        PRF_APP_END();
        full_run_started = 0;
#if AUTO_RST
        msp_reseter_resume();
#endif
    }
#endif

    TRANSITION_TO(task_init);
}


void init()
{
    msp_watchdog_disable();
    msp_gpio_unlock();

#if ENABLE_PRF
    PRF_INIT();
    PRF_POWER();
#endif

    // msp_clock_set_mclk(CLK_8_MHZ);

#if TSK_SIZ
    uart_init();
    cp_init();
#endif

#if LOG_INFO
    uart_init();
#endif

#if AUTO_RST
    msp_reseter_auto_rand(RST_TIME);
#endif

#if ENABLE_PRF
    if (first_run) {
        PRF_APP_BGN();
        first_run = 0;
    }
#endif
}


ENTRY_TASK(task_init)
INIT_FUNC(init)
