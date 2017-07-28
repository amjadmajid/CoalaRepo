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

#define END_ROM         0xFF70    //  using the address 0xFF7F disables DMA transfer
#define END_RAM         0x20a0

// 12 KB main memory size   [Linker script might need to be adjusted]
//#define NUM_PAG         (12 * 16)   //64
//#define NUM_PAG         (12 * 8)   //128
//#define NUM_PAG         (12 * 4)   //256
#define NUM_PAG           (12 * 2)   //512
//#define NUM_PAG         (12 * 1)   //1024
#define PAG_SIZE        ((0x3000)/NUM_PAG)  // 1KB

#define RAM_PAG         (END_RAM - PAG_SIZE)
#define TOT_PAG_SIZE    (PAG_SIZE * NUM_PAG)
#define PAG_SIZE_W      (PAG_SIZE/2)  //1KB
#define BIGEN_ROM       ( (END_ROM - TOT_PAG_SIZE) - TOT_PAG_SIZE  ) // 0xBF70

#define TASKS_STRUC     0x400  // 1KB
#define LIST_HEAD       (BIGEN_ROM - TASKS_STRUC)           //2 bytes

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
