#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef INCLUDE_GLOBAL_H_
#define INCLUDE_GLOBAL_H_

#define __nv  __attribute__((section(".nv_vars")))

// Special memory locaitons
#define BASE_ADDR_0         0x1980
#define BASE_ADDR_1         0x1900

#define PTERCNT         (BASE_ADDR_1    )
#define BUFSIZE         (BASE_ADDR_1 + 2)
#define ADDRHOLDER      (BASE_ADDR_1 + 4)           // 60 bytes
#define SIZEHOLDER      (BASE_ADDR_1 + 4 + 0x3C)    // 60 bytes     0x1940

#define LOCKER            (BASE_ADDR_0    )            //1 byte
#define RESETBUF          (BASE_ADDR_0 + 1)            //1 byte
#define TASK_ADDRESS      (BASE_ADDR_0 + 2)          //2 bytes
#define TASK_ADDRESS_0    (BASE_ADDR_0 + 4)          //2 bytes

#define TABLE_CNTR      (BASE_ADDR_0+6)           //2 bytes
#define LIST_HEAD       (BASE_ADDR_0+8)           // persistent linkedlist
#define BUFFER          (BASE_ADDR_0 +8+ 0x3C)      // big pointer array

// function pointer
typedef void (* funcPt)(void);

//This is a task interface. It is shared between the user and IPOS
typedef struct _taskId{
  funcPt func;
  unsigned block;
}taskId;

#define BLOCK_OFFSET        1
#define NEXT_OFFSET         2

#endif /* INCLUDE_GLOBAL_H_ */
