/*
 * arrayBased_main.c
 *
 *  Created on: 14 Jul 2017
 *      Author: amjad
 */

#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include <ipos.h>

/************************************
 *            constants             *
*************************************/
#define  DATA_LEN_LOC 0x1840u
#define  DATA_LEN     0x64
#define  BYTE_LEN     8u
#define  WORD_LEN     16u
#define  TABLE_LEN    16u
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

volatile uint16_t work_x;

/************************************
 *       Protected globals          *
*************************************/
__p unsigned char Comp_data[0x64];
__p unsigned char Decomp_data[0x64];
__p unsigned int  readPt;
__p unsigned int  writePt;
__p unsigned int  buf;
__p unsigned int  bufIdx;
__p unsigned int  byte_flag;


/************************************
 *      Functions Prototype          *
*************************************/
void initData();
static void burn( uint32_t iters);
static void blinkLed(uint32_t wait );
void init();
void codeTable(void);
void init_task0();
void init_task();
void task_read();
void task_write();

/************************************
 *             delay                *
*************************************/

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
    WVAR(Comp_data[0x00] , 0x0F);
    WVAR(Comp_data[0x01] , 0xC9);
    WVAR(Comp_data[0x02] , 0xE2);
    WVAR(Comp_data[0x03] , 0xAC);
    WVAR(Comp_data[0x04] , 0xDF);
    WVAR(Comp_data[0x05] , 0x0F);
    WVAR(Comp_data[0x06] , 0xC9);
    WVAR(Comp_data[0x07] , 0xE2);
    WVAR(Comp_data[0x08] , 0xAD);
    WVAR(Comp_data[0x09] , 0x7C);
    WVAR(Comp_data[0x0a] , 0x3F);
    WVAR(Comp_data[0x0b] , 0x0F);
    WVAR(Comp_data[0x0c] , 0x15);
    WVAR(Comp_data[0x0d] , 0x6B);
    WVAR(Comp_data[0x0e] , 0xE7);
    WVAR(Comp_data[0x0f] , 0xAD);
    WVAR(Comp_data[0x10] , 0x21);
    WVAR(Comp_data[0x11] , 0x76);
    WVAR(Comp_data[0x12] , 0x8C);
    WVAR(Comp_data[0x13] , 0x5D);
    WVAR(Comp_data[0x14] , 0xF0);
    WVAR(Comp_data[0x15] , 0xC8);
    WVAR(Comp_data[0x16] , 0x15);
    WVAR(Comp_data[0x17] , 0xBE);
    WVAR(Comp_data[0x18] , 0x48);
    WVAR(Comp_data[0x19] , 0x81);
    WVAR(Comp_data[0x1a] , 0x5D);
    WVAR(Comp_data[0x1b] , 0xF3);
    WVAR(Comp_data[0x1c] , 0x31);
    WVAR(Comp_data[0x1d] , 0x6D);
    WVAR(Comp_data[0x1e] , 0x83);
    WVAR(Comp_data[0x1f] , 0x20);
    WVAR(Comp_data[0x20] , 0x56);
    WVAR(Comp_data[0x21] , 0xFA);
    WVAR(Comp_data[0x22] , 0x44);
    WVAR(Comp_data[0x23] , 0x0A);
    WVAR(Comp_data[0x24] , 0xEF);
    WVAR(Comp_data[0x25] , 0x99);
    WVAR(Comp_data[0x26] , 0x06);
    WVAR(Comp_data[0x27] , 0xC7);
    WVAR(Comp_data[0x28] , 0x60);
    WVAR(Comp_data[0x29] , 0x37);
    WVAR(Comp_data[0x2a] , 0xBE);
    WVAR(Comp_data[0x2b] , 0x8A);
    WVAR(Comp_data[0x2c] , 0xC2);
    WVAR(Comp_data[0x2d] , 0x9C);
    WVAR(Comp_data[0x2e] , 0x05);
    WVAR(Comp_data[0x2f] , 0x65);
    WVAR(Comp_data[0x30] , 0xE6);
    WVAR(Comp_data[0x31] , 0x3C);
    WVAR(Comp_data[0x32] , 0x81);
    WVAR(Comp_data[0x33] , 0xAF);
    WVAR(Comp_data[0x34] , 0x9E);
    WVAR(Comp_data[0x35] , 0x1A);
    WVAR(Comp_data[0x36] , 0xD3);
    WVAR(Comp_data[0x37] , 0x5A);
    WVAR(Comp_data[0x38] , 0x68);
    WVAR(Comp_data[0x39] , 0xDA);
    WVAR(Comp_data[0x3a] , 0x6B);
    WVAR(Comp_data[0x3b] , 0x4D);
    WVAR(Comp_data[0x3c] , 0x8F);
    WVAR(Comp_data[0x3d] , 0x3C);
    WVAR(Comp_data[0x3e] , 0x6B);
    WVAR(Comp_data[0x3f] , 0xED);
    WVAR(Comp_data[0x40] , 0x37);
    WVAR(Comp_data[0x41] , 0x7A);
    WVAR(Comp_data[0x42] , 0xE5);
    WVAR(Comp_data[0x43] , 0x3D);
    WVAR(Comp_data[0x44] , 0x02);
    WVAR(Comp_data[0x45] , 0xE5);
    WVAR(Comp_data[0x46] , 0xFB);
    WVAR(Comp_data[0x47] , 0x1E);
    WVAR(Comp_data[0x48] , 0x40);
    WVAR(Comp_data[0x49] , 0xD7);
    WVAR(Comp_data[0x4a] , 0xDA);
    WVAR(Comp_data[0x4b] , 0x6B);
    WVAR(Comp_data[0x4c] , 0x4D);
    WVAR(Comp_data[0x4d] , 0x69);
    WVAR(Comp_data[0x4e] , 0xAD);
    WVAR(Comp_data[0x4f] , 0x35);
    WVAR(Comp_data[0x50] , 0xA6);
    WVAR(Comp_data[0x51] , 0xC7);
    WVAR(Comp_data[0x52] , 0x90);
    WVAR(Comp_data[0x53] , 0x4D);
    WVAR(Comp_data[0x54] , 0xAF);
    WVAR(Comp_data[0x55] , 0x9E);
    WVAR(Comp_data[0x56] , 0x1A);
    WVAR(Comp_data[0x57] , 0xD3);
    WVAR(Comp_data[0x58] , 0x5A);
    WVAR(Comp_data[0x59] , 0x6B);
    WVAR(Comp_data[0x5a] , 0x4D);
    WVAR(Comp_data[0x5b] , 0x69);
    WVAR(Comp_data[0x5c] , 0xB1);
    WVAR(Comp_data[0x5d] , 0xE7);
    WVAR(Comp_data[0x5e] , 0x8D);
    WVAR(Comp_data[0x5f] , 0x7D);
    WVAR(Comp_data[0x60] , 0xA6);
    WVAR(Comp_data[0x61] , 0xB4);
    WVAR(Comp_data[0x62] , 0xD6);
    WVAR(Comp_data[0x63] , 0x9A);

      //uncompressed test data (the beginning of the output)
      //b2 d0 80 00 05 02 b2 d0 80 00

      //:02     1840     00         8613   B4
      *( (uint16_t *)   DATA_LEN_LOC)  = DATA_LEN; // the length of the data
      //:02195000ffff00    init_flag
      // *( (uint8_t *)   0x1950)  = 0xff;

      // The decompressing lookup table in info memory

      //The format of the transmitted table
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
       *( (uint8_t *)   0x1889)  = 0x08;
      //:02188a00D50680
       *( (uint8_t *)   0x188a)  = 0xD5;
       *( (uint8_t *)   0x188b)  = 0x12;
      //:02188c00550703
       *( (uint8_t *)   0x188c)  = 0x55;
       *( (uint8_t *)   0x188d)  = 0x13;
      //:02188e000201AB
       *( (uint8_t *)   0x188e)  = 0x02;
       *( (uint8_t *)   0x188f)  = 0x01;
      //:021890004304F1
       *( (uint8_t *)   0x1890)  = 0x43;
       *( (uint8_t *)   0x1891)  = 0x01;
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
    P4DIR |= BIT0; // make Port 4 pin 0 an output
    P3DIR |=BIT5;
}

