/*
 * persistentList.h
 *
 *  Created on: 21 Apr 2017
 *      Author: amjad
 */
#include "global.h"

#ifndef INCLUDE_PERSISTENTLIST_H_
#define INCLUDE_PERSISTENTLIST_H_


// each call will increase the pointer by 6 bytes
void os_memMapper(unsigned int *cnt, taskId _task);

void os_addTasks(unsigned char numTasks, taskId tasks[]);
uint16_t* os_search(funcPt func);

void __os_block(funcPt func[], uint16_t numTasks);
void __os_unblock(funcPt func[], uint16_t numTasks);
#endif /* INCLUDE_PERSISTENTLIST_H_ */
