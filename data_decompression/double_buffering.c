#ifdef __COMMINT__


#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include <ipos.h>

#define DVAR(type, var)     __nv type var[2]     // this definition must be in psched.h

/************************************
 *            constants             *
*************************************/
#define WRITE_ADDR 0x6800u
#define  READ_ADDR 0x8400u
#define   DATA_LEN 0x1840u
#define   BYTE_LEN 8u
#define   WORD_LEN 16u
#define  TABLE_LEN 16u
/************************************
 *              globals             *
*************************************/
// pointer to the code table ( 32 Bytes + 1 )
uint8_t  * code    = (uint8_t  *)  0x1880;
// the symbols of the code table (16 bytes + 1)
uint8_t  * SCode   = (uint8_t  *)  0x1850;
// the binary code part of the code table (16 bytes + 1)
uint8_t  * BCode   = (uint8_t  *)  0x1860;
// the length of the need binary code 16 bytes  (ends at 1856)
uint8_t  * LCode   = (uint8_t  *)  0x1870;

void initData();

/************************************
 *             delay                *
*************************************/
volatile uint16_t work_x;
static void burn( uint32_t iters)
{
    uint32_t iter = iters;
    while(iter--)
        work_x++;
}

/************************************
 *       blink an led once          *
*************************************/
static void blinkLed(uint32_t wait )
{
    P4OUT |= BIT0; // lit the LED
    burn(wait); // wait
    P4OUT &= ~BIT0; // dim the LED
    burn(wait); // wait
}

void initData()
{

        // Compressed test data
      *( (uint8_t *)   0x8400)  = 0x0f;
      *( (uint8_t *)   0x8401)  = 0x99;
      *( (uint8_t *)   0x8402)  = 0xE2;
      *( (uint8_t *)   0x8403)  = 0xA9;
      *( (uint8_t *)   0x8404)  = 0xDF;
      *( (uint8_t *)   0x8405)  = 0x0f;
      *( (uint8_t *)   0x8406)  = 0x99;
      *( (uint8_t *)   0x8407)  = 0xe2;
      *( (uint8_t *)   0x8408)  = 0xad;
      *( (uint8_t *)   0x8409)  = 0x7c;

      //uncompressed test data (expected output)
      //b2 d0 80 00 05 02 b2 d0 80 00

      //:02     1840     00         8613   B4
      *( (uint8_t *)   DATA_LEN)  = 0x0a; // the length of the data
      *( (uint8_t *)   DATA_LEN+1)  = 0x00;
      //:02195000ffff00    init_flag
      // *( (uint8_t *)   0x1950)  = 0xff;

      // The decompressing lookup table in info memory

      //The format of the transmittered table
      //length  address  delimiter  data   CRC
      //:02     1880     00         9500   30
       *( (uint8_t *)   0x1880)  = 0x95;
       *( (uint8_t *)   0x1881)  = 0x00;
      //:02188200B50153
       *( (uint8_t *)   0x1882)  = 0xB5;
       *( (uint8_t *)   0x1883)  = 0x01;
      //:02188400E50286
       *( (uint8_t *)   0x1884)  = 0xe5;
       *( (uint8_t *)   0x1885)  = 0x02;
      //:02188600650309
       *( (uint8_t *)   0x1886)  = 0x65;
       *( (uint8_t *)   0x1887)  = 0x03;
      //:02188800C40269
       *( (uint8_t *)   0x1888)  = 0xC4;
       *( (uint8_t *)   0x1889)  = 0x02;
      //:02188a00D50680
       *( (uint8_t *)   0x188a)  = 0xD5;
       *( (uint8_t *)   0x188b)  = 0x06;
      //:02188c00550703
       *( (uint8_t *)   0x188c)  = 0x55;
       *( (uint8_t *)   0x188d)  = 0x07;
      //:02188e000201AB
       *( (uint8_t *)   0x188e)  = 0x02;
       *( (uint8_t *)   0x188f)  = 0x01;
      //:021890004304F1
       *( (uint8_t *)   0x1890)  = 0x43;
       *( (uint8_t *)   0x1891)  = 0x04;
      //:021892003305E4
       *( (uint8_t *)   0x1892)  = 0x33;
       *( (uint8_t *)   0x1893)  = 0x05;
       //:02189400F40cAF
       *( (uint8_t *)   0x1894)  = 0xf4;
       *( (uint8_t *)   0x1895)  = 0x0c;
       //:02189600140dD1
       *( (uint8_t *)   0x1896)  = 0x14;
       *( (uint8_t *)   0x1897)  = 0x0d;
      //:02189800851c54
       *( (uint8_t *)   0x1898)  = 0x85;
       *( (uint8_t *)   0x1899)  = 0x1c;
      //:02189a00A63a95
       *( (uint8_t *)   0x189a)  = 0xa6;
       *( (uint8_t *)   0x189b)  = 0x3a;
      //:02189c00763b68
       *( (uint8_t *)   0x189c)  = 0x76;
       *( (uint8_t *)   0x189d)  = 0x3b;
      //:02189e00240fEB
       *( (uint8_t *)   0x189e)  = 0x24;
       *( (uint8_t *)   0x189f)  = 0x0f;
}


