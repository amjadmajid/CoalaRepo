// This application requires a lot of memory
// therefore it uses 8Kb for paging


#include <msp430.h> 
#include <math.h>
#include <stdint.h>

#include <mspReseter.h>
#include "mspProfiler.h"
#include "mspDebugger.h"
#include <ipos.h>


//#define TSK_SIZ
//#define AUTO_RST
//#define LOG_INFO


// The original program

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

#define SIZE 16
#define PI2 6.28

__p  float x[SIZE] ={0.0};              // discrete-time signal, x
__p  float Xre[SIZE] ={0.0}, Xim[SIZE]={0.0}; // DFT of x (real and imaginary parts)
__p  float P[SIZE] ={0.0};              // power spectrum of x
__p unsigned int n =0, k = 0;



void discTimeSign()
{

#ifdef TSK_SIZ
       cp_reset();
#endif

    unsigned int in_n = RP(n);
    float in_x_n = RP(x[in_n]);

        // Generate random discrete-time signal x in range (-1,+1)
        in_x_n = ((2.0 * rand()) / RAND_MAX) - 1.0;

        WP(x[in_n]) = in_x_n;

        in_n++;

        if(in_n < SIZE){
            os_jump(0);
        }else{
            in_n = 0;
        }

    // commit the output of the task
    WP(n) = in_n;

#ifdef TSK_SIZ
     cp_sendRes("discTimeSign \0");
#endif

}

// Calculate DFT of x using brute force

void dft_outer_loop() {
    // Get the input for the task

}

void dft_real() {

#ifdef TSK_SIZ
       cp_reset();
#endif

    unsigned int in_k = RP(k);
    unsigned int in_n = RP(n);
    float in_Xre_k = RP(Xre[in_k]);

        in_Xre_k += RP(x[in_n]) * cosf(in_n * in_k * PI2 / SIZE);
        // commit to the buffer
        WP(Xre[in_k]) = in_Xre_k;
        in_n++;
        if(in_n < SIZE)
        {
          os_jump(0);
        }else{
            in_n = 0;
        }

    // commit the output of the task
    WP(n) = in_n;

#ifdef TSK_SIZ
     cp_sendRes("dft_real \0");
#endif
}

void dft_im() {

#ifdef TSK_SIZ
       cp_reset();
#endif

    unsigned int in_k = RP(k);
    unsigned int in_n = RP(n);
    float in_Xim_k = RP(Xim[in_k]);

        in_Xim_k -= RP(x[in_n]) * sinf(in_n * in_k * PI2 / SIZE);
        WP(Xim[in_k]) = in_Xim_k;
        in_n++;
        if(in_n < SIZE)
        {
            os_jump(0);
        }else{
            in_n = 0;
        }

    // commit the output of the task
    WP(n) = in_n;

#ifdef TSK_SIZ
     cp_sendRes("dft_im \0");
#endif
}

void  dft_power(){

#ifdef TSK_SIZ
       cp_reset();
#endif

    unsigned int in_k = RP(k);
    float in_Xim_k = RP(Xim[in_k]);
    float in_Xre_k = RP(Xre[in_k]);
    float in_p_k = RP(P[in_k]);

        in_p_k = in_Xre_k * in_Xre_k + in_Xim_k * in_Xim_k;
        in_k++;

    // commit the output of the task

        if(in_k < SIZE )
        {
            os_jump(3);
        }

    WP(k) = in_k;
    WP(P[in_k]) = in_p_k;

#ifdef TSK_SIZ
     cp_sendRes("dft_power \0");
#endif
}


void dft_end() {

#ifdef TSK_SIZ
       cp_reset();
#endif

    unsigned int in_k = RP(k);

        P3OUT |= BIT5;
        P3OUT &= ~BIT5;
        in_k = 0;

    // commit the output of the task
    WP(k) = in_k;

#ifdef TSK_SIZ
     cp_sendRes("dft_end \0");
#endif
}

void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
    P3OUT &= ~BIT5;
    P3DIR |=BIT5;

#if 1
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
    mr_auto_rand_reseter(50000); // every 12 msec the MCU will be reseted
#endif

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

    // uart_sendText("Start\n\r", 7);


       taskId tasks[] = {  {discTimeSign,1, 4},
                           {dft_outer_loop,2, 1},
                           {dft_real,3, 5 },
                           {dft_im, 4, 5},
                           {dft_power, 5, 1},
                           {dft_end,6, 2}
       };
       //This function should be called only once
       os_addTasks(6, tasks );

       os_scheduler();
       return 0;
}
#endif
