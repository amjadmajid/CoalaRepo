/*
@Autor: Amjad Yousef Majid
date: 02/Jan/2017
*/
#include <msp430.h>
#include <stdio.h>
#include <chain.h>
#include <stdint.h>

//define channel's fields
struct msg_blinks{
    CHAN_FIELD(unsigned, blinks);
};

//defining tasks
TASK(1,task_0)
TASK(2,task_1)

//defining channels
CHANNEL(task_0,task_1, msg_blinks);

//delay
volatile unsigned work_x;
static void burn( uint32_t iters)
{
    uint32_t iter = iters;
    while(iter--)
        work_x++;
}

//blink an led once
static void blinkLed(uint32_t wait )
{
    P1OUT |= 0x01; // lit the LED
    burn(wait); // wait
    P1OUT &= ~0x01; // dim the LED
    burn(wait); // wait
}

void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
    __enable_interrupt();
    P1DIR |= 0x01; // make Port 4 pin 0 an output

}

void task_0()
{
    // required by chain
    task_prologue();

    blinkLed( 250000 );

    unsigned blinks = 3;
    CHAN_OUT1(unsigned, blinks, blinks, CH(task_0, task_1));
    TRANSITION_TO(task_1);
}

void task_1()
{
    // required by chain
    task_prologue();

    unsigned blinks = *CHAN_IN1(unsigned,blinks, CH(task_0, task_1) );
    blinks +=blinks; // blinks * 2
    blinkLed( 250000 );
    while(blinks--)
        blinkLed( 50000 );

    TRANSITION_TO(task_0);

}

// required by chain
ENTRY_TASK(task_0)
INIT_FUNC(init)
