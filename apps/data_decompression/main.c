#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include <ipos.h>

/************************************
 *            constants             *
*************************************/
#define WRITE_ADDR 0x7000u
#define  READ_ADDR 0x8400
#define   DATA_LEN 255
#define   DATA_LEN_LOC 0x1840u
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

volatile uint16_t work_x;

/************************************
 *       Protected globals          *
*************************************/
//__p unsigned char Comp_data[0x64];
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

    *((uint8_t *) 0x8400) = 0x0F;
    *((uint8_t *) 0x8401) = 0xC9;
    *((uint8_t *) 0x8402) = 0xE2;
    *((uint8_t *) 0x8403) = 0xAC;
    *((uint8_t *) 0x8404) = 0xDF;
    *((uint8_t *) 0x8405) = 0x0F;
    *((uint8_t *) 0x8406) = 0xC9;
    *((uint8_t *) 0x8407) = 0xE2;
    *((uint8_t *) 0x8408) = 0xAD;
    *((uint8_t *) 0x8409) = 0x7C;
    *((uint8_t *) 0x840a) = 0x3F;
    *((uint8_t *) 0x840b) = 0x0F;
    *((uint8_t *) 0x840c) = 0x15;
    *((uint8_t *) 0x840d) = 0x6B;
    *((uint8_t *) 0x840e) = 0xE7;
    *((uint8_t *) 0x840f) = 0xAD;
    *((uint8_t *) 0x8410) = 0x21;
    *((uint8_t *) 0x8411) = 0x76;
    *((uint8_t *) 0x8412) = 0x8C;
    *((uint8_t *) 0x8413) = 0x5D;
    *((uint8_t *) 0x8414) = 0xF0;
    *((uint8_t *) 0x8415) = 0xC8;
    *((uint8_t *) 0x8416) = 0x15;
    *((uint8_t *) 0x8417) = 0xBE;
    *((uint8_t *) 0x8418) = 0x48;
    *((uint8_t *) 0x8419) = 0x81;
    *((uint8_t *) 0x841a) = 0x5D;
    *((uint8_t *) 0x841b) = 0xF3;
    *((uint8_t *) 0x841c) = 0x31;
    *((uint8_t *) 0x841d) = 0x6D;
    *((uint8_t *) 0x841e) = 0x83;
    *((uint8_t *) 0x841f) = 0x20;
    *((uint8_t *) 0x8420) = 0x56;
    *((uint8_t *) 0x8421) = 0xFA;
    *((uint8_t *) 0x8422) = 0x44;
    *((uint8_t *) 0x8423) = 0x0A;
    *((uint8_t *) 0x8424) = 0xEF;
    *((uint8_t *) 0x8425) = 0x99;
    *((uint8_t *) 0x8426) = 0x06;
    *((uint8_t *) 0x8427) = 0xC7;
    *((uint8_t *) 0x8428) = 0x60;
    *((uint8_t *) 0x8429) = 0x37;
    *((uint8_t *) 0x842a) = 0xBE;
    *((uint8_t *) 0x842b) = 0x8A;
    *((uint8_t *) 0x842c) = 0xC2;
    *((uint8_t *) 0x842d) = 0x9C;
    *((uint8_t *) 0x842e) = 0x05;
    *((uint8_t *) 0x842f) = 0x65;
    *((uint8_t *) 0x8430) = 0xE6;
    *((uint8_t *) 0x8431) = 0x3C;
    *((uint8_t *) 0x8432) = 0x81;
    *((uint8_t *) 0x8433) = 0xAF;
    *((uint8_t *) 0x8434) = 0x9E;
    *((uint8_t *) 0x8435) = 0x1A;
    *((uint8_t *) 0x8436) = 0xD3;
    *((uint8_t *) 0x8437) = 0x5A;
    *((uint8_t *) 0x8438) = 0x68;
    *((uint8_t *) 0x8439) = 0xDA;
    *((uint8_t *) 0x843a) = 0x6B;
    *((uint8_t *) 0x843b) = 0x4D;
    *((uint8_t *) 0x843c) = 0x8F;
    *((uint8_t *) 0x843d) = 0x3C;
    *((uint8_t *) 0x843e) = 0x6B;
    *((uint8_t *) 0x843f) = 0xED;
    *((uint8_t *) 0x8440) = 0x37;
    *((uint8_t *) 0x8441) = 0x7A;
    *((uint8_t *) 0x8442) = 0xE5;
    *((uint8_t *) 0x8443) = 0x3D;
    *((uint8_t *) 0x8444) = 0x02;
    *((uint8_t *) 0x8445) = 0xE5;
    *((uint8_t *) 0x8446) = 0xFB;
    *((uint8_t *) 0x8447) = 0x1E;
    *((uint8_t *) 0x8448) = 0x40;
    *((uint8_t *) 0x8449) = 0xD7;
    *((uint8_t *) 0x844a) = 0xDA;
    *((uint8_t *) 0x844b) = 0x6B;
    *((uint8_t *) 0x844c) = 0x4D;
    *((uint8_t *) 0x844d) = 0x69;
    *((uint8_t *) 0x844e) = 0xAD;
    *((uint8_t *) 0x844f) = 0x35;
    *((uint8_t *) 0x8450) = 0xA6;
    *((uint8_t *) 0x8451) = 0xC7;
    *((uint8_t *) 0x8452) = 0x90;
    *((uint8_t *) 0x8453) = 0x4D;
    *((uint8_t *) 0x8454) = 0xAF;
    *((uint8_t *) 0x8455) = 0x9E;
    *((uint8_t *) 0x8456) = 0x1A;
    *((uint8_t *) 0x8457) = 0xD3;
    *((uint8_t *) 0x8458) = 0x5A;
    *((uint8_t *) 0x8459) = 0x6B;
    *((uint8_t *) 0x845a) = 0x4D;
    *((uint8_t *) 0x845b) = 0x69;
    *((uint8_t *) 0x845c) = 0xB1;
    *((uint8_t *) 0x845d) = 0xE7;
    *((uint8_t *) 0x845e) = 0x8D;
    *((uint8_t *) 0x845f) = 0x7D;
    *((uint8_t *) 0x8460) = 0xA6;
    *((uint8_t *) 0x8461) = 0xB4;
    *((uint8_t *) 0x8462) = 0xD6;
    *((uint8_t *) 0x8463) = 0x9A;
        *((uint8_t *) 0x8464) = 0xD3;
       *((uint8_t *) 0x8465) = 0x5A;
       *((uint8_t *) 0x8466) = 0x6C;
       *((uint8_t *) 0x8467) = 0x79;
       *((uint8_t *) 0x8468) = 0x03;
       *((uint8_t *) 0x8469) = 0x5F;
       *((uint8_t *) 0x846a) = 0x69;
       *((uint8_t *) 0x846b) = 0xBD;
       *((uint8_t *) 0x846c) = 0x34;
       *((uint8_t *) 0x846d) = 0xD6;
       *((uint8_t *) 0x846e) = 0x9A;
       *((uint8_t *) 0x846f) = 0xD3;
       *((uint8_t *) 0x8470) = 0x5A;
       *((uint8_t *) 0x8471) = 0x6C;
       *((uint8_t *) 0x8472) = 0x79;
       *((uint8_t *) 0x8473) = 0xE3;
       *((uint8_t *) 0x8474) = 0x5F;
       *((uint8_t *) 0x8475) = 0x69;
       *((uint8_t *) 0x8476) = 0xAD;
       *((uint8_t *) 0x8477) = 0x35;
       *((uint8_t *) 0x8478) = 0xA6;
       *((uint8_t *) 0x8479) = 0xB4;
       *((uint8_t *) 0x847a) = 0xD6;
       *((uint8_t *) 0x847b) = 0x9B;
       *((uint8_t *) 0x847c) = 0x1E;
       *((uint8_t *) 0x847d) = 0x78;
       *((uint8_t *) 0x847e) = 0xD7;
       *((uint8_t *) 0x847f) = 0xDA;
       *((uint8_t *) 0x8480) = 0x6B;
       *((uint8_t *) 0x8481) = 0x4D;
       *((uint8_t *) 0x8482) = 0x69;
       *((uint8_t *) 0x8483) = 0xA0;
       *((uint8_t *) 0x8484) = 0x69;
       *((uint8_t *) 0x8485) = 0xAD;
       *((uint8_t *) 0x8486) = 0x36;
       *((uint8_t *) 0x8487) = 0x3C;
       *((uint8_t *) 0x8488) = 0x81;
       *((uint8_t *) 0x8489) = 0xAF;
       *((uint8_t *) 0x848a) = 0xB4;
       *((uint8_t *) 0x848b) = 0xD6;
       *((uint8_t *) 0x848c) = 0x9A;
       *((uint8_t *) 0x848d) = 0xD3;
       *((uint8_t *) 0x848e) = 0x5A;
       *((uint8_t *) 0x848f) = 0x6B;
       *((uint8_t *) 0x8490) = 0x4D;
       *((uint8_t *) 0x8491) = 0x8F;
       *((uint8_t *) 0x8492) = 0x3C;
       *((uint8_t *) 0x8493) = 0x6B;
       *((uint8_t *) 0x8494) = 0xED;
       *((uint8_t *) 0x8495) = 0x35;
       *((uint8_t *) 0x8496) = 0xA6;
       *((uint8_t *) 0x8497) = 0xB4;
       *((uint8_t *) 0x8498) = 0xD6;
       *((uint8_t *) 0x8499) = 0x9A;
       *((uint8_t *) 0x849a) = 0xD3;
       *((uint8_t *) 0x849b) = 0x7B;
       *((uint8_t *) 0x849c) = 0x8F;
       *((uint8_t *) 0x849d) = 0x3C;
       *((uint8_t *) 0x849e) = 0x6B;
       *((uint8_t *) 0x849f) = 0xED;
       *((uint8_t *) 0x84a0) = 0x35;
       *((uint8_t *) 0x84a1) = 0xA6;
       *((uint8_t *) 0x84a2) = 0xB4;
       *((uint8_t *) 0x84a3) = 0xD6;
       *((uint8_t *) 0x84a4) = 0x9A;
       *((uint8_t *) 0x84a5) = 0xD3;
       *((uint8_t *) 0x84a6) = 0x63;
       *((uint8_t *) 0x84a7) = 0xCF;
       *((uint8_t *) 0x84a8) = 0x1A;
       *((uint8_t *) 0x84a9) = 0xFB;
       *((uint8_t *) 0x84aa) = 0x4D;
       *((uint8_t *) 0x84ab) = 0x69;
       *((uint8_t *) 0x84ac) = 0xAD;
       *((uint8_t *) 0x84ad) = 0x35;
       *((uint8_t *) 0x84ae) = 0xA6;
       *((uint8_t *) 0x84af) = 0xB4;
       *((uint8_t *) 0x84b0) = 0xD8;
       *((uint8_t *) 0x84b1) = 0xF2;
       *((uint8_t *) 0x84b2) = 0x06;
       *((uint8_t *) 0x84b3) = 0xBF;
       *((uint8_t *) 0x84b4) = 0x26;
       *((uint8_t *) 0x84b5) = 0x9A;
       *((uint8_t *) 0x84b6) = 0xD3;
       *((uint8_t *) 0x84b7) = 0x5A;
       *((uint8_t *) 0x84b8) = 0x6B;
       *((uint8_t *) 0x84b9) = 0x4D;
       *((uint8_t *) 0x84ba) = 0x69;
       *((uint8_t *) 0x84bb) = 0xB1;
       *((uint8_t *) 0x84bc) = 0xE4;
       *((uint8_t *) 0x84bd) = 0x0D;
       *((uint8_t *) 0x84be) = 0x7F;
       *((uint8_t *) 0x84bf) = 0xBE;
       *((uint8_t *) 0x84c0) = 0xCD;
       *((uint8_t *) 0x84c1) = 0x78;
       *((uint8_t *) 0x84c2) = 0x7B;
       *((uint8_t *) 0x84c3) = 0xEC;
       *((uint8_t *) 0x84c4) = 0x66;
       *((uint8_t *) 0x84c5) = 0x2F;
       *((uint8_t *) 0x84c6) = 0x05;
       *((uint8_t *) 0x84c7) = 0x63;
       *((uint8_t *) 0x84c8) = 0xCF;
       *((uint8_t *) 0x84c9) = 0x1A;
       *((uint8_t *) 0x84ca) = 0xFB;
       *((uint8_t *) 0x84cb) = 0xD8;
       *((uint8_t *) 0x84cc) = 0x86;
       *((uint8_t *) 0x84cd) = 0xD3;
       *((uint8_t *) 0x84ce) = 0x5A;
       *((uint8_t *) 0x84cf) = 0x68;
       *((uint8_t *) 0x84d0) = 0x5A;
       *((uint8_t *) 0x84d1) = 0x6B;
       *((uint8_t *) 0x84d2) = 0x4D;
       *((uint8_t *) 0x84d3) = 0x8F;
       *((uint8_t *) 0x84d4) = 0x3D;
       *((uint8_t *) 0x84d5) = 0xB5;
       *((uint8_t *) 0x84d6) = 0xF9;
       *((uint8_t *) 0x84d7) = 0x8D;
       *((uint8_t *) 0x84d8) = 0xF6;
       *((uint8_t *) 0x84d9) = 0x9A;
       *((uint8_t *) 0x84da) = 0xD3;
       *((uint8_t *) 0x84db) = 0x5E;
       *((uint8_t *) 0x84dc) = 0x1E;
       *((uint8_t *) 0x84dd) = 0xEF;
       *((uint8_t *) 0x84de) = 0x05;
       *((uint8_t *) 0x84df) = 0x63;
       *((uint8_t *) 0x84e0) = 0xCF;
       *((uint8_t *) 0x84e1) = 0x1A;
       *((uint8_t *) 0x84e2) = 0xFB;
       *((uint8_t *) 0x84e3) = 0xD8;
       *((uint8_t *) 0x84e4) = 0x86;
       *((uint8_t *) 0x84e5) = 0xD3;
       *((uint8_t *) 0x84e6) = 0x5A;
       *((uint8_t *) 0x84e7) = 0x6B;
       *((uint8_t *) 0x84e8) = 0x4D;
       *((uint8_t *) 0x84e9) = 0x69;
       *((uint8_t *) 0x84ea) = 0xBD;
       *((uint8_t *) 0x84eb) = 0xC7;
       *((uint8_t *) 0x84ec) = 0x9E;
       *((uint8_t *) 0x84ed) = 0xDA;
       *((uint8_t *) 0x84ee) = 0xFC;
       *((uint8_t *) 0x84ef) = 0xC6;
       *((uint8_t *) 0x84f0) = 0xFB;
       *((uint8_t *) 0x84f1) = 0x4D;
       *((uint8_t *) 0x84f2) = 0x69;
       *((uint8_t *) 0x84f3) = 0xAF;
       *((uint8_t *) 0x84f4) = 0x0F;
       *((uint8_t *) 0x84f5) = 0x77;
       *((uint8_t *) 0x84f6) = 0x82;
       *((uint8_t *) 0x84f7) = 0xB1;
       *((uint8_t *) 0x84f8) = 0xE7;
       *((uint8_t *) 0x84f9) = 0x8D;
       *((uint8_t *) 0x84fa) = 0x7D;
       *((uint8_t *) 0x84fb) = 0xEC;
       *((uint8_t *) 0x84fc) = 0x43;
       *((uint8_t *) 0x84fd) = 0x69;
       *((uint8_t *) 0x84fe) = 0xAD;
       *((uint8_t *) 0x84ff) = 0x35;



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
//    __enable_interrupt();
//    P4DIR |= BIT0; // make Port 4 pin 0 an output
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
    WVAR(readPt,   READ_ADDR);       // Write
    WVAR(writePt,  WRITE_ADDR);     // Read
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

    unsigned int inbufIdx =              RVAR(bufIdx);
    unsigned char *inwritePt = (unsigned char *) RVAR(writePt);
    unsigned int inbuf =                 RVAR(buf);
    unsigned char inbyte_flag =            RVAR(byte_flag);

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


        WVAR( buf, inbuf ) ;
        WVAR(writePt, (unsigned int ) inwritePt);
        WVAR(bufIdx,inbufIdx );
        WVAR(byte_flag, inbyte_flag);

        // if decompressing the data is not done
        if( ( inwritePt - ( (unsigned char *) WRITE_ADDR ) ) >=  DATA_LEN) // *( (uint16_t *) DATA_LEN ) )
        {
            P3OUT |=BIT5;
            P3OUT &=~BIT5;
//            os_unblock(init_task);
//            blinkLed(2500 );
            os_jump(2);  // jump the init_task
            break;
        }else{
            os_jump(3);  // jump the init_task
        }

    }
}


int main(int argc, char const *argv[])
{
    init();


    taskId tasks[] = {  {init_task0,0},
                        {init_task,0},
                        {task_read,0},
                        {task_write,0 }};
    //This function should be called only once
    os_addTasks(4, tasks );

    os_scheduler();
    return 0;
}

