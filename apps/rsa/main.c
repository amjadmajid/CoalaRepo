//https://proprogramming.org/program-to-implement-rsa-algorithm-in-c/

// #include<stdio.h>
#include <msp430.h>
#include <stdlib.h>
#include <math.h>
#include <uart-debugger.h>
// #include<string.h>

#define MSG "hello"
#define MSG_LEN 6

 int p,q,n,t,flag,e[50],d[50],temp[50],j,m[50],en[50],i;

char * msg = MSG;

int prime( int);
void ce();
 int cd( int);

void encrypt();
void decrypt();

int main()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;
    uart_init();


    uart_sendText("START\n\r", 7);
    // first prime number
    p =  5;
    flag=prime(p);
    if(flag==0)
    {
        exit(1);
    }

    // second prime number
    q = 7;
    flag=prime(q);
    if(flag==0||p==q)
    {
        exit(1);
    }

    // plain text messege
    for(i=0; i < MSG_LEN  ;i++)
    {
        m[i]= * msg;
        msg++;
    }

    n=p*q;
    t=(p-1)*(q-1);

    ce();
    // printf("\nPOSSIBLE VALUES OF e AND d ARE\n");
    // for(i=0;i<j-1;i++)
    //     printf("\n%ld\t%ld",e[i],d[i]);
    encrypt();
    decrypt();

    while(1);

    return 0;
}

int prime( int pr)
{
    int i;
    j=sqrtf(pr);
    for(i=2;i<=j;i++)
    {
        if(pr%i==0)
            return 0;
    }
    return 1;
}
void ce()
{
    int k;
    k=0;
    for(i=2;i<t;i++)
    {
        if(t%i==0)
            {
                uart_sendHex16(i);
                uart_sendText("\n\r", 2);
                continue;
            }
        flag=prime(i);
        if(flag==1&&i!=p&&i!=q)
        {
            e[k]=i; flag=cd(e[k]);
            if(flag>0)
            {
                d[k]=flag;
                k++;
            }
            if(k==99)
                break;
        }
    }
}
 int cd( int x)
{
     int k=1;
    while(1)
    {
        k=k+t;
        uart_sendHex16(k);
        uart_sendText("\n\r", 2);
        if(k%x==0)
            return(k/x);
    }
}
void encrypt()
{
     int pt,ct,key=e[0],k,len;
    i=0;
    len= MSG_LEN;
    while(i!=len)
    {
        pt=m[i];
        pt=pt-96;
        k=1;
        for(j=0;j<key;j++)
        {
            k=k*pt;
            k=k%n;
        }
        temp[i]=k;
        ct=k+96;
        en[i]=ct;
        i++;
    }
    en[i]=-1;
    uart_sendText("THE ENCRYPTED MESSAGE IS\n\r", 26);
     for(i=0;en[i]!=-1;i++)
         uart_sendText(en[i], 1);
     uart_sendText('\n\r', 2);
}
void decrypt()
{
     int pt,ct,key=d[0],k;
    i=0;
    while(en[i]!=-1)
    {
        ct=temp[i];
        k=1;
        for(j=0;j<key;j++)
        {
            k=k*ct;
            k=k%n;
        }
        pt=k+96;
        m[i]=pt;
        i++;
    }
    m[i]=-1;
    uart_sendText("THE DECRYPTED MESSAGE IS\n\r", 26);
     for(i=0;m[i]!=-1;i++)
         uart_sendText(en[i], 1);
    uart_sendText('\n\r', 2);
}
