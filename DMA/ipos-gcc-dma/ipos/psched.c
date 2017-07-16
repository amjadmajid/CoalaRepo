#include <psched.h>
#include <msp430fr5969.h>

#define __KEY  0xAD
#define COMMITTING      1
#define COMMIT_FINISH   0

#define JUMP();    if(__jump !=1){   \
                        __current_task_virtual  =  \
                        (unsigned int*) (*(__current_task_virtual + NEXT_OFFSET_PT)) ;     /* soft transition */ \
                    }else{  \
                        while(__jump_cnt < __jump_to)   \
                            {   \
                                __current_task_virtual  =  \
                                (unsigned int*) (*(__current_task_virtual + NEXT_OFFSET_PT)) ;  /* soft transition */ \
                                __jump_cnt++;     \
                            }       \
                        __jump = 0;     \
                        __jump_cnt = 0;    \
                    }


__nv volatile unsigned char __locker              = 0;
__nv volatile unsigned char __commit_flag         = 0;
__nv unsigned int __task_address       = 0;    // Modified externally
__nv unsigned int* __temp_task_address  = NULL;

__nv volatile unsigned int __virtualTaskSize    = 1; // Disable the virtualization algorithm 2;
__nv volatile unsigned int __maxVirtualTaskSize = 1; // Disable the virtualization algorithm  100;
     volatile unsigned int __taskCounter        = 1; // Disable the virtualization algorithm  0;
__nv  volatile unsigned int __temp_taskCounter  = 0;
__nv volatile unsigned int __totalTaskCounter   = 0;

__nv volatile unsigned int __jump               = 0;
__nv volatile unsigned int __jump_to            = 0;
     volatile unsigned int __jump_cnt           = 0;


__nv unsigned int __reboot_state[2]={0};    //virtual Task size control

//uint16_t * _current_task = NULL;
unsigned int * __current_task_virtual = NULL;

void os_enter_critical()
{
    __bis_SR_register(GIE);
}

void os_exit_critical()
{
    __bic_SR_register(GIE);
}


// These tasks will be executed only once.
void os_initTasks( const unsigned int numTasks, funcPt tasks[])
{
    if(__locker != __KEY )
    {
        unsigned int i = 0;
        do{
            if (__commit_flag == 1)
                goto init_commit;

            // execute the init tasks
            tasks[i]();
//            __sendPagTemp( CrntPagHeader ); This is not
            //TODO Either completely ignore or correct

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


void os_jump(unsigned int j)
{
    __jump=1;
    __jump_to=j;
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
    __current_task_virtual = (unsigned int *)__task_address ;

    while(1)
    {

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
            __temp_task_address = __current_task_virtual ;
            __temp_taskCounter = __taskCounter;
            __temp_numBlockedTasks   = __numBlockedTasks;
            __temp_numUnblockedTasks = __numUnblockedTasks;
            __sendPagTemp( CrntPagHeader );
            unsigned int i;
            for (i=0; i < NUM_PAG; i++)
            {
                __persis_pagsInTemp[i] = __pagsInTemp[i];
            }
            __persis_CrntPagHeader = CrntPagHeader;  //Keep track of the last accessed page over a power cycle
            __commit_flag=COMMITTING;
commit:
             // firm transition
             __task_address  =  (unsigned int) __temp_task_address;
            __totalTaskCounter += __temp_taskCounter;
            //  Commit pages to their final locations
            __pagsCommit();

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

