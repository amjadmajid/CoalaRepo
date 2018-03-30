/******************************************************************************
 * File: scheduler.c                                                          *
 *                                                                            *
 * Look at scheduler.h for functions documentation.                           *
 *                                                                            *
 * Note: this is an internal set of functionalities used by Coala's kernel,   *
 *       do not modify or use externally.                                     *
 *                                                                            *
 * Authors: Amjad Majid, Carlo Delle Donne                                    *
 ******************************************************************************/

#include <msp430.h>
#include <stdint.h>
#include <mspprintf.h>
#include <mspProfiler.h>
#include <mspReseter.h>

#include <scheduler.h>
#include <global.h>
#include <dataprotect.h>
#include <ctlflags.h>
#include <partialcommit.h>

#define PRF_PORT 3
#define PRF_PIN  5

#define pwr_int_count    (*(uint16_t*)(0x1990))
#define pwr_int_overflow (*(uint16_t*)(0x1992))

#define MAX_TARGET_SIZE 0x40 // Max 64 coalesced tasks


// Commit phase.
__nv uint8_t commit_flag = COMMIT_FINISH;

// Task function pointers.
task_pt real_task_addr;
task_pt next_real_task_addr;
__nv task_pt coalesced_task_addr;
__nv task_pt next_coalesced_task_addr;

// Coalescing.
__nv uint16_t target_size = 1;
__nv uint16_t total_task_count = 10; // TODO: why exactly this value?
uint16_t last_task_count;
__nv uint16_t prev_history[2];
uint8_t decrease_target_size = 1;
uint16_t coalesced_task_count = 0;

// Debugging and logging.
__nv uint16_t re_exec_penalty;

// Time weight.
uint8_t real_task_time_wgt;
uint8_t next_real_task_time_wgt;
__nv uint8_t coalesced_task_time_wgt;
__nv uint8_t next_coalesced_task_time_wgt;

// Defined in dataprotect.c
extern ram_page_metadata_t ram_pages_buf[];
extern uint16_t shadow_buf_pages_count;

// Defined in partialcommit.c
extern uint8_t checkpoint_available;
extern uint8_t death_count;


#if NO_COALESCING

void __scheduler()
{
    if (commit_flag == COMMITTING) {

        __clear_partial_commit();

        // Resume from last executing task.
        coalesced_task_addr = next_coalesced_task_addr;
        // Copy dirty pages from shadow buffer to store buffer.
        __commit_to_store_buf();
        commit_flag = COMMIT_FINISH;
    }
    shadow_buf_pages_count = 0;

    // Initialize SRAM working buffer.
    uint16_t page_idx;
    uint16_t page_addr = BGN_RAM;
    for (page_idx = 0; page_idx < RAM_BUF_LEN; page_idx++) {
        ram_pages_buf[page_idx].ram_pg_tag = page_addr;
        page_addr += PAG_SIZE;
    }

    // Recall last executing (coalesced) task.
    real_task_addr = coalesced_task_addr;

    if (checkpoint_available) {
        __restore();
    }

    // Schedule tasks.
    while (1) {
        // When a coalesced task starts, no previous checkpoint is valid any longer.
        checkpoint_available = 0;

#if PROFILE_SCHEDULER
        msp_gpio_clear(COALA_PRF_PORT, COALA_PRF_SCH_PIN);
#endif

        // Execute task.
        ((task_pt) (real_task_addr))();

#if PROFILE_SCHEDULER
        msp_gpio_set(COALA_PRF_PORT, COALA_PRF_SCH_PIN);
#endif

        // Virtual progressing.
        real_task_addr = next_real_task_addr;

        // Transition stage (virtual -> persistent).
        next_coalesced_task_addr = real_task_addr;

        // Copy dirty pages to shadow buffer.
        for (page_idx = 0; page_idx < RAM_BUF_LEN && ram_pages_buf[page_idx].fram_pg_tag; page_idx++) {
            if (ram_pages_buf[page_idx].is_dirty) {
                __copy_page_to_shadow_buf(ram_pages_buf[page_idx].ram_pg_tag, ram_pages_buf[page_idx].fram_pg_tag);
                ram_pages_buf[page_idx].is_dirty = PAGE_CLEAN;
            }
        }

        // __persis_CrntPagHeader = CrntPagHeader; // Keep track of the last accessed page over a power cycle

        commit_flag = COMMITTING;

        __clear_partial_commit();

        // Firm transition to next task.
        coalesced_task_addr = next_coalesced_task_addr;

        // Copy dirty pages from shadow buffer to store buffer.
        __commit_to_store_buf();
        commit_flag = COMMIT_FINISH;
    }
}

