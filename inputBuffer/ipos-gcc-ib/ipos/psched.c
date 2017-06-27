#include <psched.h>
#include <msp430fr5969.h>

#define __KEY  0xAD
#define COMMITTING   1
#define COMMIT_FINISH  0

uint16_t * _current  = NULL;

uint8_t  * _locker       = (uint8_t *)      LOCKER;          // 1 byte
uint16_t *_task_address  = (uint16_t *)     TASK_ADDRESS;          //2 bytes
uint16_t *_task_address_0  = (uint16_t *)   TASK_ADDRESS_0;          //2 bytes
uint8_t  * _resetBuffer  = (uint8_t *)      RESETBUF;

void os_enter_critical()
{
    __bis_SR_register(GIE);
}

void os_exit_critical()
{
    __bic_SR_register(GIE);
}

void os_initOnce()
{
    *_task_address  =  (uint16_t) __head;
    (*(uint16_t *)PTERCNT)  = 0x0000;  // reset the pointer counter for the read buffer
}

// These tasks will be executed only once.
// init
void os_initTasks( const uint16_t numTasks, funcPt tasks[])
{

    if(*_locker != __KEY )
    {
        uint16_t i = 0;
        do{
            tasks[i]();   // execute the init tasks
            i++;
        }while(i != numTasks);

        // initialize variables only once
        os_initOnce();
        *_locker = __KEY;
    }
}


void os_scheduler(){
     _current =  (uint16_t *) *_task_address ;               // restore the last executed task from FRAM

     if(_resetBuffer == 1)
         goto RESET_1;
     if(_resetBuffer == 2)
         goto NEXT_1;

     restoreVar();

     while(1)
    {
        if( *((_current+BLOCK_OFFSET )) == 0  )
        {
            ((funcPt)( *_current )) ();    // access a task
        }
        _resetBuffer = 1;
RESET_1:
        resetPointers();
        _current +=  NEXT_OFFSET;
        *_task_address_0  =  *( _current);
        _resetBuffer = 2;
NEXT_1:
        *_task_address = *_task_address_0;
        _resetBuffer = 0;
        _current        =  *( _current);

    }
}

