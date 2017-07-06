
/*
 * dataProtec.c
 *
 *  Created on: 16 May 2017
 *      Author: Amjad
 */
#include <msp430fr5969.h>


/*####################################
              Paging
#####################################*/

/*
 * Max available memory is 48/2 = 24
 * Page size = 1KB
 * Maximum number of supported pages are 8
 * the tags (6 MSb)
 * 0000 01 (0x0400) (1KB)
 * 0000 10 (0x0800) (2KB)
 * 0000 11 (0x0c00) (3KB)
 * 0001 00 (0x1000) (4KB)
 * 0001 01 (0x1400) (5KB)
 * 0001 10 (0x1800) (6KB)
 * 0001 11 (0x1c00) (7KB)
 * 0010 00 (0x2000) (8KB)
 */


// The location of any page is given by (BIGEN_ROM + pag_tag - PAG_SIZE)
// The TEMP location of any page is given by ( END_ROM - TOT_PAG_SIZE + pag_tag - PAG_SIZE )
#define END_ROM         0xFF7F
#define BIFEN_ROM       0x4400
#define BIGEN_RAM       0x1C00
#define PAG_SIZE        0x400  // 1KB
#define NUM_PAG         8
#define PAG_ID_MSK      0xf
#define TOT_PAG_SIZE    (PAG_SIZE * NUM_PAG)
#define PAG_TAG_MSK     0xfc00

uint16_t __pagsInTemp[NUM_PAG] = {0}
__nv uint16_t __persis_pagsInTemp[NUM_PAG] = {0}

// 1KB
volatile uint16_t CrntPagTag = 0x0400;
volatile uint16_t maskVar=0;
/*
 * __is_varInCrntPag checks if a variable is in the current page
 * return: 0 page fault
 * return: 1 variable in current page
 */
uint16_t __is_varInCrntPag(uint16_t * var)
{
    // keep only the 6 MSb
     maskVar = var & PAG_TAG_MSK;
    // If a variable is in the current page then its 6 MSb equals the tag of the page
    return   ((maskVar & CrntPagTag) == CrntPagTag) ;
}


/*
 * pageSwap:
 */
uint16_t __pageSwap(uint16_t ReqPagTag)
{
    // Send the current page to its temp location
     sendPagTemp( CrntPagTag );
     if( pagsInTemp[ReqPagTag & PAG_TAG_MSK] == ReqPagTag)
     {
         // Bring from the temp buffer
         bringPagTemp(CrntPagTag);
     }else{
        // bring from pages final locations in ROM
         bringPagROM(pagTag);
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
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) BIGEN_RAM);
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long) (END_ROM - TOT_PAG_SIZE + pagTag - PAG_SIZE) );
                                            // Destination single address
    DMA1SZ = PAGE_SIZE;                            // Block size
    DMA1CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA1CTL |= DMAEN;                         // Enable DMA0

    DMA1CTL |= DMAREQ;                      // Trigger block transfer
    // Keep track of the buffered pages
    pagsInTemp[pagTag & PAG_TAG_MSK] = pagTag;
}

/*
 * Bring a page to its temp buffer
 */
void __bringPagTemp(uint16_t pagTag)
{
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) (END_ROM - TOT_PAG_SIZE + pagTag - PAG_SIZE) );
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long)  BIGEN_RAM);
                                            // Destination single address
    DMA1SZ = PAGE_SIZE;                            // Block size
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
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long)  BIGEN_RAM);
                                            // Destination single address
    DMA1SZ = PAGE_SIZE;                            // Block size
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
    DMA1SZ = PAGE_SIZE;                            // Block size
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
        sendPagTemp(pagTag);
        uint16_t i;
        for (i=0; i < NUM_PAG; i++)
        {
            __persis_pagsInTemp[i] = __pagsInTem[i];
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

}






