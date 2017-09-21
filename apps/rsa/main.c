#include <msp430.h>
#include <math.h>
#include <uart-debugger.h>
#include <ipos.h>

#define DEBUG 1

#define MSG "hello"
#define MSG_LEN 5

long int p,q, n, t, k,j, i, flag, e[10], d[10], m[10], temp[10], en[10],
        en_pt,en_ct=0,en_key, en_k, en_cnt, en_j=0,
        de_pt,de_ct=0,de_key, de_k, de_cnt, de_j=0;
char * msgPt = MSG;

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
        {initTask, 1},
        {ce_1, 2},
        {ce_2, 3},
        {is_i_prime, 4},
        {ce_3, 5},
        {cd, 6},
        {ce_4, 7},
        {encrypt_init, 8},
        {encrypt_inner_loop, 9},
        {encrypt_finish, 10},
        {encrypt_print, 11},
        {decrypt_init, 12},
        {decrypt_inner_loop, 13},
        {decrypt_finish, 14},
        {decrypt_print, 15},
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

#ifdef DEBUG
    uart_init();
#endif
}

void initTask()
{
#ifdef DEBUG
    uart_sendText("Start\n\r", 7);
    uart_sendText("\n\r", 2);
#endif

    p= 7;
    q= 17;
    n=p*q;
    t=(p-1)*(q-1);
    i=1;
    k=0;
    flag=0;
    for (int ii = 0; ii < MSG_LEN; ii++)
    {
        m[ii] = *(msgPt+ii);
    }
}

void ce_1()
{
    i++; // start with i=2

    if(i >= t)
    {
        os_jump(6);  // go to encryption
    }
}

void ce_2()
{
    if(t % i == 0)
    {
        os_jump(14); // go to ce_1
    }
}

void  is_i_prime()
{
    int c;
    j=sqrt(i);
    for(c=2;c<=j;c++)
    {
        if(i%c==0)
        {
            flag=0;
            os_jump(13); // go to ce_1
            return;
        }
    }
    flag = 1;
}

void ce_3()
{
    if(flag==1&&i!=p&&i!=q)
    {
        e[k]=i;
    }else{
        os_jump(12); // go to ce_1
    }
}

void cd()
{
    long int kk=1;
    while(1)
    {
        kk=kk+t;
        if(kk % e[k]==0){
            flag = (kk/ e[k] );
            break;
        }
    }
}

void ce_4()
{

    if(flag>0)
    {
        d[k]=flag;
        k++;
    }
    if(k < 9)
    {
        os_jump(10); // go to ce_1
    }
}

void encrypt_init()
{
   en_pt = m[en_cnt];
   en_pt -=96;
   en_k  = 1;
   en_j  = 0;
   en_key=e[0];
}

void encrypt_inner_loop()
{
   if(en_j < en_key)
   {
       en_k *= en_pt;
       en_k %= n;
       en_j++;
       os_jump(0);
   }
}

void encrypt_finish()
{
   temp[en_cnt] = en_k;
   en_ct = en_k + 96;
   en[en_cnt] = en_ct;

    if( en_cnt < MSG_LEN)
   {
       en_cnt++;
       os_jump(13); // go to encrypt_init
   }else{
      en[en_cnt] = -1;
   }
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
   de_k  = 1;
   de_j  = 0;
   de_key=d[0];
}

void decrypt_inner_loop()
{
   de_ct =  temp[de_cnt];
   if(de_j < de_key)
   {
       de_k *= de_ct;
       de_k %= n;
       de_j++;
       os_jump(0);
   }
}

void decrypt_finish()
{

   de_pt = de_k + 96;
   m[de_cnt] = de_pt;

    if( en[de_cnt] != -1)
   {
       de_cnt++;
       os_jump(13); // go to decrypt_1
   }
}

void decrypt_print()
{
#ifdef DEBUG
   uart_sendText("THE_DECRYPTED_MESSAGE_IS\n\r", 26);
    for(de_cnt=0;de_cnt < MSG_LEN ;de_cnt++){
        uart_sendChar(m[de_cnt]);
    }
   uart_sendText("\n\r", 2);
#endif

}









