#include <psched.h>
#include <msp430fr5969.h>


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


#define COALESCING 1
#if COALESCING
        __nv unsigned int __virtualTaskSize = 2;
             unsigned int __virtualTaskmeter = 0;
#else
        __nv unsigned int __virtualTaskSize = 1;
             unsigned int __virtualTaskmeter = 1;
#endif

        __nv unsigned char  __commit_flag = 0;

        //TODO I do not think these two variables need to be persistent
        __nv unsigned int __jump = 0;   //TODO we can eliminated and use only __jump_by
        __nv unsigned int __jump_by = 0;
             unsigned int __jump_cnt = 0;

        __nv unsigned int   __task_address = 0;    // Modified externally
        __nv unsigned int*  __temp_task_address = NULL;
             unsigned int * __current_task_virtual = NULL;

        __nv unsigned int  __numReboots_fresh = 0;
        __nv unsigned int  __numReboots_old = 0;

        __nv unsigned int __reboot_state_0 = 0;    //virtual Task size control
        __nv unsigned int __reboot_state_1 = 0;
        __nv unsigned int __temp_virtualTaskmeter = 0;
        __nv unsigned int __totalTaskCounter = 0;

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
            /**************************************
             *      Initialization
             **************************************/
            __numReboots_fresh++;

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
            if (__reboot_state_0 == __task_address)
            {
                // ignore the first power interrupt
                if (__reboot_state_1 != 1)
                {
                    // if you died more than one then decrease the virtual task size
                    if (__virtualTaskSize > 1)
                    {
                        // Decrease the virtual task size
                        __virtualTaskSize--;
                    }
                }
                // reset the reboot state on a power reboot
                __reboot_state_1 = 0;
            }
            else
            {
                // At the very beginning or dying on another task
                __reboot_state_0 = __task_address;
                __reboot_state_1 = 1;
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


                __virtualTaskmeter++;

                if ((__virtualTaskmeter >= __virtualTaskSize))  // a virtual task is finished
                {

                    /***************************************
                     *  Task Merging Algorithm part (B)
                     *  Track tasks execution
                     ***************************************/
                    if ((__totalTaskCounter + __virtualTaskmeter) > __totNumTask)
                    {
#if COALESCING
                        if(__numReboots_fresh > (__numReboots_old + 4) )
                        {
                            if(__virtualTaskSize > 1)
                            {
                                __virtualTaskSize--;
                            }

                        }else{
                             __virtualTaskSize++;
                             __numReboots_old = __numReboots_fresh;
                        }
                        __numReboots_fresh=0;
                        // To distinguish between consecutive power interrupt
                        // and power interrupt on the same task after a complete round
                        __reboot_state[0] = 0;

#endif
                        // reset __virtualTaskmeter, __temp_virtualTaskmeter and  __totalTaskCounter
                        __virtualTaskmeter = 0;
                    }

                    // virtual progress
                    JUMP();

                    /***************************************
                     *        The transition stage
                     ***************************************/
                    __temp_task_address = __current_task_virtual;
                    __temp_virtualTaskmeter = __virtualTaskmeter;
                    __virtualTaskmeter = 0;

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

                    if(__temp_virtualTaskmeter){
                        __totalTaskCounter += __temp_virtualTaskmeter;
                    }else{
                        // reset the total task counter after a successful program execution round
                        __totalTaskCounter=0;
                    }

                    /**********************************************
                     *    Commit pages to their final locations
                     **********************************************/
                    __pagsCommit();
                    //			__reboot_state[0] = 0; // KIWAN

                    __commit_flag = COMMIT_FINISH;

                }
                else
                {
                    // virtual progress
                    JUMP();
                }
            }
        }

