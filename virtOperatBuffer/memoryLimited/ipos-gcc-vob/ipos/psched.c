#include <psched.h>
#include <msp430fr5969.h>

#define __KEY  0xAD
#define COMMITTING      1
#define COMMIT_FINISH   0

__nv volatile uint8_t __locker              = 0;
__nv volatile uint8_t __commit_flag         = 0;
__nv volatile uint16_t __task_address       = 0;    // Modified externally

__nv volatile uint16_t __virtualTaskSize    = 2;
__nv volatile uint16_t __maxVirtualTaskSize = 100;
     volatile uint16_t __taskCounter        = 0;
__nv volatile uint16_t __totalTaskCounter   = 0;

__nv volatile uint16_t __jump               = 0;
__nv volatile uint16_t __jump_to            = 0;
     volatile uint16_t __jump_cnt           = 0;

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

            tasks[i]();   // execute the init tasks

            wb_firstPhaseCommit();
            __commit_flag=COMMITTING;
init_commit:
            wb_secondPhaseCommit();
            __commit_flag=COMMIT_FINISH;
            i++;
        }while(i != numTasks);

        __locker = __KEY;   // Lock this function
    }
}

void os_jump(uint16_t j)
{
    __jump=1;
    __jump_to=j;
}

void os_scheduler(){
    if (__commit_flag == COMMITTING)
        goto commit;

//    wb_initTable(); // initialize the hashed table

        if(__reboot_state[0] == __task_address )      //Died on the same task
        {
            if(__reboot_state[1] != 0)
            {
                if(__virtualTaskSize > 1)
                {
                    __virtualTaskSize--;               // Decrease the virtual task size
                    __maxVirtualTaskSize = __virtualTaskSize;
                }
                __reboot_state[1] = 0;                // reset the reboot state
            }
        }else{                                        // At the very beginning  or  Dying on another task
            __reboot_state[0] = __task_address;
            __reboot_state[1] = 1;
        }

     __current_task_virtual = (uint16_t *) __task_address ;

    while(1)
    {
        if( ( * (__current_task_virtual+BLOCK_OFFSET_PT )) == 0  )              // Skip block tasks
        {
            ( (funcPt)( *__current_task_virtual ) ) ();                          // access a task

            __taskCounter++;
            if( (__taskCounter  >= __virtualTaskSize) )
            {
                __task_address  =  (uint16_t) (__current_task_virtual) ;       // firm transition


                if( (__totalTaskCounter + __taskCounter) > __totNumTask)
                {
                    if(__maxVirtualTaskSize > __virtualTaskSize )
                    {
                        __virtualTaskSize++;
                        __reboot_state[0] = 0;           // To distinguish between consecutive power interrupt
                                                        // and power interrupt on the same task after a complete round
                    }
                }

                wb_firstPhaseCommit();                                  //  First stage, SRAM -> FRAM 
                __commit_flag=COMMITTING;
commit:
                __totalTaskCounter += __taskCounter;
                wb_secondPhaseCommit();                                 //  Second stage, FRAM -> FRAM
                __commit_flag=COMMIT_FINISH;
                __taskCounter=0;

            }
        }

        if(__jump !=1){
            __current_task_virtual  =  (uint16_t) (*(__current_task_virtual + NEXT_OFFSET_PT)) ;     // soft transition
        }else{
            while(__jump_cnt < __jump_to)
                {
                    __current_task_virtual  =  (uint16_t) (*(__current_task_virtual + NEXT_OFFSET_PT)) ;  // soft transition
                    __jump_cnt++;
                }
            __jump = 0;
            __jump_cnt = 0;
        }
    }
}

