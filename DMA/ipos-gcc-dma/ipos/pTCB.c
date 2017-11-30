/*
 *  These functions are responsible for building the persistent circular list in non-volatile memory
 */

#include <pTCB.h>

unsigned int* __head   =  NULL;
#define BLOCK   1
#define UNBLOCK   0
__nv unsigned char funcBlocker = 0;

volatile unsigned int  __totNumTask =0;

/*
 * memMapper create a node of linkedlist in persistent memory (FRAM)
 */
void os_memMapper(unsigned int *cnt, taskId _task)
{
    *(__head +*cnt) = (unsigned int) _task.func;
    ++(*cnt);
    *(__head +*cnt) = _task.id;
    *(__head +*cnt) <<= 8;
    *(__head +*cnt) |= _task.riskLevel;
    ++(*cnt);
    *(__head +*cnt) =(unsigned int) (__head +1+(*cnt)); // a pointer to the next node
    ++(*cnt);
}

//void ememMapper(unsigned int *cnt, taskId* _task)
//{
//    taskId* __task = (taskId*)(__head +*cnt);
//            __task->func = _task->func;
//            __task->block = _task->block;
//
//
//           *cnt+= sizeof(taskId);
//}

/*
 * building the persistent circular linked list
 */

void os_addTasks(unsigned char numTasks, taskId tasks[]){
    __totNumTask = numTasks;
    if( funcBlocker != 0xAD)
    {
        // each task needs 6 byte of meta data.
        unsigned int __temp_head =  ((BGN_ROM - numTasks * 6) - 2);
        __head =  (unsigned int*)  __temp_head;
        unsigned char i = 0;
        unsigned int cnt=0;
        while (i<numTasks)
        {
            os_memMapper(&cnt, tasks[i]);
            i++;
        }
        *(__head +(--cnt) ) =  __temp_head;   // link the tail of the linkedlist with the head

        __coalTskAddr    =  (unsigned int) __head ;
        funcBlocker = 0xAD;
    }
}

unsigned int * os_search(funcPt func)
{
    unsigned int* __current = __head;

    do{
        // Do three lock ups before looping before executing a new loop cycle (a bit of optimizations)
        if( (funcPt) *__current ==  func )
        {
            return __current;
        }

        __current = (unsigned int*) *(__current+NEXT_OFFSET_PT);  // pointer arithmetic

        if( (funcPt) *__current ==  func )
        {
            return __current;
        }

        __current = (unsigned int*) *(__current+NEXT_OFFSET_PT);

        if( (funcPt) *__current ==  func )
        {
            return __current;
        }

    }while( __current != __head);

     return NULL;
}
