/*
 * dataProtec.c
 *
 *  Created on: 16 May 2017
 *      Author: Amjad
 */

#define BLOCKSIZE 0x64        // The block size of the transfered data

#include <msp430fr5969.h>
/************************************
       Re-populate RAM VAR
************************************/
void repopulate(){
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) 0xFE80);
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long) 0x2300);
                                            // Destination single address
    DMA1SZ = BLOCKSIZE;                            // Block size
    DMA1CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA1CTL |= DMAEN;                         // Enable DMA0

    DMA1CTL |= DMAREQ;                      // Trigger block transfer
}

/************************************
        First phase commit
************************************/
void wb_firstPhaseCommit(){

    P3DIR |= BIT5;
    P3OUT |= BIT5;

    // Configure DMA channel 0
    __data16_write_addr((unsigned short) &DMA0SA,(unsigned long) 0x2300);
    __data16_write_addr((unsigned short) &DMA0DA,(unsigned long) 0xFD80);
    DMA0SZ = BLOCKSIZE;
    DMA0CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3;
    DMA0CTL |= DMAEN;

    DMA0CTL |= DMAREQ;
}

/************************************
        Second phase commit
************************************/

void wb_secondPhaseCommit()
{
    // Configure DMA channel 0
    __data16_write_addr((unsigned short) &DMA0SA,(unsigned long) 0xFD80);
    __data16_write_addr((unsigned short) &DMA0DA,(unsigned long) 0xFE80);
    DMA0SZ = BLOCKSIZE;
    DMA0CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3;
    DMA0CTL |= DMAEN;

    DMA0CTL |= DMAREQ;

    P3OUT &= ~BIT5;
}
