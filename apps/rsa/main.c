#include <msp430.h>
#include <math.h>
#include <mspReseter.h>
#include "mspProfiler.h"
#include "mspDebugger.h"
#include <ipos.h>


//#define TSK_SIZ
//#define AUTO_RST
//#define LOG_INFO


//#define DEBUG 1

#define MSG "hello"
#define MSG_LEN 5

__p long int p,q, n, t, k,j, i, flag, e[10], d[10], m[10], temp[10], en[10],
        en_pt,en_ct=0,en_key, en_k, en_cnt, en_j=0,
        de_pt,de_ct=0,de_key, de_k, de_cnt, de_j=0;
char * msgPt = MSG;

__nv unsigned protect = 0;

// Prototypes
void initTask();
void ce_1();
void ce_2();
void is_i_prime();
void ce_3();
void cd();
void ce_4();
void encrypt_init();
void encrypt_inner_loop();
void encrypt_finish();
void encrypt_print();
void decrypt_init();
void decrypt_inner_loop();
void decrypt_finish();
void decrypt_print();
void init();


int main(void )
{
    init();

    taskId tasks[] = {
        {initTask,           1, 3},
        {ce_1,               2, 1},
        {ce_2,               3, 1 },
        {is_i_prime,         4, 4},
        {ce_3,               5, 1},
        {cd,                 6, 5},
        {ce_4,               7, 1},
        {encrypt_init,       8, 1},
        {encrypt_inner_loop, 9, 1},
        {encrypt_finish,     10, 1},
        {encrypt_print,      11, 1},
        {decrypt_init,       12, 1},
        {decrypt_inner_loop, 13, 1},
        {decrypt_finish,     14, 1},
        {decrypt_print,      15, 1},
    };

    //This function should be called only once
    os_addTasks(15, tasks );
    os_scheduler();

  return 0;
}

void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.

    P3OUT &=~BIT5;
    P3DIR |=BIT5;

#if 0
    CSCTL0_H = CSKEY >> 8;                // Unlock CS registers
    //    CSCTL1 = DCOFSEL_4 |  DCORSEL;                   // Set DCO to 16MHz
    CSCTL1 = DCOFSEL_6;                   // Set DCO to 8MHz
    CSCTL2 =  SELM__DCOCLK;               // MCLK = DCO
    CSCTL3 = DIVM__1;                     // divide the DCO frequency by 1
    CSCTL0_H = 0;
#endif

#ifdef TSK_SIZ
    cp_init();
#endif

#ifdef LOG_INFO
    uart_init();
#endif

#ifdef AUTO_RST
    mr_auto_rand_reseter(13000); // every 12 msec the MCU will be reseted
#endif

}

void initTask()
{
#ifdef TSK_SIZ
    cp_reset ();
#endif
    protect = 1;

#ifdef DEBUG
    uart_sendText("Start\n\r", 7);
    uart_sendText("\n\r", 2);
#endif

    int in_p = 7;
    int in_q = 17;

    WP(p)= in_p;
    WP(q)= in_q;
    WP(n)= in_p * in_q;
    WP(t)= (in_p-1) * (in_q-1);
    WP(i)=1;
    WP(k)=0;
    WP(flag)=0;
    for (int ii = 0; ii < MSG_LEN; ii++)
    {
        WP(m[ii]) = *(msgPt+ii);
    }

#ifdef TSK_SIZ
    cp_sendRes ("initTask \0");
#endif
}

void ce_1()
{
#ifdef TSK_SIZ
    cp_reset ();
#endif

    WP(i)++; // start with i=2

    if(RP(i) >= RP(t) )
    {
        os_jump(6);  // go to encryption
    }

#ifdef TSK_SIZ
    cp_sendRes ("ce_1 \0");
#endif
}

void ce_2()
{
#ifdef TSK_SIZ
    cp_reset ();
#endif

    if( RP(t) % RP(i) == 0)
    {
        os_jump(14); // go to ce_1
    }

#ifdef TSK_SIZ
    cp_sendRes ("ce_2 \0");
#endif
}

void  is_i_prime()
{
#ifdef TSK_SIZ
    cp_reset ();
#endif

    int c;
    c=sqrt(RP(i));
    WP(j) = c;
    for(c=2; c <= RP( j) ;c++)
    {
        if( RP(i) % c==0)
        {
            WP(flag)=0;
            os_jump(13); // go to ce_1
            return;
        }
    }
    WP(flag) = 1;

#ifdef TSK_SIZ
    cp_sendRes ("is_i_prime \0");
#endif
}

void ce_3()
{
#ifdef TSK_SIZ
    cp_reset ();
#endif

    long int in_i = RP(i);
    if( RP(flag) == 1 && in_i != RP(p) && in_i != RP(q) )
    {
        WP(e[RP(k)]) = in_i ;
    }else{
        os_jump(12); // go to ce_1
    }

#ifdef TSK_SIZ
    cp_sendRes ("ce_3 \0");
#endif
}

