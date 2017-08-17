/*
 Author: Amjad Yousef Majid
 Date: 06/feb/2017
*/
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <libmsp/periph.h>
#include <libmsp/clock.h>
#include <libmsp/watchdog.h>
#include <libmsp/gpio.h>

#ifdef CONFIG_EDB
#include <libedb/edb.h>
#endif

#ifdef TEST_EIF_PRINTF
#include <libio/printf.h>
#endif // TEST_EIF_PRINTF

#include "pins.h"


static void init_hw()
{
    msp_watchdog_disable();
    msp_gpio_unlock();
    msp_clock_setup();
}

#define ARR_SIZE 128

unsigned int  vo_num[ARR_SIZE+1] ;

 volatile unsigned int x = 0;
 unsigned int * src_pt = vo_num; //(unsigned int *) 0x1c20 ;//
 unsigned int * des_pt = (unsigned int *) 0x5000;


int main() {
init_hw();  
#ifdef CONFIG_EDB
    edb_init();
#endif




vo_num[0] = 7833;
vo_num[1] = 3516;
vo_num[2] = 38908;
vo_num[3] = 61578;
vo_num[4] = 9481;
vo_num[5] = 62372;
vo_num[6] = 39678;
vo_num[7] = 43210;
vo_num[8] = 37565;
vo_num[9] = 64062;
vo_num[10] = 16336;
vo_num[11] = 18089;
vo_num[12] = 41906;
vo_num[13] = 5503;
vo_num[14] = 21267;
vo_num[15] = 33938;
vo_num[16] = 54792;
vo_num[17] = 31088;
vo_num[18] = 6550;
vo_num[19] = 59163;
vo_num[20] = 60391;
vo_num[21] = 59576;
vo_num[22] = 55305;
vo_num[23] = 39295;
vo_num[24] = 1764;
vo_num[25] = 35078;
vo_num[26] = 57155;
vo_num[27] = 11349;
vo_num[28] = 10011;
vo_num[29] = 28523;
vo_num[30] = 12918;
vo_num[31] = 39206;
vo_num[32] = 2777;
vo_num[33] = 24431;
vo_num[34] = 35971;
vo_num[35] = 52107;
vo_num[36] = 161;
vo_num[37] = 40560;
vo_num[38] = 9179;
vo_num[39] = 21912;
vo_num[40] = 58699;
vo_num[41] = 40866;
vo_num[42] = 42119;
vo_num[43] = 4920;
vo_num[44] = 48737;
vo_num[45] = 60947;
vo_num[46] = 9775;
vo_num[47] = 2496;
vo_num[48] = 17719;
vo_num[49] = 18531;
vo_num[50] = 17101;
vo_num[51] = 17107;
vo_num[52] = 43585;
vo_num[53] = 46239;
vo_num[54] = 41694;
vo_num[55] = 55753;
vo_num[56] = 11137;
vo_num[57] = 10729;
vo_num[58] = 17337;
vo_num[59] = 685;
vo_num[60] = 20036;
vo_num[61] = 55686;
vo_num[62] = 31573;
vo_num[63] = 405;
vo_num[64] = 19305;
vo_num[65] = 57873;
vo_num[66] = 33679;
vo_num[67] = 44949;
vo_num[68] = 34473;
vo_num[69] = 32394;
vo_num[70] = 29538;
vo_num[71] = 62537;
vo_num[72] = 58778;
vo_num[73] = 9108;
vo_num[74] = 19482;
vo_num[75] = 23210;
vo_num[76] = 41514;
vo_num[77] = 11479;
vo_num[78] = 40532;
vo_num[79] = 59167;
vo_num[80] = 40328;
vo_num[81] = 61345;
vo_num[82] = 48272;
vo_num[83] = 17365;
vo_num[84] = 50869;
vo_num[85] = 3609;
vo_num[86] = 28572;
vo_num[87] = 52641;
vo_num[88] = 23830;
vo_num[89] = 1722;
vo_num[90] = 38188;
vo_num[91] = 27895;
vo_num[92] = 46932;
vo_num[93] = 43211;
vo_num[94] = 3491;
vo_num[95] = 5706;
vo_num[96] = 30695;
vo_num[97] = 25618;
vo_num[98] = 9844;
vo_num[99] = 22341;
vo_num[100] = 27749;
vo_num[101] = 4280;
vo_num[102] = 24484;
vo_num[103] = 62251;
vo_num[104] = 63766;
vo_num[105] = 54988;
vo_num[106] = 46107;
vo_num[107] = 22619;
vo_num[108] = 62598;
vo_num[109] = 31514;
vo_num[110] = 12480;
vo_num[111] = 13133;
vo_num[112] = 63796;
vo_num[113] = 22257;
vo_num[114] = 2702;
vo_num[115] = 37898;
vo_num[116] = 13190;
vo_num[117] = 46677;
vo_num[118] = 27550;
vo_num[119] = 45792;
vo_num[120] = 45152;
vo_num[121] = 53166;
vo_num[122] = 52018;
vo_num[123] = 20396;
vo_num[124] = 47177;
vo_num[125] = 34596;
vo_num[126] = 31780;
vo_num[127] = 60374;
vo_num[128] = 60374;
// vo_num[129] = 60374;



 while(1){

    WATCHPOINT(0);

    volatile unsigned cnt; 
    for(cnt=0; cnt < 15 ; cnt++)
        {
          while(x < ARR_SIZE){
                *des_pt = * src_pt;
                src_pt++;
                des_pt++;
                x++;
              }
          src_pt = vo_num;// (unsigned int *) 0x1c20 ;
          des_pt = (unsigned int *) 0x5000;  
          x=0;
        }

    WATCHPOINT(1); 


  }
    return 0;
}
