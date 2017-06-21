#include "pTCB.h"
#include "dataProtec.h"

#ifndef PSECHED_H_
#define PSECHED_H_

// functions prototyping
void os_initTasks( const uint16_t numTasks, funcPt tasks[]);
void os_scheduler();

void os_exit_critical();
void os_enter_critical();

void os_jump(uint16_t j);

#endif
