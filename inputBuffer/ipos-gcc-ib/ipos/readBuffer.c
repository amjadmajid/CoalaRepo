#include <readBuffer.h>
#include "global.h"
#include <string.h>

uint16_t *_ptrCnt =(uint16_t *)              PTERCNT;        //2 bytes
uint16_t *_bufSize =(uint16_t *)             BUFSIZE;        //2 bytes
uint16_t * _addrHolder =(uint16_t *)         ADDRHOLDER;     // 60 bytes
uint16_t * _sizeHolder =(uint16_t *)         SIZEHOLDER;     // 60 bytes
uint8_t * _buffer =(uint8_t *)               BUFFER;

void resetPointers()
{
    *_ptrCnt = 0; // once this is set to 0,
    *_bufSize= 0;
}

void  protectVar(uint16_t * varPt, size_t size)
{

    *(_addrHolder+(*_ptrCnt) ) =(uint16_t) varPt; // the address of the variable
    *(_sizeHolder+(*_ptrCnt) ) = (uint16_t) size; // The size of the variable
    memcpy(( _buffer + (*_bufSize) ) , varPt, size); // the value
    // The pointers will always point to the next empty spots
    (*_bufSize)+=size;
    (*_ptrCnt)++;


}

void restoreVar()
{
    // restore the read values
    uint16_t _cntr = 1;
    uint16_t _size = (*_bufSize);
    while( _cntr <= (*_ptrCnt) )
    {
        _size -= *(_sizeHolder + ( (*_ptrCnt)  - _cntr) );
        memcpy(  (uint8_t *) *(_addrHolder + ( (*_ptrCnt)  -_cntr) ),
                 (uint8_t *) *( _buffer + _size),
                              *(_sizeHolder + ( (*_ptrCnt)  -_cntr)) );

        _size -=  *(_sizeHolder + ( (*_ptrCnt)  -_cntr)) ;
        _cntr++;
    }

}
