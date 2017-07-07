#include <psched.h>
#include <msp430fr5969.h>

#define __KEY  0xAD
#define COMMITTING      1
#define COMMIT_FINISH   0

#define JUMP();    if(__jump !=1){   \
                        __current_task_virtual  =  \
                        (uint16_t) (*(__current_task_virtual + NEXT_OFFSET_PT)) ;     /* soft transition */ \
                    }else{  \
                        while(__jump_cnt < __jump_to)   \
                            {   \
                                __current_task_virtual  =  \
                                (uint16_t) (*(__current_task_virtual + NEXT_OFFSET_PT)) ;  /* soft transition */ \
                                __jump_cnt++;     \
                            }       \
                        __jump = 0;     \
                        __jump_cnt = 0;    \
                    }


__nv volatile uint8_t __locker              = 0;
__nv volatile uint8_t __commit_flag         = 0;
__nv volatile uint16_t __task_address       = 0;    // Modified externally
__nv volatile uint16_t __temp_task_address  = 0;

__nv volatile uint16_t __virtualTaskSize    = 1; // Disable the virtualization algorithm 2;
__nv volatile uint16_t __maxVirtualTaskSize = 1; // Disable the virtualization algorithm  100;
     volatile uint16_t __taskCounter        = 1; // Disable the virtualization algorithm  0;
__nv  volatile uint16_t __temp_taskCounter  = 0;
__nv volatile uint16_t __totalTaskCounter   = 0;

__nv volatile uint16_t __jump               = 0;
__nv volatile uint16_t __jump_to            = 0;
     volatile uint16_t __jump_cnt           = 0;

     volatile uint16_t __numBlockedTasks=0;
     volatile uint16_t __numUnblockedTasks=0;
__nv volatile uint16_t __temp_numBlockedTasks=0;
__nv volatile uint16_t __temp_numUnblockedTasks=0;
__nv funcPt __blockedTasks [16]={0};      // TODO at most 16 tasks can be blocked
__nv funcPt __unblockedTasks [16]={0};     // TODO at most 16 tasks can be blocked

__nv uint16_t __reboot_state[2]={0};    //virtual Task size control

//uint16_t * _current_task = NULL;
uint16_t * __current_task_virtual = NULL;

void os_enter_critical()
{
    __bis_SR_register(GIE);
}

void os_exit_critical()
{
    __bic_SR_register(GIE);
}


// These tasks will be executed only once.
void os_initTasks( const uint16_t numTasks, funcPt tasks[])
{
    if(__locker != __KEY )
    {
        uint16_t i = 0;
        do{
            if (__commit_flag == 1)
                goto init_commit;

            // execute the init tasks
            tasks[i]();

            __commit_flag=COMMITTING;
init_commit:
            __pagsCommit();
            __commit_flag=COMMIT_FINISH;
            i++;
        }while(i != numTasks);

        // Lock this function
        __locker = __KEY;
    }
}


void os_jump(uint16_t j)
{
    __jump=1;
    __jump_to=j;
}

void os_block(funcPt func)
{
    __blockedTasks[__numBlockedTasks] = func;
    __numBlockedTasks++;
}

void os_unblock(funcPt func)
{
    __unblockedTasks[__numUnblockedTasks] = func;
    __numUnblockedTasks++;
}

void os_scheduler()
{
    if (__commit_flag == COMMITTING)
    {
        goto commit;
    }

    __bringCrntPagROM();


    // Task Merging Algorithm (A)

    /*
    * OBDERVATION: Dying twice consecutively  on the same virtual task means
    * this virtual task cannot be executed with the given energy buffer.
    */
    //Died on the same task
    if(__reboot_state[0] == __task_address )
    {
        if(__reboot_state[1] != 0)
        {
            if(__virtualTaskSize > 1)
            {
                // Decrease the virtual task size
                __virtualTaskSize--;
                __maxVirtualTaskSize = __virtualTaskSize;
            }
            // reset the reboot state
            __reboot_state[1] = 0;
        }
    }else{
        // At the very beginning or dying on another task
        __reboot_state[0] = __task_address;
        __reboot_state[1] = 1;
    }

    // Recover the virtual state
    __current_task_virtual = (uint16_t *) __task_address ;

    while(1)
    {

        /*
         * Skip the persistently and softly blocked tasks
         *
         * TODO An alternative way to implement the blocking concept is to take a snapshot
         * of the blocking status on the virtual task boundaries and to undo any changes
         * on a power up.
         */
        // Skip blocked tasks
        while( ( * (__current_task_virtual+BLOCK_OFFSET_PT )) != 0  )
        {
            __current_task_virtual  =  (uint16_t) (*(__current_task_virtual + NEXT_OFFSET_PT)) ;

        }

        //TODO a better way to skip the blocked tasks might be needed
        // Maybe it is possible to use only the jump concept
        uint16_t blockIndx ;
        for (blockIndx = 0 ;__blockedTasks[blockIndx]; blockIndx++ )
        {
            __current_task_virtual  =  (uint16_t) (*(__current_task_virtual + NEXT_OFFSET_PT)) ;

        }

        // access a task
        ( (funcPt)( *__current_task_virtual ) ) ();

        //Task Merging Algorithm (B)
        __taskCounter++;

        if( (__taskCounter  >= __virtualTaskSize) )
        {
            if( (__totalTaskCounter + __taskCounter) > __totNumTask)
            {
                if(__maxVirtualTaskSize > __virtualTaskSize )
                {
                    __virtualTaskSize++;
                    // To distinguish between consecutive power interrupt
                    // and power interrupt on the same task after a complete round
                    __reboot_state[0] = 0;

                }
            }
            // virtual progress
            JUMP();
            __temp_task_address = (uint16_t) (__current_task_virtual) ;
            __temp_taskCounter = __taskCounter;
            __temp_numBlockedTasks   = __numBlockedTasks;
            __temp_numUnblockedTasks = __numUnblockedTasks;
            __commit_flag=COMMITTING;
commit:
             // firm transition
             __task_address  =  __temp_task_address;
            __totalTaskCounter += __temp_taskCounter;
            //  Commit pages to their final locations
            __pagsCommit();

            // These buffers are being cleared inside the functions
            __os_block(__blockedTasks,__temp_numBlockedTasks);
            __os_unblock(__unblockedTasks,__temp_numUnblockedTasks);

            __commit_flag=COMMIT_FINISH;

            __taskCounter=0;

        }
        else
        {
            // virtual progress
            JUMP();
        }
    }
}

