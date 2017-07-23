/*
    This is a DFT implemention in chain

    @ Author: Anonymous
    @ Date  : 6/April/2017

*/
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <math.h>

#include <chain.h>
#include <stdlib.h>

#define SIZE 64
#define PI2 6.28

#ifdef DEBUG
  float x[SIZE];                    // discrete-time signal, x
  float x_chain[SIZE];              // discrete-time signal, x
  float Xre[SIZE], Xim[SIZE];       // DFT of x (real and imaginary parts)
  float P[SIZE];
  float p_deb[SIZE]={0};
 void dft_c() {
    /*
        The dft_c function is taken from
        https://batchloaf.wordpress.com/2013/12/07/simple-dft-in-c/
    */
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

#endif

void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;
    __enable_interrupt();
    P1DIR |=BIT0;

#ifdef DEBUG
    dft_c() ;
#endif

}


/*******Declare Channel fields **********/
struct msg_n {
    CHAN_FIELD(unsigned, n);
};

struct msg_k {
    CHAN_FIELD(unsigned, k);
};

struct msg_n_k {
    CHAN_FIELD(unsigned, k);
};

struct msg_x_arr {
    CHAN_FIELD_ARRAY(float, x, SIZE);
};

struct msg_xre {
    CHAN_FIELD_ARRAY(float, xre, SIZE);
    CHAN_FIELD(unsigned, k);
};

struct msg_xim {
    CHAN_FIELD_ARRAY(float, xim, SIZE);
    CHAN_FIELD(unsigned, k);
};

struct msg_x_im_re {
    CHAN_FIELD_ARRAY(float, xre, SIZE);
    CHAN_FIELD_ARRAY(float, xim, SIZE);
};

struct msg_p_arr {
    CHAN_FIELD_ARRAY(unsigned, p, SIZE);
};

//DEBUGGING

/*******Declare tasks **********/
TASK(1, dft_init)
TASK(2, signal_counter)
TASK(3, signal_initializer)
TASK(4, dft_outer_loop)
TASK(5, dft_inner_loop)
TASK(6, dft_real)
TASK(7, dft_im)
TASK(8, dft_update)
TASK(9, dft_power)
TASK(10, dft_end)

/*******Declare channels **********/
CHANNEL(dft_init            , dft_outer_loop        , msg_k);
CHANNEL(dft_init            , dft_inner_loop        , msg_n);
CHANNEL(dft_init            , signal_counter        , msg_n);

CHANNEL(signal_counter      , signal_initializer    , msg_n);

CHANNEL(signal_initializer  , signal_counter        , msg_n);
CHANNEL(signal_initializer  , dft_real              , msg_x_arr );
CHANNEL(signal_initializer  , dft_im                , msg_x_arr );



CHANNEL(dft_outer_loop      , dft_power             , msg_k );
CHANNEL(dft_outer_loop      , dft_update            , msg_k );
CHANNEL(dft_outer_loop      , dft_real              , msg_xre);
CHANNEL(dft_outer_loop      , dft_im                , msg_xim);

CHANNEL(dft_inner_loop      , dft_real              , msg_n );
CHANNEL(dft_inner_loop      , dft_im                , msg_n );
CHANNEL(dft_inner_loop      , dft_update            , msg_n );



CHANNEL(dft_update          , dft_inner_loop        , msg_n );
CHANNEL(dft_update          , dft_real              , msg_xre );
CHANNEL(dft_update          , dft_im                , msg_xim );
CHANNEL(dft_update          , dft_power             , msg_x_im_re );


CHANNEL(dft_real            , dft_update                , msg_xre );
CHANNEL(dft_im              , dft_update                , msg_xim );

CHANNEL(dft_power           , dft_outer_loop        , msg_k );

CHANNEL(dft_power           , dft_init              , msg_p_arr);



void dft_init() {
//    task_prologue();

    unsigned n = 0xabab;
    unsigned k = 0;

    CHAN_OUT1(unsigned, k, k, CH(dft_init, dft_outer_loop));
    CHAN_OUT1(unsigned, n, n, CH(dft_init, dft_inner_loop));
    CHAN_OUT1(unsigned, n, n, CH(dft_init, signal_counter));

    srand(0);

    TRANSITION_TO(signal_counter);
}

volatile unsigned n_ck;
// initialize x[n] with random samples
void signal_counter() {
//    task_prologue();

    unsigned n =  *CHAN_IN2( unsigned, n, CH( dft_init, signal_counter), CH(signal_initializer,signal_counter) );
    n_ck = n ;

    if (n < SIZE) {
        CHAN_OUT1(unsigned, n, n, CH(signal_counter, signal_initializer));
        TRANSITION_TO(signal_initializer);
    } else {
        TRANSITION_TO(dft_outer_loop);
    }
}

void signal_initializer() {
//    task_prologue();
    unsigned n =  *CHAN_IN1( unsigned, n, CH( signal_counter,signal_initializer ) );
    float x_n = ((2.0 * rand()) / RAND_MAX) - 1.0;
#ifdef DEBUG
    x_chain[n] = x_n;
#endif
    CHAN_OUT1(float, x[n], x_n , CH(signal_initializer, dft_real));
    CHAN_OUT1(float, x[n], x_n , CH(signal_initializer, dft_im));
    n++;
    CHAN_OUT1(unsigned, n, n, CH(signal_initializer, signal_counter));
    TRANSITION_TO(signal_counter);
}



