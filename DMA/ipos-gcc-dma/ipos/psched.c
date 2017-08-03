#include <psched.h>
#include <msp430fr5969.h>

//#define __KEY  0xAD
#define COMMITTING      1
#define COMMIT_FINISH   0

#define JUMP2()  if(!__jump)  \
                    {   \
                            __current_task_virtual = (unsigned int *) *(__current_task_virtual + NEXT_OFFSET_PT) ;     /* soft transition */  \
                    }else{  \
                        unsigned int totJumpSize = (*(__current_task_virtual + BLOCK_OFFSET_PT) + __jump_by);   \
                            if(totJumpSize > __totNumTask)  \
                            {   \
                                int dis  = totJumpSize - __totNumTask;  \
                                while( dis  > __totNumTask) \
                                {   \
                                    dis = dis - __totNumTask;   \
                                }   \
                                dis = dis - (totJumpSize - __jump_by);  \
                                __current_task_virtual  += (dis + dis + dis) ;  \
                            }else{  \
                                __current_task_virtual  += ( __jump_by+__jump_by  + __jump_by )  ;  \
                                }   \
                            __jump = 0; \
                        }

#define JUMP();    if(!__jump){   \
                        __current_task_virtual  =  \
                        (unsigned int*) (*(__current_task_virtual + NEXT_OFFSET_PT)) ;     /* soft transition */ \
                    }else{  \
                        while(__jump_cnt < __jump_by)   \
                            {   \
                                __current_task_virtual  =  \
                                (unsigned int*) (*(__current_task_virtual + NEXT_OFFSET_PT)) ;  /* soft transition */ \
                                __jump_cnt++;     \
                            }       \
                        __jump = 0;     \
                        __jump_cnt = 0;    \
                    }

//#define COALESCING 1

#if COALESCING
__nv volatile unsigned int __virtualTaskSize = 2;
__nv volatile unsigned int __maxVirtualTaskSize = 100;
     volatile unsigned int __taskCounter = 0;
#else
__nv volatile unsigned int __virtualTaskSize = 1;
__nv volatile unsigned int __maxVirtualTaskSize = 1;
     volatile unsigned int __taskCounter = 1;
#endif

__nv volatile unsigned int __temp_taskCounter = 0;
__nv volatile unsigned int __totalTaskCounter = 0;

//TODO I do not think these two variables need to be persistent
__nv volatile unsigned int __jump = 0;   //TODO we can eliminated and use only __jump_by
__nv volatile unsigned int __jump_by = 0;
     volatile unsigned int __jump_cnt = 0;

__nv uint8_t  __commit_flag = 0;
//__nv uint8_t __locker = 0;
__nv unsigned int __task_address = 0;    // Modified externally
__nv unsigned int* __temp_task_address = NULL;

__nv unsigned int __reboot_state[2] = { 0 };    //virtual Task size control

    unsigned int * __current_task_virtual = NULL;

void os_enter_critical()
{
    __bis_SR_register(GIE);
}

void os_exit_critical()
{
    __bic_SR_register(GIE);
}


//void JUMP2()  {
//    if(__jump !=1)
//    {
//            __current_task_virtual = (unsinged int *) *(__current_task_virtual + NEXT_OFFSET_PT) ;     /* soft transition */ \
//    }else{
//        unsigned int totJumpSize = (*(__current_task_virtual + BLOCK_OFFSET_PT) + __jump_by);
//            if(totJumpSize > __totNumTask)
//            {
//                int dis  = totJumpSize - __totNumTask;
//                while( dis  > __totNumTask)
//                {
//                    dis = dis - __totNumTask;
//                }
//                dis = dis - (totJumpSize - __jump_by);
//                __current_task_virtual  += (dis + dis + dis) ;
//            }else{
//                __current_task_virtual  += ( __jump_by+__jump_by  + __jump_by )  ;
//                }
//            __jump = 0;
//        }
//}


