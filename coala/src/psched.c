#include <psched.h>
#include <msp430fr5969.h>
#include <mspDebugger.h>

// PLEASE SELECT AN ALGORITHIN
#define NO_COALESCING 0
#define SLO_CHNG_ALGO 0
#define FST_CHNG_ALGO 1
#define FST_CHNG_TSK_AWAR_ALGO 0

/***********************************
 *    Common Debugging flags
 **********************************/
#define COAL_TSK_SIZ_SEND 1
#define NUM_PWR_INTR__MEM_DUMP 0
#define __numbPwrInt (*(uint16_t*)(0x1990))
#define __PI_overflow (*(uint16_t*)(0x1992))

// algorithm selection flags control
#if     FST_CHNG_ALGO && SLO_CHNG_ALGO || \
        FST_CHNG_ALGO && FST_CHNG_TSK_AWAR_ALGO || \
        FST_CHNG_ALGO && NO_COALESCING || \
        SLO_CHNG_ALGO && FST_CHNG_TSK_AWAR_ALGO || \
        SLO_CHNG_ALGO && NO_COALESCING || \
        FST_CHNG_TSK_AWAR_ALGO && NO_COALESCING
#error "More than one algorithm is enabled"
#endif

#if FST_CHNG_ALGO ==0  && SLO_CHNG_ALGO ==0 && FST_CHNG_TSK_AWAR_ALGO ==0  && NO_COALESCING ==0
#error "No coalescing algorithm is selected"
#endif


/***********************************
 *     System flags
 **********************************/
#define COMMITTING      1
#define COMMIT_FINISH   0

#include "jump.txt"

/***********************************
 *     Common variables
 **********************************/
unsigned char __jump = 0;
unsigned int __jump_by = 0;
unsigned int __jump_cnt = 0;

unsigned int  __coalTaskCntr = 0;

__nv unsigned char  __commit_flag = 0;
unsigned int *__realTask = NULL;
unsigned int *__riskLevel = NULL;

// __coalTskAddr holds a task address. It is modified externally
__nv unsigned int  __coalTskAddr = 0;
__nv unsigned int *__temp_coalTskAddr = NULL;
__nv unsigned int __max_coalTskSize = 0x7f;

/***********************************
 *  Algorithm specific variables
 **********************************/
#if SLO_CHNG_ALGO
__nv unsigned int __coalTskSize = 2;    // start with 2 because we will directly decrease it in the os_scheduler
#endif

#if FST_CHNG_ALGO || FST_CHNG_TSK_AWAR_ALGO

__nv unsigned int  __realTaskCntr = 10;

#endif


/***********************************
 *  Common functionality
 **********************************/
void os_jump(unsigned int j)
{
    __jump = 1;
    __jump_by = j;
}

void os_enter_critical()
{
    __bis_SR_register(GIE);
}

void os_exit_critical()
{
    __bic_SR_register(GIE);
}



#if NO_COALESCING