void codeTable(void){
    unsigned char i ;
    for(i = 0 ; i < TABLE_LEN ; i++){
        *(LCode+i)    = ( (*code) >> 0) & 0x0f;     // the first 4 digits (length container)
        *(SCode+i)    = ( (*code) >> 4) & 0x0f ;    // the second 4 digits (symbol container)

        code ++;
        *(BCode+i) = *code;                         // the second byte ( binary code container)
        code++;
      }
}

void init_task0()
{
    // populate the decoding table
    initData();
    codeTable();
}

void init_task()
{
    WVAR(readPt,   (unsigned int)Comp_data );       // Write
    WVAR(writePt,  (unsigned int)Decomp_data);     // Read
    WVAR(buf,0);
    WVAR(bufIdx ,16);
    WVAR(byte_flag, 1);
}

void task_read()
{
    unsigned int inbufIdx   =                   RVAR(bufIdx);
    unsigned char *inreadPt = (unsigned char *) RVAR(readPt);
    unsigned int inbuf      =                   RVAR(buf);

    while(inbufIdx >=8 )
    {
        inbuf |= *inreadPt << (inbufIdx - 8);
        inreadPt++;
        inbufIdx -=8;
    }

    WVAR( buf, inbuf ) ;
    WVAR(readPt, (unsigned int) inreadPt);
    WVAR(bufIdx,inbufIdx );

}