//// These tasks will be executed only once.
//void os_initTasks(const unsigned int numTasks, funcPt tasks[])
//{
//    if (__locker != __KEY)
//    {
//        unsigned int i = 0;
//        do
//        {
//            if (__commit_flag == 1)
//                goto init_commit;
//
//            // execute the init tasks
//            tasks[i]();
////            __sendPagTemp( CrntPagHeader ); This is not
//            //TODO Either completely ignore or correct
//
//            __commit_flag = COMMITTING;
//            init_commit: __pagsCommit();
//            __commit_flag = COMMIT_FINISH;
//            i++;
//        }
//        while (i != numTasks);
//
//        // Lock this function
//        __locker = __KEY;
//    }
//}

void os_jump(unsigned int j)
{
    __jump = 1;
    __jump_by = j;
}

void os_scheduler()
{
    /**************************************
     *      Initialization
     **************************************/

    if (__commit_flag == COMMITTING)
    {
        // we need to do a page swap to bring the correct page from temp  or  ROM
        // this page swap must not send a page, and that must not happen since the dirtyPag is 0 at this stage
        __bringPersisCrntPag(__persis_CrntPagHeader );
        __current_task_virtual = __temp_task_address;
        goto commit;
    }

    // Recover the virtual state
    __current_task_virtual = (unsigned int *) __task_address;
    __bringCrntPagROM();

    /***************************************
     *  Task Merging Algorithm part (A)
     ***************************************/
    /*
     * OBDERVATION: Dying twice consecutively  on the same virtual task means that
     * this virtual task cannot be executed with the given energy buffer.
     */
    //Died on the same task
    if (__reboot_state[0] == __task_address)
    {
        // ignore the first power interrupt
        // if (__reboot_state[1] != 1) KIWAN
        // { KIWAN
            // if you died more than one then decrease the virtual task size
            if (__virtualTaskSize > 1)
            {
                // Decrease the virtual task size
                __virtualTaskSize--;
//                __maxVirtualTaskSize = __virtualTaskSize;
            }
        //} KIWAN
        // reset the reboot state on a power reboot
        // __reboot_state[1] = 0; KIWAN

    }
    else
    {
        // At the very beginning or dying on another task
        __reboot_state[0] = __task_address;
        // __reboot_state[1] = 1; KIWAN
    }

    /***************************************
     *   Tasks navigating and executing
     ***************************************/
    while (1)
    {

    /***************************************
     *         Access a task
     ***************************************/
        ((funcPt) (*__current_task_virtual))();


    /***************************************
     *  Task Merging Algorithm part (B)
     *  Track tasks execution
     ***************************************/
        __taskCounter++;

        if ((__taskCounter >= __virtualTaskSize))
        {
            if ((__totalTaskCounter + __taskCounter) > __totNumTask)
            {
                if (__maxVirtualTaskSize > __virtualTaskSize)
                {
                    __virtualTaskSize++;
                    // To distinguish between consecutive power interrupt
                    // and power interrupt on the same task after a complete round
                    __reboot_state[0] = 0;

                }
            }
            // virtual progress
            JUMP();

            /***************************************
             *        The transition stage
             ***************************************/
            __temp_task_address = __current_task_virtual;
            __temp_taskCounter = __taskCounter;

            __sendPagTemp(CrntPagHeader);

            unsigned int p_cnt = 0;
            unsigned int page;
            while ((page = __pagsInTemp[p_cnt]) != 0)
            {
                // send the pages to their final locations in ROM
                __persis_pagsInTemp[p_cnt] = page;
                p_cnt++;
            }

            __persis_CrntPagHeader = CrntPagHeader; //Keep track of the last accessed page over a power cycle
            __commit_flag = COMMITTING;
commit:
            // firm transition
            __task_address = (unsigned int) __temp_task_address;
            __totalTaskCounter += __temp_taskCounter;

    /**********************************************
     *    Commit pages to their final locations
     **********************************************/
            __pagsCommit();
			__reboot_state[0] = 0; // KIWAN

            __commit_flag = COMMIT_FINISH;

            __taskCounter = 0;

        }
        else
        {
            // virtual progress
            JUMP();
        }
    }
}