#else // SLOW_ALG || FAST_ALG || FAST_TASK_AWARE_ALG || VALLEY_ALG || VALLEY_TASK_AWARE_ALG || WICKED_ALG

INLINE void reboot_prologue();
INLINE void recall_persistent_state();
INLINE void initialize_sram_buffer();
INLINE void recall_coalescing_state();
INLINE void static_task_epilogue();
INLINE void save_volatile_state();
INLINE void commit_to_shadow();
INLINE void commit_to_store();
INLINE void update_target_budget();


void __scheduler()
{
#if NUM_PWR_INTR_MEM_DUMP
    pwr_int_count++;
    if (pwr_int_count == 0) {
        pwr_int_overflow++;
    }
#endif

    // Update target budget and history after a power failure.
    reboot_prologue();

    // Resume from commit phase 2 if power failed during that phase.
    if (commit_flag == COMMITTING) {
        __clear_partial_commit();
        recall_persistent_state();
        commit_to_store();
        commit_flag = COMMIT_FINISH;
    }

    // Reset FRAM shadow buffer state.
    shadow_buf_pages_count = 0;

    // Initialize SRAM working buffer.
    initialize_sram_buffer();

    // Recall last coalesced task's address (and weight).
    recall_coalescing_state();

    // Restore checkpoint if available.
    if (checkpoint_available) {
        __restore();
    }

    // Run.
    while (1) {

        // When a coalesced task starts, no previous checkpoint is valid any longer.
        checkpoint_available = 0;

        // Reset current budget.
        coalesced_task_count = 0;

        // Coalesce tasks.
        while (coalesced_task_count < target_size) {
            // Execute.
            ((task_pt) (real_task_addr))();
            // Update next task's address (and weight), current budget and history.
            static_task_epilogue();

            // Log last coalesced task count (re-execution penalty).
#if AUTO_RST
            msp_reseter_halt();
#endif
            re_exec_penalty = coalesced_task_count;
#if AUTO_RST
            msp_reseter_resume();
#endif
        }

        // Save address (and weight) of the next task from which coalesce.
        save_volatile_state();

        // Commit to FRAM shadow buffer (phase 1).
        commit_to_shadow();

        // Commit to FRAM store buffer (phase 2).
        commit_flag = COMMITTING;

        // Clear last coalesced task count (re-execution penalty null).
#if AUTO_RST
        msp_reseter_halt();
#endif
        re_exec_penalty = 0;
#if AUTO_RST
        msp_reseter_resume();
#endif

        __clear_partial_commit();
        recall_persistent_state();
        commit_to_store();
        commit_flag = COMMIT_FINISH;

        // Update target budget on a successful completion of a coalesced task.
        update_target_budget();
    }
}


void reboot_prologue()
{
    // Save last history.
#if VALLEY_ALG || VALLEY_TASK_AWARE_ALG
    last_task_count = total_task_count;
#endif

    // Update target budget and history.
#if SLOW_ALG
    if (target_size > 1) {
        target_size--;
    }
#elif !WICKED_ALG
    target_size = (total_task_count >> 1) + 1;
#else // WICKED_ALG
    uint16_t mean = (total_task_count + prev_history[0] + prev_history[1]) >> 2;
    if (total_task_count >= mean) {
        uint16_t temp_target = total_task_count >> 2;
        target_size = temp_target + temp_target + temp_target + (total_task_count & 3);
    } else {
        target_size = (total_task_count >> 1) + 1;
    }
    prev_history[1] = prev_history[0];
    prev_history[0] = total_task_count;
#endif
    total_task_count = 0;
}


