   #include <psched.h>
#include <msp430fr5969.h>
#include <mspDebugger.h>

#define DEBUG 1


#define COMMITTING      1
#define COMMIT_FINISH   0

#define JUMP2()  if(!__jump)  \
                    {   \
                            __realTask = (unsigned int *) *(__realTask + NEXT_OFFSET_PT) ;     /* soft transition */  \
                    }else{  \
                        unsigned int totJumpSize = (*(__realTask + BLOCK_OFFSET_PT) + __jump_by);   \
                            if(totJumpSize > __totNumTask)  \
                            {   \
                                int dis  = totJumpSize - __totNumTask;  \
                                while( dis  > __totNumTask) \
                                {   \
                                    dis = dis - __totNumTask;   \
                                }   \
                                dis = dis - (totJumpSize - __jump_by);  \
                                __realTask  += (dis + dis + dis) ;  \
                            }else{  \
                                __realTask  += ( __jump_by+__jump_by  + __jump_by )  ;  \
                                }   \
                            __jump = 0; \
                        }


#define JUMP();    if(!__jump){   \
        __realTask  =  \
        (unsigned int*) (*(__realTask + NEXT_OFFSET_PT)) ;     /* soft transition */ \
        }else{  \
            while(__jump_cnt < __jump_by)   \
            {   \
                __realTask  =  \
                (unsigned int*) (*(__realTask + NEXT_OFFSET_PT)) ;  /* soft transition */ \
        __jump_cnt++;     \
            }       \
            __jump = 0;     \
            __jump_cnt = 0;    \
        }

unsigned char __jump = 0;
unsigned int __jump_by = 0;
unsigned int __jump_cnt = 0;

__nv unsigned char  __commit_flag = 0;

// __virtualTaskAddr holds persistently the address of the last executed task
// Initially __virtualTaskAddr will be initialized to the fist task in the taskId array
// __virtualTaskAddr is modified externally

__nv unsigned int  __virtualTaskAddr = 0;
__nv unsigned int *__temp_virtualTaskAddr = NULL;

    unsigned int *__realTask = NULL;

// Coalescing params
__nv unsigned int __coalTskSize = 2;    // start with 2 because we will directly decrease it in the os_scheduler
__nv unsigned int __max_coalTskSize = 64;



void os_jump(unsigned int j)
{
    __jump = 1;
    __jump_by = j;
}

void os_scheduler()
{
    // Coalescing
    if(__coalTskSize > 1)
    {
        __coalTskSize--;  // on a power interrupt decrease the coalesced task size
    }


 /******************************************************
  *  Does power interrupted execution or commit stage
  ******************************************************/

    if (__commit_flag)
    {
        // we need to do a page swap to bring the correct page from temp  or  ROM
        // this page swap must not send a page, and that must not happen since the dirtyPag is 0 at this stage
        __bringPersisCrntPag(__persis_CrntPagHeader );
        __realTask = __temp_virtualTaskAddr;
        goto commit;
    }


/******************************************************
 * Recover the virtual state
 ******************************************************/
    __bringCrntPagROM();
    __realTask = (unsigned int *) __virtualTaskAddr;
    unsigned int __coalTskCntr = 0;

    while (1)
    {
        /*********************************************
         * Task execuiton and virtual progressing
         ********************************************/
        // Coalescing
while(__coalTskCntr < __coalTskSize)
        {
             ((funcPt) (*__realTask))();
            JUMP();  // virtual progressing
            __coalTskCntr++;
        }

#if DEBUG
      uart_sendHex8(__coalTskSize);
      uart_sendStr("\n\r\0");
#endif


        __coalTskCntr=0;
        if(__coalTskSize < __max_coalTskSize)
        {
        __coalTskSize++; // on a successful coalesced task execution, increase its size
        }

        /*********************************************
         * Transition stage (from virtual -> persistent)
         ********************************************/
        __temp_virtualTaskAddr = __realTask;
        __sendPagTemp(CrntPagHeader);

        unsigned int p_cnt;
        for (p_cnt=0; __pagsInTemp[p_cnt]; p_cnt++)
        {
            //move the dirty pages addresses to a persistent buffer
            __persis_pagsInTemp[p_cnt] = __pagsInTemp[p_cnt];
        }

        //Keep track of the last accessed page over a power cycle
        __persis_CrntPagHeader = CrntPagHeader;
        __commit_flag = COMMITTING;

        /******************************************************
         * firm transition and page commiting
         ******************************************************/
commit:
     __virtualTaskAddr = (unsigned int) __temp_virtualTaskAddr;

     // Commit the dirty pages
    __pagsCommit();
    __commit_flag = COMMIT_FINISH;
    }

}
