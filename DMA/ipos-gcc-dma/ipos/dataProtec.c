
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


uint16_t __pagsInTemp[NUM_PAG] = {0};
__nv uint16_t __persis_pagsInTemp[NUM_PAG] = {0};

// 1KB
uint16_t CrntPagTag = PAG_SIZE;
volatile uint16_t maskVar=0;
__nv uint8_t pageCommit = 0;
/*
 * __is_varInCrntPag checks if a variable is in the current page
 * return: 0 page fault
 * return: 1 variable in current page
 */
uint16_t __is_varInCrntPag(uint16_t * var)
{
    // keep only the 6 MSb
     maskVar = (uint16_t)var & PAG_TAG_MSK;
    // If a variable is in the current page then its 6 MSb equals the tag of the page
    return   (uint16_t)((maskVar & CrntPagTag) == CrntPagTag) ;
}


/*
 * pageSwap:
 */
uint16_t __pageSwap(uint16_t ReqPagTag)
{
    // Send the current page to its temp location
     __sendPagTemp( CrntPagTag );
     if( __pagsInTemp[ReqPagTag & PAG_TAG_MSK] == ReqPagTag)
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

/*
 * Send a page to its temp buffer
 */
void __sendPagTemp(uint16_t pagTag)
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) END_RAM-PAG_SIZE);
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long) (END_ROM - TOT_PAG_SIZE + pagTag - PAG_SIZE) );
                                            // Destination single address
    DMA1SZ = PAG_SIZE;                            // Block size
    DMA1CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA1CTL |= DMAEN;                         // Enable DMA0

    DMA1CTL |= DMAREQ;                      // Trigger block transfer
    // Keep track of the buffered pages
    __pagsInTemp[pagTag & PAG_TAG_MSK] = pagTag;
}

/*
 * Bring a page to its temp buffer
 */
void __bringPagTemp(uint16_t pagTag)
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) (END_ROM - TOT_PAG_SIZE + pagTag - PAG_SIZE) );
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long)  END_RAM-PAG_SIZE);
                                            // Destination single address
    DMA1SZ = PAG_SIZE;                            // Block size
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
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) (BIFEN_ROM + pagTag - PAG_SIZE) );
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long)  END_RAM-PAG_SIZE);
                                            // Destination single address
    DMA1SZ = PAG_SIZE;                            // Block size
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
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long)  (BIFEN_ROM + pagTag - PAG_SIZE) );
                                            // Destination single address
    DMA1SZ = PAG_SIZE;                            // Block size
    DMA1CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA1CTL |= DMAEN;                         // Enable DMA0

    DMA1CTL |= DMAREQ;                      // Trigger block transfer
}


/*
 * Bring a page to its ROM buffer
 */
void __bringCrntPagROM()
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) (BIFEN_ROM + CrntPagTag - PAG_SIZE) );
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long)  END_RAM-PAG_SIZE);
                                            // Destination single address
    DMA1SZ = PAG_SIZE;                            // Block size
    DMA1CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA1CTL |= DMAEN;                         // Enable DMA0

    DMA1CTL |= DMAREQ;                      // Trigger block transfer
}

/*####################################
          Paging commit
#####################################*/

void __pagsCommit()
{
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






