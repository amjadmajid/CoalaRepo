/*
@Autor: Amjad Yousef Majid
date: 02/Jan/2017
*/
#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include "chain.h"


//define channel's fields
struct msg_self_blinks{
    SELF_CHAN_FIELD(unsigned, blinks);
};

// the result of the MACRO below is hardcoded as { {{ (0x0200U) }} }
#define FIELD_INIT_msg_self_blinks { \
    SELF_FIELD_INITIALIZER \
}

//defining tasks
TASK(1,task_0)


//defining channels
SELF_CHANNEL(task_0, msg_self_blinks);

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
    P4OUT |= 0x01; // lit the LED
    burn(wait); // wait
    P4OUT &= ~0x01; // dim the LED
    burn(wait); // wait
}

void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
    __enable_interrupt();
    P4DIR |= 0x01; // make Port 4 pin 0 an output

}

unsigned blinks = 3;
volatile unsigned blinksIn=0;

void task_0()
{
    // required by chain
    task_prologue();

    if(blinks!=3){
        blinkLed(300000); // start of the task
        // declares it as volatile to prevent it from being optimized away
        blinksIn = *CHAN_IN1(unsigned, blinks, SELF_IN_CH(task_0));
    }
    blinks++;
    CHAN_OUT1(unsigned, blinks, blinks, SELF_OUT_CH(task_0));

    while(blinksIn--)
        blinkLed(15000);

    TRANSITION_TO(task_0);
}


// required by chain
ENTRY_TASK(task_0)
INIT_FUNC(init)
