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
#define BGN_RAM         0x1F00
#define APP_MEM         (8 * 1024)  // 12 KB [Linker script might need to be adjusted]
#define RAM_BUF_SIZE    0x400  // 1024 bytes
#define PAG_SIZE        256   // 64 128 256 512 1024
#define RAM_BUF_LEN     (RAM_BUF_SIZE/PAG_SIZE)
#define NUM_PRS_PAGS    (APP_MEM/PAG_SIZE)  // 1KB
#define PAG_SIZE_W      (PAG_SIZE/2)  //1KB
#define BGN_ROM         ( (END_ROM - APP_MEM) - APP_MEM  ) // 0xBF70
#define LIST_HEAD       (BIGEN_ROM - TASKS_STRUC)           //2 bytes

// function pointer
typedef void (* funcPt)(void);
//This is a task interface. It is shared between the user and IPOS
typedef struct _taskId{
  funcPt * func;
  uint8_t id;
  uint8_t riskLevel;
}taskId;

typedef struct {
    unsigned int ramPagAddr;
    uint8_t dirtyPag;
    unsigned int crntPagHdr;
}ramPagMeta;

extern unsigned int __coalTskAddr;
extern volatile unsigned int  __totNumTask;
extern ramPagMeta ramPagsBuf[];

// extern unsigned int __persis_CrntPagHeader;
extern unsigned int __pagsInTemp[];
extern unsigned int __persis_pagsInTemp[];
// extern volatile uint8_t dirtyPag ;
extern uint32_t __temp_temp;
// extern unsigned int CrntPagHeader;  // Holds the address of the first byte of a page
extern unsigned int __coalTaskCntr;


#define BLOCK_OFFSET_PT     1
#define NEXT_OFFSET_PT      2

#define BLOCK_OFFSET        2
#define NEXT_OFFSET         4


#define PAGCMT() __coalTaskCntr=1

#endif /* INCLUDE_GLOBAL_H_ */