void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
    __enable_interrupt();
    P4DIR |= BIT0; // make Port 4 pin 0 an output
    P3DIR |=BIT5;
}

void codeTable(void){
    uint8_t i ;
    for(i = 0 ; i < TABLE_LEN ; i++){
        *(LCode+i)    = ( (*code) >> 0) & 0x0f;     // the first 4 digits (length container)
        *(SCode+i)    = ( (*code) >> 4) & 0x0f ;    // the second 4 digits (symbol container)

        code ++;
        *(BCode+i) = *code;                         // the second byte ( binary code container)
        code++;
      }
}

__nv uint8_t commit ;
DVAR(uint8_t, *readPt);
DVAR(uint8_t, *writePt);
DVAR(uint16_t, buf);
DVAR(uint16_t, bufIdx);
DVAR(uint8_t, byte_flag);

void init_task0()
{
    // populate the decoding table
    initData();
    codeTable();

    commit = 0;
}

void init_task()
{

    WVAR(readPt[0],  (uint8_t *) READ_ADDR);       // Write
    WVAR(writePt[0], (uint8_t *) WRITE_ADDR);     // Read
    WVAR(buf[0],0);
    WVAR(bufIdx[0] ,16);
    WVAR(byte_flag[0], 1);

    os_block(init_task);
}

void task_read()
{
    if(commit == 1)
        goto commit_read;

    uint16_t inbufIdx = RVAR(bufIdx[0]);
    uint8_t *inreadPt = RVAR(readPt[0]);
    uint16_t inbuf = RVAR(buf[0]);

    while(inbufIdx >=8 )
    {
        inbuf |= *inreadPt << (inbufIdx - 8);
        inreadPt++;
        inbufIdx -=8;
    }

    WVAR( buf[1], inbuf ) ;
    WVAR(readPt[1], inreadPt);
    WVAR(bufIdx[1],inbufIdx );

    commit= 1;
commit_read:
    WVAR( buf[0], buf[1] ) ;
    WVAR(readPt[0], readPt[1]);
    WVAR(bufIdx[0], bufIdx[1]);
    commit=0;
}

void task_write()
{
    if(commit == 2)
        goto commit_write;

    uint16_t inbufIdx = RVAR(bufIdx[0]);
    uint8_t *inwritePt = RVAR(writePt[0]);
    uint16_t inbuf = RVAR(buf[0]);
    uint8_t inbyte_flag = RVAR(byte_flag[0]);

    while(inbufIdx < BYTE_LEN)
    {
        uint8_t i;
        for(i=0; i < TABLE_LEN; i++){
            // If there is an exact match between the new byte and a code, output a symbol
            // the extra needed zeros (to form a byte) is added to the MSBs therefore shifting is not required
            if(  ( inbuf & (~0 <<( WORD_LEN - *(LCode+i) ) ) )  == (  ( (uint16_t)*(BCode+i) ) << (WORD_LEN -*(LCode+i) ) ) )
            {
                if(inbyte_flag == 1)
                {
                    (*inwritePt)   = *(SCode+i)<<4 ;
                    inbyte_flag = 0;
                }else{
                    inbyte_flag = 1;
                    (*inwritePt)  |= *(SCode+i) ;
                }

                break;
            }
        }
        inbuf <<= (*(LCode+i)) ;
        inwritePt+=inbyte_flag;
        inbufIdx += *(LCode+i);


        WVAR( buf[1], inbuf ) ;
        WVAR(writePt[1], inwritePt);
        WVAR(bufIdx[1],inbufIdx );
        WVAR(byte_flag[1], inbyte_flag);

        commit= 2;
commit_write:
        WVAR( buf[0], buf[1] ) ;
        WVAR(writePt[0], writePt[1]);
        WVAR(bufIdx[0], bufIdx[1]);
        WVAR(byte_flag[0], byte_flag[1]);
        commit=0;

        // if decompressing the data is not done
        if( ( inwritePt - ( (uint8_t *) WRITE_ADDR ) ) >=  *( (uint8_t *) DATA_LEN ) )
        {
            P3OUT |=BIT5;
            P3OUT &=~BIT5;
            os_unblock(init_task);
            break;
            //blinkLed(25000 );
        }

    }
}


int main(int argc, char const *argv[])
{
    init();
    funcPt init_tasks[] = {init_task0};
    os_initTasks(1, init_tasks);
    taskId tasks[] = {  {init_task,0},
                        {task_read,0},
                        {task_write,0 }};
    //This function should be called only once
    os_addTasks(3, tasks );
    os_scheduler();
    return 0;
}

#endif
