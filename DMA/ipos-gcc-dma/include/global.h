#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef INCLUDE_GLOBAL_H_
#define INCLUDE_GLOBAL_H_

#define __nv  __attribute__((section(".nv_vars")))
#define __p  __attribute__((section(".p_vars")))
#define __ro_nv __attribute__((section(".ro_nv_vars")))
//#define __v  __attribute__((section(".ram_vars")))

// Special memory locaitons

#define END_ROM         0xFF70   //  using the address 0xFF7F disables DMA transfer
#define END_RAM         0x20a0
#define APP_MEM         (12 * 1024)  // 12 KB [Linker script might need to be adjusted]
#define PAG_SIZE        128   // 64 128 256 512 1024
#define NUM_PAG         (APP_MEM/PAG_SIZE)  // 1KB
#define RAM_PAG         (END_RAM - PAG_SIZE)
//#define TOT_PAG_SIZE    (PAG_SIZE * NUM_PAG)
#define PAG_SIZE_W      (PAG_SIZE/2)  //1KB
#define BIGEN_ROM       ( (END_ROM - APP_MEM) - APP_MEM  ) // 0xBF70

//#define TASKS_STRUC     0x400  // 1KB
//#define LIST_HEAD       (BIGEN_ROM - TASKS_STRUC)           //2 bytes

// function pointer
typedef void (* funcPt)(void);

extern unsigned int __task_address;
extern volatile unsigned int  __totNumTask;
extern unsigned int __persis_CrntPagHeader;
extern unsigned int __pagsInTemp[];
extern unsigned int __persis_pagsInTemp[];

//This is a task interface. It is shared between the user and IPOS
typedef struct _taskId{
  funcPt * func;
  unsigned block;
}taskId;

#define BLOCK_OFFSET_PT     1
#define NEXT_OFFSET_PT      2

#define BLOCK_OFFSET        2
#define NEXT_OFFSET         4

#endif /* INCLUDE_GLOBAL_H_ */
