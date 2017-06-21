#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef INCLUDE_GLOBAL_H_
#define INCLUDE_GLOBAL_H_

#define __nv  __attribute__((section(".nv_vars")))
//#define __p  __attribute__((section(".rom_vars")))
#define __v  __attribute__((section(".ram_vars")))

// Special memory locaitons
#define BASE_ADDR       0x1980
#define LIST_HEAD       (BASE_ADDR)           //2 bytes

extern volatile uint16_t  __task_address;
extern volatile uint16_t  __totNumTask;
//extern uint16_t *__head;

// function pointer
typedef void (* funcPt)(void);

//This is a task interface. It is shared between the user and IPOS
typedef struct _taskId{
  funcPt func;
  unsigned block;
}taskId;

#define BLOCK_OFFSET_PT     1
#define NEXT_OFFSET_PT      2

#define BLOCK_OFFSET        2
#define NEXT_OFFSET         4

#endif /* INCLUDE_GLOBAL_H_ */