void dft_outer_loop() {
//    task_prologue();
    P1OUT |=BIT4;
    unsigned k =  *CHAN_IN2( unsigned, k, CH( dft_init, dft_outer_loop), CH(dft_power,dft_outer_loop) );
    if (k < SIZE) {
        CHAN_OUT1(unsigned, k, k, CH(dft_outer_loop, dft_real));
        CHAN_OUT1(unsigned, k, k, CH(dft_outer_loop, dft_im));
        CHAN_OUT1(unsigned, k, k, CH(dft_outer_loop, dft_update));
        CHAN_OUT1(unsigned, k, k, CH(dft_outer_loop, dft_power));
        float _dummy=0;
        CHAN_OUT1(float, xre[k], _dummy , CH(dft_outer_loop, dft_real));
        CHAN_OUT1(float, xim[k], _dummy , CH(dft_outer_loop, dft_im));

        TRANSITION_TO(dft_inner_loop);
    } else {
        P1OUT &=~BIT4;
        TRANSITION_TO(dft_end);
    }
}

void dft_inner_loop()
{
//    task_prologue();

    unsigned n =  *CHAN_IN2( unsigned, n, CH( dft_init, dft_inner_loop), CH(dft_update,dft_inner_loop) );
    CHAN_OUT1(unsigned, n, n, CH(dft_inner_loop, dft_real));
    CHAN_OUT1(unsigned, n, n, CH(dft_inner_loop, dft_im));
    CHAN_OUT1(unsigned, n, n, CH(dft_inner_loop, dft_update));

    TRANSITION_TO(dft_real);
}


void dft_real() {
//    task_prologue();

    unsigned k =        *CHAN_IN1( unsigned, k   , CH( dft_outer_loop,      dft_real ) );
    unsigned n =        *CHAN_IN1( unsigned, n   , CH( dft_inner_loop,      dft_real ) );
    float x_n =     *CHAN_IN1( float, x[n], CH( signal_initializer, dft_real ) );
    float xre_k =   *CHAN_IN2( float, xre[k], CH( dft_outer_loop,   dft_real ), CH( dft_update, dft_real ) );

    xre_k += x_n * cosf(n * k * PI2 / SIZE);

    CHAN_OUT1(float, xre[k], xre_k, CH(dft_real, dft_update));
    TRANSITION_TO(dft_im);
}


void dft_im() {
//    task_prologue();

    unsigned k =        *CHAN_IN1( unsigned, k      , CH( dft_outer_loop,       dft_im ) );
    unsigned n =        *CHAN_IN1( unsigned, n      , CH( dft_inner_loop,       dft_im ) );
    float x_n =     *CHAN_IN1( float, x[n]  , CH( signal_initializer,   dft_im ) );
    float xim_k =   *CHAN_IN2( float, xim[k] , CH( dft_outer_loop,  dft_im ),  CH( dft_update,  dft_im ) );

    xim_k += x_n * sinf(n * k * PI2 / SIZE);


    CHAN_OUT1(float, xim[k], xim_k, CH(dft_im, dft_update));
    TRANSITION_TO(dft_update);
}


void dft_update()
{
//    task_prologue();
    unsigned n =    *CHAN_IN1( unsigned, n   , CH( dft_inner_loop,  dft_update ) );
    unsigned k =    *CHAN_IN1( unsigned, k   , CH( dft_outer_loop,  dft_update ) );


    float xre_k =   *CHAN_IN1( float, xre[k], CH( dft_real, dft_update ) );
    float xim_k =   *CHAN_IN1( float, xim[k], CH( dft_im,   dft_update ) );

    CHAN_OUT1(float, xre[k], xre_k, CH(dft_update, dft_real));
    CHAN_OUT1(float, xim[k], xim_k, CH(dft_update, dft_im));

    n++;
    if(n >= SIZE)
    {
        n=0;
        CHAN_OUT1(unsigned, n, n, CH( dft_update, dft_inner_loop));
        CHAN_OUT1(float, xre[k], xre_k, CH(dft_update, dft_power));
        CHAN_OUT1(float, xim[k], xim_k, CH(dft_update, dft_power));
        TRANSITION_TO(dft_power);
    }
    CHAN_OUT1(unsigned, n,  n, CH( dft_update, dft_inner_loop));
    TRANSITION_TO(dft_inner_loop);

}

void dft_power() {
//    task_prologue();
    unsigned k     =    *CHAN_IN1( unsigned,k, CH( dft_outer_loop   , dft_power ) );
    float xim_k =   *CHAN_IN1( float, xim[k], CH( dft_update  , dft_power ) );
    float xre_k =   *CHAN_IN1( float, xre[k], CH( dft_update , dft_power ) );



    float p_k = xre_k * xre_k + xim_k * xim_k;
#ifdef DEBUG
    p_deb[k] = p_k;
#endif
    CHAN_OUT1(float, p[k], p_k, CH(dft_power, dft_init));
    k++;
    CHAN_OUT1(unsigned, k, k, CH(dft_power, dft_outer_loop));
    TRANSITION_TO(dft_outer_loop);
}

void dft_end() {
#ifdef DEBUG
    unsigned i ;
    for(i=0; i< SIZE ; i++)
    {
        if(P[i] != p_deb[i] )
        {
                P4OUT |=BIT0;
                __delay_cycles(300000);
                P4OUT &=~BIT0;
                __delay_cycles(300000);
        }
    }
#endif
    P1OUT |=BIT0;
    __delay_cycles(30000);
    P1OUT &=~BIT0;
    __delay_cycles(30000);

    TRANSITION_TO(dft_end);
}

ENTRY_TASK(dft_init)
INIT_FUNC(init)
