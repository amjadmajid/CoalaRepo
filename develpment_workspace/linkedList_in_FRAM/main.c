#include <msp430.h> 
#include <stdlib.h>
#include <stdint.h>

#define __nv  __attribute__((section(".nv_vars")))

typedef void (* funcPt)(void);

typedef struct task{
  funcPt func;
  unsigned block;
}taskId;

uint16_t *__head   =   (uint16_t *)    0x1986;
#define BLOCK   1
#define UNBLOCK   0

#define BLOCK_OFFSET        2
#define PIRIORITY_OFFSET    3
#define NEXT_OFFSET         4


void t1()
{
    volatile unsigned char x1 = 0;
}

void t2()
{
    volatile unsigned char x2 = 0;
}


void t3()
{
    volatile unsigned char x3 = 0;
}


// each call will increase the pointer by 6 bytes
void memMapper(unsigned int *cnt, taskId * task)
{
    *(__head+*cnt) = (unsigned int) task ->func;
    ++(*cnt);
    *(__head+*cnt) = task->block;
    ++(*cnt);
    *(__head+*cnt) =(unsigned int) (__head+1+(*cnt)); // this trick helps me in making the scheduler nicer
    ++(*cnt);
}


void persisLst(unsigned char numTasks, taskId tasks[]){
    unsigned char i = 0;
    unsigned int cnt=0;
    while (i<numTasks)
    {
        memMapper(&cnt, &tasks[i]);
        i++;
    }
    *(__head+(--cnt) ) =  (unsigned int)  0x1986;
}


uint16_t * os_search(funcPt func)
{
    uint16_t* __current = __head;

    do{
        if( (uint16_t *) *__current ==  func )
        {
            return __current;
        }

        __current = *(__current+NEXT_OFFSET/2);  // pointer arithmetic

    }while( __current != __head);

     return NULL;
}



void os_block(funcPt func)
{
    uint16_t* taskPt = os_search(func) ;
    if( taskPt )
    {
        *(taskPt+BLOCK_OFFSET/2)  = BLOCK;  // pointer arithmetic
    }
}

void os_unblock(funcPt func)
{
    uint16_t* taskPt = os_search(func) ;
    if( taskPt )
    {
        *(taskPt+BLOCK_OFFSET/2)  = UNBLOCK;  // pointer arithmetic
    }
}

int main(void) {
    taskId tasks[] = {
                       {t1, 0},
                       {t2, 0},
                       {t3, 0}};
    persisLst(3, tasks);

    os_block(t2);
    os_unblock(t2);

	return 0;
}