void recall_persistent_state()
{
    coalesced_task_addr = next_coalesced_task_addr;
#if FAST_TASK_AWARE_ALG || VALLEY_TASK_AWARE_ALG || WICKED_ALG
    coalesced_task_time_wgt = next_coalesced_task_time_wgt;
#endif
}


void initialize_sram_buffer()
{
    uint16_t page_idx;
    uint16_t page_addr = BGN_RAM;
    for (page_idx = 0; page_idx < RAM_BUF_LEN; page_idx++) {
        ram_pages_buf[page_idx].ram_pg_tag = page_addr;
        page_addr += PAG_SIZE;
    }
}


void recall_coalescing_state()
{
    real_task_addr = coalesced_task_addr;
#if FAST_TASK_AWARE_ALG || VALLEY_TASK_AWARE_ALG || WICKED_ALG
    real_task_time_wgt = coalesced_task_time_wgt;
#endif
}


void static_task_epilogue()
{
#if FAST_TASK_AWARE_ALG || VALLEY_TASK_AWARE_ALG || WICKED_ALG
    uint16_t curr_task_time_wgt = real_task_time_wgt;
#endif

    // Virtual (volatile) progressing.
    real_task_addr = next_real_task_addr;
#if FAST_TASK_AWARE_ALG || VALLEY_TASK_AWARE_ALG || WICKED_ALG
    real_task_time_wgt = next_real_task_time_wgt;
#endif

    // Update current budget and history.
#if SLOW_ALG || FAST_ALG || VALLEY_ALG
    coalesced_task_count++;
    total_task_count++;
#else
    coalesced_task_count += curr_task_time_wgt;
    total_task_count += curr_task_time_wgt;
#endif
}


void save_volatile_state()
{
    next_coalesced_task_addr = real_task_addr;
#if FAST_TASK_AWARE_ALG || VALLEY_ALG || WICKED_ALG
    next_coalesced_task_time_wgt = real_task_time_wgt;
#endif
}


void commit_to_shadow()
{
    // Copy dirty pages to shadow buffer.
    uint16_t page_idx;
    for (page_idx = 0; page_idx < RAM_BUF_LEN && ram_pages_buf[page_idx].fram_pg_tag; page_idx++) {
        if (ram_pages_buf[page_idx].is_dirty) {
            __copy_page_to_shadow_buf(ram_pages_buf[page_idx].ram_pg_tag, ram_pages_buf[page_idx].fram_pg_tag);
            ram_pages_buf[page_idx].is_dirty = PAGE_CLEAN;
        }
    }
}


void commit_to_store()
{
    __commit_to_store_buf();
}


void update_target_budget()
{
    // Recompute target budget value.
#if SLOW_ALG
    target_size++;
#elif FAST_ALG || FAST_TASK_AWARE_ALG
    target_size = ((total_task_count >> 1) + 1);
#else // VALLEY_ALG || VALLEY_TASK_AWARE_ALG || WICKED_ALG
    if (decrease_target_size) {
        if (target_size > 1) {
            target_size = target_size >> 1;
        } else {
            decrease_target_size = 0;
            target_size = target_size << 1;
        }
    } else {
        if (target_size < last_task_count) {
            target_size = target_size << 1;
        } else {
            decrease_target_size = 1;
            target_size = target_size >> 1;
        }
    }
#endif

    // Bound target budget.
    if (target_size > MAX_TARGET_SIZE) {
        target_size = MAX_TARGET_SIZE;
    }
}

#endif
