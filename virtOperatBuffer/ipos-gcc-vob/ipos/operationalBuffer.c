#include <operationalBuffer.h>

__nv volatile uint16_t __tbl_cntr =  0;
__nv volatile uint16_t __vtbl_flag =  0;  // if this is set then the buffer is not empty

/************************************
 a table of pointers to linkedlists
************************************/
static wb_node * _table[TABLESIZE];

void wb_initTable()
{
  uint16_t i ;
  for(i = 0; i < TABLESIZE; i++ )
  {
    _table[i]=NULL;
  }
}

/************************************
          hash function
************************************/
unsigned wb_hash(uint16_t * addr)
{
    return  (uint16_t) addr & TABLESIZE;
//  return  (uint16_t) addr % TABLESIZE;
//  return 0;
}

/************************************
         wb_search function
************************************/
wb_node * wb_search(uint16_t * addr )
{
  wb_node * np = _table[wb_hash(addr) ];

  while(np != NULL)
  {
    if (np->addr == addr)
    {
      return np;
    }
    np = np->next;
  }
  return NULL;
}

/************************************
        get function
************************************/
uint16_t wb_get(uint16_t * addr )
{
    wb_node * node = wb_search( addr );
    if(node != NULL)
    {
        return node->value;
    }
    return NULL;   //TODO
}

/************************************
        insert function
************************************/
void wb_insert(uint16_t * addr, uint16_t val)
{
  __vtbl_flag = 1;
  wb_node * np;
  if( (np = wb_search(addr)) == NULL ) // Not Found
  {
    np = (wb_node *) malloc( sizeof( wb_node));
    if(np == NULL)
      {
        return;
      }
      np->addr= addr;
      np->value= val;
      uint16_t hashVal = wb_hash(addr);
      np->next = _table[ hashVal ];
    _table[hashVal] = np;
  }else{
    np->value = val;
  }
}

/************************************
        First phase commit
************************************/
// This can be a single pointer
__nv uint16_t tempPersistentBuf[TEMPBUFSIZE_RAWS][TEMPBUFSIZE_COLS] ={{0}};

void wb_firstPhaseCommit(){
   __tbl_cntr = 0;    //initialize the counter for the next committing stage
   if(__vtbl_flag == 0 )
       {
           return;  // nothing to commit
       }

  uint16_t i, j=0;
  for( i=0; i < TABLESIZE; i++)
  {
    wb_node * np = _table[i];
    wb_node * np2 =NULL;
    while(np !=NULL)
    {
      tempPersistentBuf[j][0]= (uint16_t) np->addr;
      tempPersistentBuf[j][1]= np->value;
      j++;
      np2 = np->next;
      np = NULL;
      free(np);
      np =np2;
    }
  }
  __vtbl_flag = 0;  // First committing is finished
}

/************************************
        Second phase commit
************************************/

void wb_secondPhaseCommit()
{
  while(__tbl_cntr < TEMPBUFSIZE_RAWS)
  {
      if((uint16_t *) tempPersistentBuf[ __tbl_cntr ][0]  == NULL)
          return;

    *((uint16_t *) tempPersistentBuf[ __tbl_cntr ][0] ) = tempPersistentBuf[ __tbl_cntr ][1] ;
    tempPersistentBuf[ __tbl_cntr ][0] = 0;  // reset the address cell
    __tbl_cntr++;
  }
}
