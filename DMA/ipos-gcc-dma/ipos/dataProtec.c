
/*
 * dataProtec.c
 *
 *  Created on: 16 May 2017
 *      Author: Amjad
 */

#include "dataProtec.h"

/*####################################
              Paging
#####################################*/

//TODO check what happened when page size changed
uint16_t __pagsInTemp[NUM_PAG] = {0};
__nv uint16_t __persis_pagsInTemp[NUM_PAG] = {0};

uint16_t CrntPagHeader = BIGEN_ROM;
__nv uint16_t __persis_CrntPagHeader = BIGEN_ROM;
__nv uint8_t pageCommit = 0;


/*
 * Bring a page to its ROM buffer
 */
void __bringCrntPagROM()
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA0SA,(unsigned long) ( __persis_CrntPagHeader) );
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA0DA,(unsigned long) RAM_PAG );
                                            // Destination single address
    DMA0SZ = PAG_SIZE_W;                            // Block size
    DMA0CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA0CTL |= DMAEN;                         // Enable DMA0

    DMA0CTL |= DMAREQ;                      // Trigger block transfer

    CrntPagHeader = __persis_CrntPagHeader; // update the volatile current page header
}


/*
 * Send a page to its temp buffer
 */
void __sendPagTemp(uint16_t pagHeader)
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) RAM_PAG);
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long) pagHeader + TOT_PAG_SIZE );
                                            // Destination single address
    DMA1SZ = PAG_SIZE_W;                            // Block size
    DMA1CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA1CTL |= DMAEN;                         // Enable DMA0

    DMA1CTL |= DMAREQ;                      // Trigger block transfer



    // Find the page index
    uint16_t pageGuess = pagHeader - BIGEN_ROM ; // the upper limit of the first page
    uint8_t idx=0;
    uint16_t __pageSizes = PAG_SIZE;
    while( ! (pageGuess <= __pageSizes) )  // if the var is not with the page
    {
        idx++;
        __pageSizes +=   PAG_SIZE;               // move to next page
    }

    // Keep track of the buffered pages
    __pagsInTemp[ idx ] = pagHeader;
}


/*
 * Bring a page to its temp buffer
 */
void __bringPagTemp(uint16_t pagHeader)
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) (pagHeader + TOT_PAG_SIZE) );
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long)  RAM_PAG );
                                            // Destination single address
    DMA1SZ = PAG_SIZE_W;                            // Block size
    DMA1CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA1CTL |= DMAEN;                         // Enable DMA0

    DMA1CTL |= DMAREQ;                      // Trigger block transfer
}

/*
 * Bring a page to its ROM buffer
 */
void __bringPagROM(uint16_t pagHeader)
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) (pagHeader) );
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long) RAM_PAG );
                                            // Destination single address
    DMA1SZ = PAG_SIZE_W;                            // Block size
    DMA1CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA1CTL |= DMAEN;                         // Enable DMA0

    DMA1CTL |= DMAREQ;                      // Trigger block transfer
}

/*
 * Send a page to its final locaiton
 */
void __sendPagROM(uint16_t pagHeader)
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA2SA,(unsigned long) pagHeader + TOT_PAG_SIZE);
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA2DA,(unsigned long)  pagHeader );
                                            // Destination single address
    DMA2SZ = PAG_SIZE_W;                            // Block size
    DMA2CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA2CTL |= DMAEN;                         // Enable DMA0

    DMA2CTL |= DMAREQ;                      // Trigger block transfer
}


/*
 * pageSwap:
 */
//TODO this function does not
uint16_t __pageSwap(uint16_t * varAddr)
{

    //1// send the current page
    __sendPagTemp( CrntPagHeader );
    //2// Find the requested page
    uint16_t ReqPagTag;
    uint16_t ReqPagTag_dirty = (uint16_t) varAddr;
    uint16_t __temp_pagSize = BIGEN_ROM+PAG_SIZE ; // the upper limit of the first page
    // TODO we are not checking if the var is not in any page !
    uint8_t idx=0;
    while( ! (ReqPagTag_dirty <= __temp_pagSize) )  // if the var is not with the page
    {
        idx++;
        __temp_pagSize +=   PAG_SIZE;               // move to next page
    }

    ReqPagTag = __temp_pagSize-PAG_SIZE;

    //3// pull the ther requested page from the temp buffer or ROM
     if( __pagsInTemp[idx] == ReqPagTag)
     {
         // Bring from the temp buffer
         __bringPagTemp(ReqPagTag);
     }else{
        // bring from pages final locations in ROM
         __bringPagROM(ReqPagTag);
     }
     //4// keep track of the current page
     CrntPagHeader = ReqPagTag;
    return 0;
}

/*####################################
          Paging commit
#####################################*/

void __pagsCommit()
{
    // TODO what happen when the power interrupts during the
    // commit of the page from SRAM to temp Buffer
    if(!pageCommit)
    {
        // send the current page to the temp buffer
        __sendPagTemp(CrntPagHeader);
        __persis_CrntPagHeader = CrntPagHeader;  //Keep track of the last accessed page over a power cycle
        uint16_t i;
        for (i=0; i < NUM_PAG; i++)
        {
            __persis_pagsInTemp[i] = __pagsInTemp[i];
        }
        pageCommit = 1;
    }
    uint16_t cnt;

    for (cnt=0; cnt < NUM_PAG; cnt++)
    {
        // send the pages to their final locations in ROM
        if(__persis_pagsInTemp[cnt] )
        {
            __sendPagROM( __persis_pagsInTemp[cnt] );
        }
    }

    pageCommit = 0;  // enable sending the first page to the temp buffer

}






