#include <msp430.h> 
#include <math.h>
#include "ipos.h"
#include <stdint.h>




#if 0

#define SIZE 16
#define PI2 6.28

  float x[SIZE];              // discrete-time signal, x
  float Xre[SIZE], Xim[SIZE]; // DFT of x (real and imaginary parts)
  float P[SIZE];              // power spectrum of x

  void dft_c() {

    int n, k; // iSIZEdices for time and frequency domains

    // Generate random discrete-time signal x in range (-1,+1)
    srand(0);
    for (n = 0; n < SIZE; ++n)
      x[n] = ((2.0 * rand()) / RAND_MAX) - 1.0;



    // Calculate DFT of x using brute force
    for (k = 0; k < SIZE; ++k) {
      // Real part of X[k]
      Xre[k] = 0;

      for (n = 0; n < SIZE; ++n)
        Xre[k] += x[n] * cosf(n * k * PI2 / SIZE);


      // Imaginary part of X[k]
      Xim[k] = 0;
      for (n = 0; n < SIZE; ++n)
        Xim[k] -= x[n] * sinf(n * k * PI2 / SIZE);

      // Power at kth frequency bin
      P[k] = Xre[k] * Xre[k] + Xim[k] * Xim[k];
    }
  }

void init()
{
  WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
  // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
  PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
  P3DIR |=BIT5;
}


int main(void){
    init();
    while(1)
    {
        dft_c() ;
        P3OUT |= BIT5;
        P3OUT &= ~BIT5;
    }
}

#else

void discTimeSign();
void dft_outer_loop() ;
void dft_real();
void dft_im();
void  dft_power();
void dft_end() ;
static void blinkLed(uint32_t wait );
static void burn( uint32_t iters);

#define SIZE 8
#define PI2 6.28

__v  float x[SIZE] ={0.0};              // discrete-time signal, x
__v  float Xre[SIZE] ={0.0}, Xim[SIZE]={0.0}; // DFT of x (real and imaginary parts)
__v  float P[SIZE] ={0.0};              // power spectrum of x
__v uint16_t n =0, k = 0;



void discTimeSign()
{
    // Generate random discrete-time signal x in range (-1,+1)
    x[n] = ((2.0 * rand()) / RAND_MAX) - 1.0;
    n++;

    if(n < SIZE){
        os_same();
    }else{
        n = 0;
        os_block(discTimeSign);
    }
}

// Calculate DFT of x using brute force

void dft_outer_loop() {
    if(k >= (SIZE-1) )
    {
        os_unblock(dft_end);
    }
}

void dft_real() {
  Xre[k] += x[n] * cosf(n * k * PI2 / SIZE);
  n++;
  if(n < SIZE)
  {
      os_same();
  }else{
      n = 0;
  }
}

void dft_im() {
    Xim[k] -= x[n] * sinf(n * k * PI2 / SIZE);
    n++;
    if(n < SIZE)
    {
        os_same();
    }else{
        n = 0;
    }
}

void  dft_power(){
    P[k] = Xre[k] * Xre[k] + Xim[k] * Xim[k];
    k++;
}

void dft_end() {
    P3OUT |= BIT5;
    P3OUT &= ~BIT5;
    k = 0;

//    blinkLed(2500);
    os_block(dft_end);
    os_unblock(discTimeSign);
}

void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
    P3OUT &= ~BIT5;
    P3DIR |=BIT5;
}

//delay
volatile uint32_t work_x;
static void burn( uint32_t iters)
{
    uint32_t iter = iters;
    while(iter--)
        work_x++;
}


//blink an led once
static void blinkLed(uint32_t wait )
{
    P4DIR |= BIT0;
    P4OUT |= BIT0; // lit the LED
    burn(wait); // wait
    P4OUT &= ~BIT0; // dim the LED
    burn(wait); // wait
}


int main(void) {
    init();


       taskId tasks[] = {  {discTimeSign,0},
                           {dft_outer_loop,0},
                           {dft_real,0 },
                           {dft_im, 0},
                           {dft_power, 0},
                           {dft_end,1}
       };
       //This function should be called only once
       os_addTasks(6, tasks );

       os_scheduler();
       return 0;
}
#endif
