/*
 * dataProtec.c
 *
 *  Created on: 16 May 2017
 *      Author: Amjad
 */


 // THIS IS ALSO DEFINED IN THE LINKER FILE
#define BLOCKSIZE 128           // The block size of the transfered data in WORDS
#define ROM_2   (0xFF7F - BLOCKSIZE - BLOCKSIZE)     //we need to duplicate the number of bytes, since the transfer is in words
#define ROM_1   (0xFF7F - BLOCKSIZE - BLOCKSIZE- BLOCKSIZE - BLOCKSIZE- 0x2)  // DMA transfer more than specified, therefore we added offset
#define RAM_VARS_ADDR  0x2000

#include <msp430fr5969.h>
/************************************
       Re-populate RAM VAR
************************************/
void repopulate(){
    // Configure DMA channel 1
    __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) ROM_2);
                                            // Source block address
    __data16_write_addr((unsigned short) &DMA1DA,(unsigned long) RAM_VARS_ADDR);
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

    // Configure DMA channel 0
    __data16_write_addr((unsigned short) &DMA0SA,(unsigned long) RAM_VARS_ADDR);
    __data16_write_addr((unsigned short) &DMA0DA,(unsigned long) ROM_1);
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
    __data16_write_addr((unsigned short) &DMA0SA,(unsigned long) ROM_1);
    __data16_write_addr((unsigned short) &DMA0DA,(unsigned long) ROM_2);
    DMA0SZ = BLOCKSIZE;
    DMA0CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3;
    DMA0CTL |= DMAEN;

    DMA0CTL |= DMAREQ;

}