void cd()
{
#ifdef TSK_SIZ
    cp_reset ();
#endif

    long int kk=1, __cry;
    while(1)
    {
        kk=kk +  RP(t);
        if(kk % RP( e[RP(k)] ) ==0){
            __cry = (kk/ RP( e[ RP(k) ]) );
            WP(flag) = __cry;
            break;
        }
    }

#ifdef TSK_SIZ
    cp_sendRes ("cd \0");
#endif
}

void ce_4()
{
#ifdef TSK_SIZ
    cp_reset ();
#endif

    int __cry = RP(flag);
    if(__cry > 0)
    {
        WP(d[ RP(k) ]) =__cry;
        WP(k)++;
    }
    if( RP(k) < 9)
    {
        os_jump(10); // go to ce_1
    }

#ifdef TSK_SIZ
    cp_sendRes ("ce_4 \0");
#endif
}

void encrypt_init()
{
#ifdef TSK_SIZ
    cp_reset ();
#endif

  long int __cry;
   __cry = RP(m[ RP(en_cnt) ]) ;
   WP(en_pt) = __cry;
   WP(en_pt) -=96;
   WP(en_k)  = 1;
   WP(en_j)  = 0;
   __cry = RP(e[0]) ;
   WP(en_key) = __cry;

#ifdef TSK_SIZ
    cp_sendRes ("encrypt_init \0");
#endif
}

void encrypt_inner_loop()
{
#ifdef TSK_SIZ
    cp_reset ();
#endif

   long int __cry;
   if( RP(en_j) < RP(en_key) )
   {
       __cry = RP(en_k) * RP(en_pt);
       WP(en_k) = __cry;
       __cry = RP(en_k) % RP(n);
       WP(en_k) = __cry;
       WP(en_j)++;
       os_jump(0);
   }

#ifdef TSK_SIZ
    cp_sendRes ("encrypt_inner_loop \0");
#endif
}

void encrypt_finish()
{
#ifdef TSK_SIZ
    cp_reset ();
#endif

   long int __cry;
   __cry = RP(en_k);
   WP(temp[ RP(en_cnt) ]) = __cry;
   __cry = RP(en_k) + 96;
   WP(en_ct) = __cry;
   __cry = RP(en_ct);
   WP(en[ RP(en_cnt) ]) = __cry;

    if( RP(en_cnt) < MSG_LEN)
   {
       WP(en_cnt)++;
       os_jump(13); // go to encrypt_init
   }else{
      WP(en[ RP(en_cnt) ]) = -1;
   }

#ifdef TSK_SIZ
    cp_sendRes ("encrypt_finish \0");
#endif
}

void encrypt_print()
{
#ifdef DEBUG
   uart_sendText("THE_ENCRYPTED_MESSAGE_IS\n\r", 26);
    for(en_cnt=0;en_cnt < MSG_LEN;en_cnt++){
        uart_sendChar(en[en_cnt]);
    }
    uart_sendText("\n\r", 2);
#endif

}
void decrypt_init()
{
#ifdef TSK_SIZ
    cp_reset ();
#endif

   long int __cry;
   WP(de_k)  = 1;
   WP(de_j)  = 0;
   __cry =d[0];
   WP(de_key) = __cry;

#ifdef TSK_SIZ
    cp_sendRes ("decrypt_init \0");
#endif
}

void decrypt_inner_loop()
{
#ifdef TSK_SIZ
    cp_reset ();
#endif
   long int __cry;
   __cry =  RP(temp[ RP(de_cnt) ]);
   WP(de_ct) = __cry;

   if( RP(de_j) < RP(de_key) )
   {
       __cry = RP(de_k) * RP(de_ct);
       WP(de_k) = __cry;
       __cry = RP(de_k) % RP(n);
       WP(de_k) = __cry;
       WP(de_j)++;
       os_jump(0);
   }

#ifdef TSK_SIZ
   cp_sendRes ("decrypt_inner_loop \0");
#endif
}

void decrypt_finish()
{
#ifdef TSK_SIZ
    cp_reset ();
#endif

   long int __cry;
   __cry = RP(de_k) + 96;
   WP(de_pt) = __cry;
   WP(m[ RP(de_cnt) ]) = __cry;

    if( RP(en[ RP(de_cnt) ]) != -1)
   {
       WP(de_cnt)++;
       os_jump(13); // go to decrypt_init
   }

#ifdef TSK_SIZ
   cp_sendRes ("decrypt_print \0");
#endif
}

void decrypt_print()
{

#ifdef TSK_SIZ
    cp_reset ();
#endif

    __disable_interrupt();

    if(protect){
        P3OUT |=BIT5;
        P3OUT &=~BIT5;
    }
    protect=0;

    __enable_interrupt();

#ifdef TSK_SIZ
    cp_sendRes ("decrypt_finish \0");
#endif

#ifdef DEBUG
   uart_sendText("THE_DECRYPTED_MESSAGE_IS\n\r", 26);
    for(de_cnt=0;de_cnt < MSG_LEN ;de_cnt++){
        uart_sendChar(m[de_cnt]);
    }
   uart_sendText("\n\r", 2);
#endif

}









