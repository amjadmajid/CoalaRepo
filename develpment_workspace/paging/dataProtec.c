
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

/*
 * Bring a page to its ROM buffer
 */
void __bringCrntPagROM()
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) (BIGEN_ROM + CrntPagTag - PAG_SIZE) );
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long) RAM_PAG );
                                            // Destination single address
    DMA1SZ = PAG_SIZE_W;                            // Block size
    DMA1CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA1CTL |= DMAEN;                         // Enable DMA0

    DMA1CTL |= DMAREQ;                      // Trigger block transfer
}


/*
 * Send a page to its temp buffer
 */
volatile uint16_t idx;
void __sendPagTemp(uint16_t pagTag)
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) RAM_PAG);
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long) (END_ROM - TOT_PAG_SIZE + pagTag - PAG_SIZE) );
                                            // Destination single address
    DMA1SZ = PAG_SIZE_W;                            // Block size
    DMA1CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA1CTL |= DMAEN;                         // Enable DMA0

    DMA1CTL |= DMAREQ;                      // Trigger block transfer

    // Keep track of the buffered pages
    __pagsInTemp[ (pagTag>>PAG_ADDR_SIZE)-1 ] = pagTag;

}


/*
 * Bring a page to its temp buffer
 */
void __bringPagTemp(uint16_t pagTag)
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) (END_ROM - TOT_PAG_SIZE + pagTag - PAG_SIZE) );
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
void __bringPagROM(uint16_t pagTag)
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) (BIGEN_ROM + pagTag - PAG_SIZE) );
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
void __sendPagROM(uint16_t pagTag)
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) (END_ROM - TOT_PAG_SIZE + pagTag - PAG_SIZE) );
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long)  (BIGEN_ROM + pagTag - PAG_SIZE) );
                                            // Destination single address
    DMA1SZ = PAG_SIZE_W;                            // Block size
    DMA1CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA1CTL |= DMAEN;                         // Enable DMA0

    DMA1CTL |= DMAREQ;                      // Trigger block transfer
}


/*
 * pageSwap:
 */
uint16_t __pageSwap(uint16_t * varAddr)
{

    uint16_t ReqPagTag;
    uint16_t ReqPagTag_dirty = (uint16_t) varAddr;


    uint16_t __temp_pagSize = BIGEN_ROM+PAG_SIZE ; // the upper limit of the first page

    // TODO we are not checking if the var is not in any page !
    while( ! (ReqPagTag_dirty <= __temp_pagSize) )  // if the var is not with the page
    {
        __temp_pagSize +=   PAG_SIZE;               // move to next page
    }

    ReqPagTag = __temp_pagSize;

    // Send the current page to its temp location
     __sendPagTemp( CrntPagTag );

     if( __pagsInTemp[ (ReqPagTag>>PAG_ADDR_SIZE)-1 ] == ReqPagTag)
     {
         // Bring from the temp buffer
         __bringPagTemp(ReqPagTag);
     }else{
        // bring from pages final locations in ROM
         __bringPagROM(ReqPagTag);
     }
     // keep track of the current page
     CrntPagTag = ReqPagTag;
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
        __sendPagTemp(CrntPagTag);
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






