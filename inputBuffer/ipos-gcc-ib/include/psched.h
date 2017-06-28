#include "global.h"
#include "inputBuffer.h"
#include "pTCB.h"

#ifndef PSECHED_H_
#define PSECHED_H_

// functions prototyping
void os_initTasks( const uint16_t numTasks, funcPt tasks[]);
void os_scheduler();
void os_initOnce();

void os_exit_critical();
void os_enter_critical();

#define DVAR(type, var)   __nv type var

#define WVAR(name, value)    name =  value

#define RVAR(name)  name;\
                    protectVar(&name,sizeof(name))


#endif
