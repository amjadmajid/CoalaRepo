#include <psched.h>
#include <msp430fr5969.h>

#define __KEY  0xAD
#define COMMITTING      1
#define COMMIT_FINISH   0

__nv volatile uint16_t   _locker         = 0;
__nv volatile uint16_t   commit_flag     = 0;
__nv volatile uint16_t  _task_address    = 0;
__nv volatile uint16_t c_ref             = 1;
__nv volatile uint16_t  c                = 0 ;

uint16_t * _current_task = NULL;

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
            commit_flag=1;
init_commit:
            wb_secondPhaseCommit();
            commit_flag=0;

            i++;
        }while(i != numTasks);

        _locker = __KEY;   // Lock this function
    }
}



void os_scheduler(){
    if (commit_flag == COMMITTING)
        goto commit;
    if(_locker == __KEY){
            repopulate();   // if os_initTasks() is not called, repopulate() will not be called as well
        }else{
            _locker == __KEY;
        }
    while(1)
    {
        _current_task = (uint16_t *) _task_address ;
        c++;

        if( ( * (_current_task+BLOCK_OFFSET_PT )) == 0  )
        {
            ( (funcPt)( *_current_task ) ) ();    // access a task
            if( (c  >= c_ref) )
            {
                wb_firstPhaseCommit();              //  First stage, commit the virtual buffer to the first persistent buffer
                commit_flag=COMMITTING;
commit:
                wb_secondPhaseCommit();             //  Second stage, commit the persistent buffer to the variables final locations
                commit_flag=COMMIT_FINISH;
                c=0;
            }
        }
        _task_address  =  (uint16_t) (*(_current_task + NEXT_OFFSET_PT)) ;
    }
}