void os_scheduler()
{

    if (__commit_flag == COMMITTING)
    {
        __pagsCommit(); // Commit the dirty pages
        __coalTskAddr = (unsigned int) __temp_coalTskAddr;
        __commit_flag = COMMIT_FINISH;
    }

    // initialize ram pages buffer

    unsigned int __rb_cnt;
    unsigned int __ramAddr = BGN_RAM;
    for(__rb_cnt=1; __rb_cnt < RAM_BUF_LEN; __rb_cnt++)
    {
        __ramAddr += PAG_SIZE;
        ramPagsBuf[__rb_cnt].ramPagAddr = __ramAddr;
    }

    // Recover the virtual state
    __movPag( BGN_ROM , BGN_RAM);

    __realTask = (unsigned int *) __coalTskAddr;


    while (1)
    {
        // Accessing a task
        ((funcPt) (*__realTask))();

        // virtual progressing
        JUMP();

        // transition stage (from virtual -> persistent)
        __temp_coalTskAddr = __realTask;

        // moving the dirty pages to PagTempBuffer 
        unsigned int _wp_cntr;

        for(_wp_cntr = 0; (_wp_cntr < RAM_BUF_LEN)  && ramPagsBuf[_wp_cntr].crntPagHdr ; _wp_cntr++)
        {
            if(ramPagsBuf[_wp_cntr].dirtyPag)
            {
                __sendPagTemp(ramPagsBuf[_wp_cntr].ramPagAddr, ramPagsBuf[_wp_cntr].crntPagHdr );  // Send to TempPag
                ramPagsBuf[_wp_cntr].dirtyPag = 0;
            }
        }

        unsigned int _bufs_cntr;
    
        // move the addresses of the dirty pages to a persistent buffer
        //TODO additional check is needed to not exceed the buffer size
        for (_bufs_cntr=0; __pagsInTemp[_bufs_cntr]; _bufs_cntr++)
        {
            __persis_pagsInTemp[_bufs_cntr] = __pagsInTemp[_bufs_cntr];
        }

        // __persis_CrntPagHeader = CrntPagHeader; //Keep track of the last accessed page over a power cycle

        __commit_flag = COMMITTING;

        // firm transition
        __coalTskAddr = (unsigned int) __temp_coalTskAddr;

        __pagsCommit(); // Commit the dirty pages
        __commit_flag = COMMIT_FINISH;

    }

} // end of scheduler function

#endif

#if SLO_CHNG_ALGO

void os_scheduler()
{

#if NUM_PWR_INTR__MEM_DUMP
    __numbPwrInt++;
    if(__numbPwrInt >= 0xffff)
    {
        __PI_overflow++;
        __numbPwrInt=0;
    }
#endif

    // Coalescing
    if(__coalTskSize > 1)
    {
        __coalTskSize--;  // on a power interrupt decrease the coalesced task size
    }


    /******************************************************
     *  Does the power is interrupted in the commit stage
     ******************************************************/

    if (__commit_flag == COMMITTING)
    {
        __pagsCommit(); // Commit the dirty pages
        __coalTskAddr = (unsigned int) __temp_coalTskAddr;
        __commit_flag = COMMIT_FINISH;
    }

    // initialize ram pages buffer
    unsigned int __rb_cnt;
    unsigned int __ramAddr = BGN_RAM;
    for(__rb_cnt=1; __rb_cnt < RAM_BUF_LEN; __rb_cnt++)
    {
        __ramAddr += PAG_SIZE;
        ramPagsBuf[__rb_cnt].ramPagAddr = __ramAddr;
    }

    /******************************************************
     * Recover the virtual state
     ******************************************************/
    __movPag( BGN_ROM , BGN_RAM);
    __realTask = (unsigned int *) __coalTskAddr;

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

#if COAL_TSK_SIZ_SEND
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
        __temp_coalTskAddr = __realTask;

        unsigned int _wp_cntr;
        // moving the dirty pages to PagTempBuffer 
        for(_wp_cntr = 0; (_wp_cntr < RAM_BUF_LEN)  && ramPagsBuf[_wp_cntr].crntPagHdr ; _wp_cntr++)
        {
            if(ramPagsBuf[_wp_cntr].dirtyPag)
            {
                __sendPagTemp(ramPagsBuf[_wp_cntr].ramPagAddr, ramPagsBuf[_wp_cntr].crntPagHdr );  // Send to TempPag
                ramPagsBuf[_wp_cntr].dirtyPag = 0;
            }
        }


        unsigned int _bufs_cntr;
        // move the addresses of the dirty pages to a persistent buffer
        //TODO additional check is needed to not exceed the buffer size
        for (_bufs_cntr=0; __pagsInTemp[_bufs_cntr]; _bufs_cntr++)
        {
            __persis_pagsInTemp[_bufs_cntr] = __pagsInTemp[_bufs_cntr];
        }

        //Keep track of the last accessed page over a power cycle
        // __persis_CrntPagHeader = CrntPagHeader;
        __commit_flag = COMMITTING;

        /******************************************************
         * firm transition and page commiting
         ******************************************************/
        __coalTskAddr = (unsigned int) __temp_coalTskAddr;

        // Commit the dirty pages
        __pagsCommit();
        __commit_flag = COMMIT_FINISH;
    }
}  // end of scheduler function
#endif


