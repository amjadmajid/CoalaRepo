#include "operationalBuffer.h"
#include "pTCB.h"

#ifndef PSECHED_H_
#define PSECHED_H_

// functions prototyping
void os_initTasks( const uint16_t numTasks, funcPt tasks[]);
void os_scheduler();

void os_exit_critical();
void os_enter_critical();

#define DVAR(type, var)     __nv type var

#define WVAR(name, value)   wb_insert( &name, value)

// If the return value of wb_get is not NULL then return it else return the value from FRAM
#define RVAR(name)   ( (wb_get( &name)) ? __wb_get_val() : name )




#endif
