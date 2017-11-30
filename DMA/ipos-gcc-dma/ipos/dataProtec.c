#include "dataProtec.h"
#include "contFlags.h"
//#include <mspDebugger.h>

     uint32_t __temp_temp=0;
     // TODO THIS ARRAY CONSUME TOO MUCH OF THE SRAM
     unsigned int __pagsInTemp[NUM_PRS_PAGS] = {0};  // This can be pretty big and consume large section of the SRAM, therefore I shift it to FRAM
__nv unsigned int __persis_pagsInTemp[NUM_PRS_PAGS] = {0};


    volatile unsigned int swapIndex = 0;
    ramPagMeta ramPagsBuf[RAM_BUF_LEN] = {{BGN_RAM , CLR_DIRTY_PAGE,  BGN_ROM }, 0};
//__nv unsigned int __prs_crntPagHdr = BGN_ROM;

__nv unsigned int __pageFaultCounter = 0;
__nv unsigned int __fullpageFaultCounter = 0;
__nv unsigned int __cntr=0;   // to enable incremental commit

void __movPag(unsigned int from, unsigned int to)
{
    // Configure DMA channel 1

//      uart_sendStr("__movPag: \0");
//      uart_sendHex16(from);
//      uart_sendStr(" \0");
//      uart_sendHex16(to);
//      uart_sendStr("\n\r \0");

    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) (from) );
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long) (to) );
                                            // Destination single address
    DMA1SZ = PAG_SIZE_W;                            // Block size
    DMA1CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA1CTL |= DMAEN;                         // Enable DMA0

    DMA1CTL |= DMAREQ;                      // Trigger block transfer
}

//uint16_t __d_from=0;
//uint16_t __d_to=0;

void __sendPagTemp(unsigned int from, unsigned int to)
{

    //    __d_from = from;
//    __d_to= to;

      // uart_sendStr("sendPagTemp: \0");
      // uart_sendHex16(__d_from);
      // uart_sendStr(" \0");
      // uart_sendHex16(__d_to);
      // uart_sendStr("\n\r \0");



    __movPag(from,(to+APP_MEM));

    unsigned int idx=0;
    while(__pagsInTemp[idx] !=0 )
    {
        if(to == __pagsInTemp[idx] )
        {
            break;
        }
        idx++;
    }
    // Keep track of the buffered pages
    __pagsInTemp[ idx ] = to;

}

/*####################################
          Pages Swap
#####################################*/

void __pageSwap(uint8_t * varAddr, uint8_t * dirtyPag, unsigned int *curtPagHdr, unsigned int ramPag  )
{
#if PAG_FAULT_CNTR
    __pageFaultCounter++;
    if(* dirtyPag)
      __fullpageFaultCounter++;
#endif

    // uart_sendStr("pageSwap: \n\r\0");

    //2// Find the requested page
    unsigned int ReqPagTag;
    unsigned int ReqPagTag_dirty = (unsigned int) varAddr;
    unsigned int __temp_pagSize = BGN_ROM+PAG_SIZE ; // the upper limit of the first page
    // TODO we are not checking if the var is not in any page !
    unsigned int idx=0;

    // Search the page in the buffer
    while( (ReqPagTag = __pagsInTemp[idx]) != 0)
    {
        if ( (ReqPagTag_dirty >= ReqPagTag) && (ReqPagTag_dirty < (ReqPagTag+PAG_SIZE) ) )
                {
                    if(*dirtyPag)
                    {
                        __sendPagTemp( ramPag, *curtPagHdr  );  // check the buffer before inserting to it
                        *dirtyPag=CLR_DIRTY_PAGE;
                    }
                    // we found the page
                    __movPag( (ReqPagTag+ APP_MEM), ramPag );

                    goto PAG_IN_TEMP;
                }
        idx++;
    }

    //TODO optimize this search (maybe with a switch statement )
    // search the page in the ROM if it is not in the buffer
    while( ! (ReqPagTag_dirty < __temp_pagSize) )  // if the var is not with the page
    {
        __temp_pagSize +=   PAG_SIZE;               // move to next page
    }

    ReqPagTag = __temp_pagSize-PAG_SIZE;

    if(*dirtyPag)
    {
        __sendPagTemp( ramPag, *curtPagHdr  );
        *dirtyPag=CLR_DIRTY_PAGE;
    }
    __movPag(ReqPagTag, ramPag); // bring a page from ROM to RAM (first page)

PAG_IN_TEMP:

    *curtPagHdr  = ReqPagTag;
      // uart_sendHex16( *curtPagHdr);
      // uart_sendStr("\n\r \0");
}

/*####################################
          Paging commit
#####################################*/

void __pagsCommit()
{
  //TODO what if the __persis_pagsInTemp is full, there will be an error!!! 
    while (__persis_pagsInTemp[__cntr])
    {
            __movPag( (__persis_pagsInTemp[__cntr]+ APP_MEM) , __persis_pagsInTemp[__cntr] );
            __pagsInTemp[__cntr] = 0; // clear the temp buffer
           __cntr++;
    }

    for(;--__cntr;)
    {
        __persis_pagsInTemp[__cntr] =0;
    }
}

/*####################################
      User interface facilitator
#####################################*/

uint8_t* __return_addr_no_check(uint8_t* var) {

  // if(var < BGN_ROM || var < BGN_ROM+APP_MEM)
  // {
  //   return var;
  // }

    volatile unsigned int  cntr=0;

    while(cntr < RAM_BUF_LEN ){

          if( ramPagsBuf[cntr].crntPagHdr == 0)
            {
              break;
            }

          if( __IS_VAR_IN_CRNT_PAG(var, ramPagsBuf[cntr]) )
            {

              return __VAR_PT_IN_RAM_PG(var, ramPagsBuf[cntr]);
            }
          cntr++;
    }
    // cntr = swapIndex; // swapIndex must be updated it before the return statement 

    swapIndex++;
    if(swapIndex >= RAM_BUF_LEN){
        swapIndex=0;
    }

    __pageSwap(var, &ramPagsBuf[swapIndex].dirtyPag, &ramPagsBuf[swapIndex].crntPagHdr, ramPagsBuf[swapIndex].ramPagAddr);

    
    return __VAR_PT_IN_RAM_PG(var, ramPagsBuf[swapIndex]);
}

uint8_t* __return_addr_wr_no_check(uint8_t* var) {

    volatile unsigned int  cntr=0;

    while(cntr < RAM_BUF_LEN ){
          if( ramPagsBuf[cntr].crntPagHdr == 0)
            {
              break;
            }

          if( __IS_VAR_IN_CRNT_PAG(var, ramPagsBuf[cntr]) )
            {
                ramPagsBuf[cntr].dirtyPag = 1;
              return __VAR_PT_IN_RAM_PG(var, ramPagsBuf[cntr]);
            }
          cntr++;
    }

    swapIndex++;
    if(swapIndex >= RAM_BUF_LEN){
        swapIndex=0;
    }

    __pageSwap(var, &ramPagsBuf[swapIndex].dirtyPag, &ramPagsBuf[swapIndex].crntPagHdr, ramPagsBuf[swapIndex].ramPagAddr);
    ramPagsBuf[swapIndex].dirtyPag = 1;


    return __VAR_PT_IN_RAM_PG(var, ramPagsBuf[swapIndex]);
}
