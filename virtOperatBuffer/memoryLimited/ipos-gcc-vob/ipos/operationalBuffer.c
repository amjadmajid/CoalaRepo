#include <operationalBuffer.h>
     uint16_t __wb_returned_value = 0;
__nv uint16_t _persistBufCom = 0;
     uint16_t _indexTableDynSize = 0;
     uint16_t _indexTable[TEMPBUFSIZE_RAWS] = {0};   // _indexTable is used  to speedup the commit process
     uint16_t tempVirtualBuf[TEMPBUFSIZE_RAWS][TEMPBUFSIZE_COLS] ={{0}};
__nv uint16_t tempPersistentBuf[TEMPBUFSIZE_RAWS][TEMPBUFSIZE_COLS] ={{0}};

#ifdef NOT_NEEDED
/************************************
    initialize the _indexTable
************************************/
void wb_initTable()
{
    // MAYBE NOT NEED IT
  uint16_t i ;
  for(i = 0; i < TEMPBUFSIZE_RAWS; i++ )
  {
      _indexTable[i]=0;
      if(_indexTable[i] == 0)
      {
          break;
      }
  }
}

/************************************
       search the _indexTable
************************************/
uint16_t wb_search(uint16_t * addr )
{
    uint16_t i ;
    for(i = 0; i < _indexTableDynSize; i++ )
    {
        if(_indexTable[i] == (uint16_t) addr)
        {
            return 1;
        }
    }
    return 0;

}
#endif

/************************************
          hash function
************************************/
uint16_t wb_hash(uint16_t * addr)
{
    return  ( ((uint16_t) addr) & TEMPBUFSIZE_RAWS);
}

/************************************
        get the returned value
************************************/
uint16_t __wb_get_val()
{
    return __wb_returned_value;
}


/************************************
        get function
************************************/
uint16_t wb_get(uint16_t * addr )
{
    uint16_t hashVal = wb_hash(addr);
    __wb_returned_value = tempVirtualBuf[ hashVal ][1];
    return tempVirtualBuf[ hashVal ][0] ; // either the address (True) or zero (False)
}


/************************************
        insert function
************************************/
void wb_insert(uint16_t * addr, uint16_t val)
{
    // the order in this function is very important
    // to enable checking for the value without searching
    uint16_t hashVal = wb_hash(addr);
    if ( !tempVirtualBuf[ hashVal ][0] )  // if it is not found insert it
        {
            _indexTable[_indexTableDynSize] = (uint16_t)  addr;
            _indexTableDynSize++;
        }

      tempVirtualBuf[ hashVal ][0] = (uint16_t) addr ;
      tempVirtualBuf[ hashVal ][1] = val ;
}

/************************************
        First phase commit
************************************/

void wb_firstPhaseCommit(){
   if(_indexTableDynSize == 0 )
       {
           return;  // nothing to commit
       }

  uint16_t i, hashedVal;
  for( i=0; i < _indexTableDynSize; i++)
      {
          hashedVal = wb_hash(_indexTable[i]);
          tempPersistentBuf[i][0]= tempVirtualBuf[hashedVal][0];
          tempPersistentBuf[i][1]= tempVirtualBuf[hashedVal][1];
          //DMA might speedup the process of resetting
          tempVirtualBuf[hashedVal][0] = 0 ; // Reset the cell
          _indexTable[i] = 0; // Reset the cell
        }

    _persistBufCom = _indexTableDynSize;
    _indexTableDynSize = 0;  // First committing is finished
}

/************************************
        Second phase commit
************************************/

void wb_secondPhaseCommit()
{
    uint16_t i;
  for(i = 0 ; i < _persistBufCom; i++)
  {
    *((uint16_t *) tempPersistentBuf[ i ][0] ) = tempPersistentBuf[ i ][1] ;
  }
}