void task_write()
{

    unsigned int inbufIdx =                      RVAR(bufIdx);
    unsigned char *inwritePt = (unsigned char *) RVAR(writePt);
    unsigned int inbuf =                         RVAR(buf);
    unsigned char inbyte_flag =                  RVAR(byte_flag);

    while(inbufIdx < BYTE_LEN)
    {
        unsigned char i;
        for(i=0; i < TABLE_LEN; i++){
            // If there is an exact match between the new byte and a code, output a symbol
            // the extra needed zeros (to form a byte) is added to the MSBs therefore shifting is not required
            if(  ( inbuf & (~0 <<( WORD_LEN - *(LCode+i) ) ) )  == (  ( (uint16_t)*(BCode+i) ) << (WORD_LEN -*(LCode+i) ) ) )
            {
                if(inbyte_flag == 1)
                {
                    WVAR((*inwritePt) , (*(SCode+i)<<4)) ;
                    inbyte_flag = 0;
                }else{
                    inbyte_flag = 1;
                   WVAR( (*inwritePt) , | *(SCode+i) ) ;
                }

                break;
            }
        }
        inbuf <<= (*(LCode+i)) ;
        inwritePt+=inbyte_flag;
        inbufIdx += *(LCode+i);


        WVAR( buf, inbuf ) ;
        WVAR(writePt, (unsigned int ) inwritePt);
        WVAR(bufIdx,inbufIdx );
        WVAR(byte_flag, inbyte_flag);

        // if decompressing the data is not done
        if( ( inwritePt - ( (unsigned char *) Decomp_data ) ) >=  DATA_LEN) // *( (uint16_t *) DATA_LEN ) )
        {
            P3OUT |=BIT5;
            P3OUT &=~BIT5;
//            blinkLed(2500 );
            os_jump(2);  // jump the init_task0
            break;

        }else{
            os_jump(3);  // jump the init_task0, init_task
        }

    }
}


int main(int argc, char const *argv[])
{
    init();

//    funcPt init_tasks[] = {init_task0};
//    os_initTasks(1, init_tasks);

    taskId tasks[] = {  {init_task0,0},
                        {init_task,0},
                        {task_read,0},
                        {task_write,0 }};
    //This function should be called only once
    os_addTasks(4, tasks );

    os_scheduler();
    return 0;
}

