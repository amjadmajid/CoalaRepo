#include <psched.h>
#include <msp430fr5969.h>

#define __KEY  0xAD
#define COMMITTING      1
#define COMMIT_FINISH   0

__nv volatile uint8_t _locker          = 0;
__nv volatile uint8_t commit_flag      = 0;
__nv volatile uint16_t _task_address   = 0;    // Modified externally
__nv volatile uint16_t virtualTaskSize = 1;
__nv volatile uint16_t TaskCounter     = 0;
__nv volatile uint16_t jump            = 0;
__nv volatile uint16_t jump_to         = 0;
uint16_t volatile jump_cnt             = 0;

//uint16_t * _current_task = NULL;
uint16_t * _current_task_virtual = NULL;

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
    if(_locker != __KEY )
    {
        uint16_t i = 0;
        do{
            if (commit_flag == 1)
                goto init_commit;

            tasks[i]();   // execute the init tasks

            wb_firstPhaseCommit();
            commit_flag=COMMITTING;
init_commit:
            wb_secondPhaseCommit();
            commit_flag=COMMIT_FINISH;
            i++;
        }while(i != numTasks);

        _locker = __KEY;   // Lock this function
    }
}

void os_jump(uint16_t j)
{
    jump=1;
    jump_to=j;
}

void os_scheduler(){
    if (commit_flag == COMMITTING)
        goto commit;

    if(_locker == __KEY){
            repopulate();                                               // if os_initTasks() is not called,
                                                                       // repopulate() must not be called as well
        }else{
            _locker = __KEY;
        }

     _current_task_virtual = (uint16_t *) _task_address ;

    while(1)
    {
//        _current_task =  _current_task_virtual ;

        if( ( * (_current_task_virtual+BLOCK_OFFSET_PT )) == 0  )              // Skip block tasks
        {
            ( (funcPt)( *_current_task_virtual ) ) ();                          // access a task

            TaskCounter++;
            if( (TaskCounter  >= virtualTaskSize) )
            {
                _task_address  =  (uint16_t) (_current_task_virtual) ;       // firm transition

                wb_firstPhaseCommit();                                  //  First stage, SRAM -> FRAM 
                commit_flag=COMMITTING;
commit:
                wb_secondPhaseCommit();                                 //  Second stage, FRAM -> FRAM
                commit_flag=COMMIT_FINISH;
                TaskCounter=0;

            }
        }

        if(jump !=1){
            _current_task_virtual  =  (uint16_t) (*(_current_task_virtual + NEXT_OFFSET_PT)) ;     // soft transition 
        }else{
            while(jump_cnt < jump_to)
                {
                    _current_task_virtual  =  (uint16_t) (*(_current_task_virtual + NEXT_OFFSET_PT)) ;  // soft transition
                    jump_cnt++;
                }
            jump = 0;
        }
    }
}

