#include "global.h"

#ifndef INCLUDE_WRITEBUFFER_H_
#define INCLUDE_WRITEBUFFER_H_


/************************************
        Node of linkedlists
************************************/
typedef struct wb_node {
  struct wb_node * next;
  uint16_t * addr;
  uint16_t value;
}wb_node;


#define TABLESIZE 0x007f // My hashing function applies a masking technique
#define TEMPBUFSIZE_RAWS TABLESIZE
#define TEMPBUFSIZE_COLS 2

extern uint16_t tempPersistentBuf[TEMPBUFSIZE_RAWS][TEMPBUFSIZE_COLS];
uint16_t wb_hash(uint16_t * addr);
uint16_t __wb_get_val();
uint16_t wb_get(uint16_t * addr );
void wb_insert(uint16_t * addr, uint16_t val);
void wb_firstPhaseCommit();
void wb_secondPhaseCommit();
void wb_initTable();

#endif /* INCLUDE_WRITEBUFFER_H_ */