#if FST_CHNG_ALGO


void os_scheduler()
{

#if NUM_PWR_INTR__MEM_DUMP
    __numbPwrInt++;
    if(__numbPwrInt >= 0xffff)
    {
        __PI_overflow++;
        __numbPwrInt=0;
    }
#endif

    __coalTaskCntr = (__realTaskCntr >> 1)+1 ;  // make half of the last execution history, your new virtual task size
    __realTaskCntr = 0;    // on a power reboot the __realTaskCntr must be reseted

    if (__commit_flag == COMMITTING)
    {
        __pagsCommit(); // Commit the dirty pages
        __coalTskAddr = (unsigned int) __temp_coalTskAddr;
        __commit_flag = COMMIT_FINISH;
    }

    // initialize ram pages buffer
    unsigned int __rb_cnt;
    unsigned int __ramAddr = BGN_RAM;
    for(__rb_cnt=1; __rb_cnt < RAM_BUF_LEN; __rb_cnt++)
    {
        __ramAddr += PAG_SIZE;
        ramPagsBuf[__rb_cnt].ramPagAddr = __ramAddr;
    }

    // Recover the virtual state
    __movPag( BGN_ROM , BGN_RAM);

    __realTask = (unsigned int *) __coalTskAddr;

    while (1)
    {

//        //For Testing MUST Be removed
//        __coalTaskCntr = 8;

#if COAL_TSK_SIZ_SEND
        uart_sendHex8(__coalTaskCntr);
        uart_sendStr("\n\r\0");

#endif

        for(; __coalTaskCntr > 0 ; __coalTaskCntr-- )
        {
            // Accessing a task
            ((funcPt) (*__realTask))();

            // virtual progressing
            JUMP();

            //last execution history (my next virtual task size)
            // set a maximum virtual task size (64)
            if(__realTaskCntr < __max_coalTskSize){
                __realTaskCntr++;
            }
        }
        //At this point a virtual task must be finished

        // transition stage (from virtual -> persistent)
        __temp_coalTskAddr = __realTask;

        unsigned int _wp_cntr;
        // moving the dirty pages to PagTempBuffer 
        for(_wp_cntr = 0; (_wp_cntr < RAM_BUF_LEN)  && ramPagsBuf[_wp_cntr].crntPagHdr ; _wp_cntr++)
        {
            if(ramPagsBuf[_wp_cntr].dirtyPag)
            {
                __sendPagTemp(ramPagsBuf[_wp_cntr].ramPagAddr, ramPagsBuf[_wp_cntr].crntPagHdr );  // Send to TempPag
                ramPagsBuf[_wp_cntr].dirtyPag = 0;
            }
        }

        unsigned int _bufs_cntr;
        // move the addresses of the dirty pages to a persistent buffer
        //TODO additional check is needed to not exceed the buffer size
        for (_bufs_cntr=0; __pagsInTemp[_bufs_cntr]; _bufs_cntr++)
        {
            __persis_pagsInTemp[_bufs_cntr] = __pagsInTemp[_bufs_cntr];
        }

        // __persis_CrntPagHeader = CrntPagHeader; //Keep track of the last accessed page over a power cycle

        __commit_flag = COMMITTING;
// commit:
        // firm transition
        __coalTskAddr = (unsigned int) __temp_coalTskAddr;

        // Commit the dirty pages
        __pagsCommit();
        __commit_flag = COMMIT_FINISH;


        //if you die during the commit stage, the __realTaskCntr will be zero.
        //However,  __coalTaskCntr will get a value from __realTaskCntr before the __realTaskCntr is reseted
        if( !__coalTaskCntr )
        {
            __coalTaskCntr = ((__realTaskCntr >>1)+1) ; // the new virtual task is half of the history of execution
        }

    }
}  // end of scheduler function
#endif

