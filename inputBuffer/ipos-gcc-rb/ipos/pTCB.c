/*
 *  These functions are responsible for building the persistent circular list in FRAM
 */

#include <pTCB.h>

uint16_t *__head   =   (uint16_t *)    LIST_HEAD;
#define BLOCK   1
#define UNBLOCK   0

/*
 * memMapper create a node of linkedlist in persistent memory (FRAM)
 */
void os_memMapper(unsigned int *cnt, taskId _task)
{
    *(__head +*cnt) = (uint16_t) _task.func;
    ++(*cnt);
    *(__head +*cnt) = _task.block;  // This can be splitted into two bytes. One for blocking and one of priority
    ++(*cnt);
    *(__head +*cnt) =(uint16_t) (__head +1+(*cnt)); // a pointer to the next node
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
    unsigned char i = 0;
    unsigned int cnt=0;
    while (i<numTasks)
    {
        os_memMapper(&cnt, tasks[i]);
        i++;
    }
    *(__head +(--cnt) ) =  (unsigned int)  LIST_HEAD;   // link the tail of the linkedlist with the head
}

uint16_t * os_search(funcPt func)
{
    uint16_t* __current = __head;

    do{
        if( (uint16_t *) *__current ==  func )
        {
            return __current;
        }

        __current = *(__current+NEXT_OFFSET);  // pointer arithmetic

    }while( __current != __head);

     return NULL;
}



void os_block(funcPt func)
{
    uint16_t* taskPt = os_search(func) ;
    if( taskPt )
    {
        *(taskPt+BLOCK_OFFSET)  = BLOCK;  // pointer arithmetic
    }
}

void os_unblock(funcPt func)
{
    uint16_t* taskPt = os_search(func) ;
    if( taskPt )
    {
        *(taskPt+BLOCK_OFFSET)  = UNBLOCK;  // pointer arithmetic
    }
}

