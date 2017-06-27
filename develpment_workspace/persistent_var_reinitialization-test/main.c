#include <msp430.h>
#include <stdint.h>


#define __nv  __attribute__((section(".nv_vars")))

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
    P4OUT |= BIT0; // lit the LED
    burn(wait); // wait
    P4OUT &= ~BIT0; // dim the LED
    burn(wait); // wait
}


// initialize MSP430
void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
    P4OUT = 0;
    P4DIR |= BIT0; // make Port 1 pin 0 an output
}


/*#####################################
 #    Type of persistent variables    #
*###################################### */
__nv  uint16_t var ;
//__nv  uint16_t var2 = 0 ;
// uint16_t * pt = (uint16_t *) 0x8400;


int main(void) {
    init();

//    if( *((uint8_t *) 0x1902)  != 0xAD )
//    {
//        *(pt) =  0 ;
//        *((uint8_t *) 0x1902) = 0xAD;
//    }

    var = 0;

    while(1)
    {
//        if( *(pt) == 0)
//        {
//            blinkLed(1000);
//            blinkLed(1000);
//            blinkLed(1000);
//
//            *(pt) = 1;
//        }else
//        {
//            blinkLed(50000);
//            *(pt) = 0;
//        }

        if (var == 0)
            {
                blinkLed(50000);
                var = 1;
            }
        if( var == 1 )
        {
            blinkLed(500);
            blinkLed(500);
            blinkLed(500);
            var = 0;

        }

//
//        if (var2 ==0)
//            {
//                blinkLed(1000);
//                blinkLed(10000);
//                blinkLed(1000);
//                blinkLed(10000);
//                var2=1;
//            }else{
//                var = 0;
//            }


    }



    return 0;
}
