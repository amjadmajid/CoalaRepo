#include <msp430.h> 


/*
 * The result of this test shows that the ternary operator is slower than if else by 3 us
 * ternary operator: 21.88 us
 * if-else: 18.94 us
 */
volatile unsigned int x = 0;
volatile unsigned int x_res = 0;
volatile unsigned int y = 0;
volatile unsigned int y_res = 0;
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.

	P3DIR |= BIT5;

	P3OUT |= BIT5;
    if(x < 4)
    {
        x_res= 0;
    }else{
        x_res =9;
    }

    P3OUT &= ~BIT5;

    P3OUT |= BIT5;
    y_res = (y < 2) ? 4 : 8;
    P3OUT &= ~BIT5;

    __no_operation();
    while(1);
	return 0;
}