#if FST_CHNG_TSK_AWAR_ALGO

void os_scheduler()
{

#if NUM_PWR_INTR__MEM_DUMP
    __numbPwrInt++;
    if(__numbPwrInt >= 0xffff)
    {
        __PI_overflow++;
        __numbPwrInt=0;
    }
#endif

    __coalTaskCntr = (__realTaskCntr >> 1)+1 ;  // make half of the last execution history, your new virtual task size
    __realTaskCntr = 0;    // on a power reboot the __realTaskCntr must be reseted

    if (__commit_flag == COMMITTING)
    {
        __pagsCommit(); // Commit the dirty pages
        __coalTskAddr = (unsigned int) __temp_coalTskAddr;
        __commit_flag = COMMIT_FINISH;
    }

    // Recover the virtual state
    __movPag( BGN_ROM , BGN_RAM);
    __realTask = (unsigned int *) __coalTskAddr;
    __riskLevel =  ( (unsigned int *) (__coalTskAddr+2)) ;


    uint16_t temp_rl;
    while (1)
    {


#if COAL_TSK_SIZ_SEND
        uart_sendHex8(__coalTaskCntr);
        uart_sendStr("\n\r\0");

#endif

        while( __coalTaskCntr > 0 )
        {
            temp_rl =  ((*__riskLevel) & 0xff) ;
            // Accessing a task
            ((funcPt) (*__realTask))();

            // virtual progressing
            JUMP();

            //last execution history (my next virtual task size)
            // set a maximum virtual task size (64)
            if(__realTaskCntr < __max_coalTskSize ){
                __realTaskCntr ++;
            }

            if(temp_rl < __coalTaskCntr ){
                __coalTaskCntr -=  temp_rl;
            }else{
                __coalTaskCntr = 0;
            }

            __riskLevel++;
            __riskLevel = (unsigned int *) *__riskLevel;
            __riskLevel++;
        }
        //At this point a virtual task must be finished



        // transition stage (from virtual -> persistent)
        __temp_coalTskAddr = __realTask;

        unsigned int __rb_cnt;
        // moving the dirty pages to PagTempBuffer 
        for(_wp_cntr = 0; (_wp_cntr < RAM_BUF_LEN)  && ramPagsBuf[_wp_cntr].crntPagHdr ; _wp_cntr++)
        {
            if(ramPagsBuf[_wp_cntr].dirtyPag)
            {
                __sendPagTemp(ramPagsBuf[_wp_cntr].ramPagAddr, ramPagsBuf[_wp_cntr].crntPagHdr );  // Send to TempPag
                ramPagsBuf[_wp_cntr].dirtyPag = 0;
            }
        }

        unsigned int _bufs_cntr;
        // move the addresses of the dirty pages to a persistent buffer
        //TODO additional check is needed to not exceed the buffer size
        for (_bufs_cntr=0; __pagsInTemp[_bufs_cntr]; _bufs_cntr++)
        {
            __persis_pagsInTemp[_bufs_cntr] = __pagsInTemp[_bufs_cntr];
        }

        // __persis_CrntPagHeader = CrntPagHeader; //Keep track of the last accessed page over a power cycle

        __commit_flag = COMMITTING;
// commit:
        // firm transition
        __coalTskAddr = (unsigned int) __temp_coalTskAddr;

        // Commit the dirty pages
        __pagsCommit();
        __commit_flag = COMMIT_FINISH;


        //if you die during the commit stage, the __realTaskCntr will be zero.
        //However,  __coalTaskCntr will get a value from __realTaskCntr before the __realTaskCntr is reseted
        if( !__coalTaskCntr )
        {
            __coalTaskCntr = ((__realTaskCntr >>1)+1) ; // the new virtual task is half of the history of execution
        }

    }
}  // end of scheduler function
#endif




























