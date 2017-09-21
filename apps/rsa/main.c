#include <msp430.h>
#include <math.h>
#include <uart-debugger.h>
#include <ipos.h>

#define DEBUG 1

#define MSG "hello"
#define MSG_LEN 5

long int p,q, n, t, k,j, i, flag, e[10], d[10], m[10], temp[10], en[10];
char * msgPt = MSG;

// Prototypes
void initTask();
void ce_1();
void ce_2();
void is_i_prime();
void ce_3();
void cd();
void ce_4();
void encrypt();
void decrypt();
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
        {encrypt, 8},
        {decrypt, 9}
    };

    //This function should be called only once
    os_addTasks(9, tasks );
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
        os_jump(8); // go to ce_1
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
            os_jump(7); // go to ce_1
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
        os_jump(6); // go to ce_1
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
    if(k > 9)
    {
        os_jump( 1 );  // go to encrypt ( this function call is not needed)
    }else{
        os_jump(4); // go to ce_1
    }

}

void encrypt()
{
    long int pt,ct,key=e[0], kk;

    int cnt=0;
    while(cnt < MSG_LEN)
    {
        pt=m[cnt];
        pt=pt-96;
        kk=1;
        for(j=0;j<key;j++)
        {
            kk=kk*pt;
            kk=kk%n;
        }
        temp[cnt]=kk;
        ct=kk+96;
        en[cnt]=ct;
        cnt++;
    }
    en[cnt]=-1;

#ifdef DEBUG
    uart_sendText("THE_ENCRYPTED_MESSAGE_IS\n\r", 26);
     for(cnt=0;cnt < MSG_LEN;cnt++){
         uart_sendChar(en[cnt]);
     }
     uart_sendText("\n\r", 2);
#endif

__delay_cycles(500000);
}

void decrypt()
{
    long int pt,ct,kk, key=d[0], cnt=0;

    while(en[cnt]!=-1)
    {
        ct=temp[cnt];
        kk=1;
        for(j=0;j<key;j++)
        {
            kk=kk*ct;
            kk=kk%n;
        }
        pt=kk+96;
        m[cnt]=pt;
        cnt++;
    }
    m[cnt]=-1;

#ifdef DEBUG

    uart_sendText("THE_DECRYPTED_MESSAGE_IS\n\r", 26);
     for(cnt=0;cnt < MSG_LEN ;cnt++){
         uart_sendChar(m[cnt]);
     }
    uart_sendText("\n\r", 2);

#endif

__delay_cycles(500000);

}










