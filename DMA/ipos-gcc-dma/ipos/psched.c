#include <psched.h>
#include <msp430fr5969.h>

//#define __KEY  0xAD
#define COMMITTING      1
#define COMMIT_FINISH   0
#define VERTUTASK       10

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

#define DEBUG
//#define COALESCING 1

    unsigned char __jump = 0;
    unsigned int __jump_by = 0;
    unsigned int __jump_cnt = 0;

__nv unsigned char  __commit_flag = 0;

__nv unsigned int  __virtualTaskAddr = 0;    // Modified externally
__nv unsigned int *__temp_virtualTaskAddr = NULL;

//__nv unsigned int  __virtualTaskSize = VERTUTASK;
     unsigned int  __virtualTaskCntr = 0;
__nv unsigned int  __maxVirtualTaskSize = 0x7f;

     unsigned int *__realTask = NULL;
__nv unsigned int  __realTaskCntr = VERTUTASK;

__nv unsigned char * pers_pt= (unsigned char *)0xB000;
__nv unsigned int pers_cnt = 0;

void os_enter_critical()
{
    __bis_SR_register(GIE);
}

void os_exit_critical()
{
    __bic_SR_register(GIE);
}


void os_jump(unsigned int j)
{
    __jump = 1;
    __jump_by = j;
}

void os_scheduler()
{

# if COALESCING
    __virtualTaskCntr = (__realTaskCntr >> 1)+1 ;  // make half of the last execution history, your new virtual task size
    __realTaskCntr = 0;    // on a power reboot the __realTaskCntr must be reseted
#endif

    if (__commit_flag == COMMITTING)
    {
        // we need to do a page swap to bring the correct page from temp  or  ROM
        // this page swap must not send a page, and that must not happen since the dirtyPag is 0 at this stage
        __bringPersisCrntPag(__persis_CrntPagHeader );
        __realTask = __temp_virtualTaskAddr;
        goto commit;
    }

    // Recover the virtual state
    __bringCrntPagROM();
    __realTask = (unsigned int *) __virtualTaskAddr;


    while (1)
    {

#if COALESCING

//#if DEBUG
//        if(pers_cnt < 1024){
//            *(pers_pt+pers_cnt) = __virtualTaskCntr;  // Debugging
//            pers_cnt++;
//        }
//
//#endif
        for(; __virtualTaskCntr > 0 ; __virtualTaskCntr-- )
        {
#endif
            // Accessing a task
            ((funcPt) (*__realTask))();

            // virtual progressing
            JUMP();

# if COALESCING
            //last execution history (my next virtual task size)
            // set a maximum virtual task size (64)
            if(__realTaskCntr < __maxVirtualTaskSize){
            __realTaskCntr++;
            }
        }

#endif
        //At this point a virtual task must be finished



        // transition stage (from virtual -> persistent)
        __temp_virtualTaskAddr = __realTask;
        __sendPagTemp(CrntPagHeader);

        unsigned int p_cnt;
        for (p_cnt=0; __pagsInTemp[p_cnt]; p_cnt++)
        {
            //move the dirty pages addresses to a persistent buffer
            __persis_pagsInTemp[p_cnt] = __pagsInTemp[p_cnt];
        }

        __persis_CrntPagHeader = CrntPagHeader; //Keep track of the last accessed page over a power cycle

        __commit_flag = COMMITTING;
commit:
        // firm transition
     __virtualTaskAddr = (unsigned int) __temp_virtualTaskAddr;

     // Commit the dirty pages
    __pagsCommit();
    __commit_flag = COMMIT_FINISH;

# if COALESCING
    //if you die during the commit stage, the __realTaskCntr will be zero.
    //However,  __virtualTaskCntr will get a value from __realTaskCntr before the __realTaskCntr is reseted
    if( !__virtualTaskCntr )
    {
        __virtualTaskCntr = ((__realTaskCntr >>1)+1) ; // the new virtual task is half of the history of execution
    }
#endif

    }

}
