#include <msp430g2553.h>
#include "config.h"

void configureClock(void){

	BCSCTL1 = CALBC1_8MHZ;		// Set range
	DCOCTL = CALDCO_8MHZ;		// Set DCO step + modulation
	BCSCTL2 = DIVS_3;			// SMCLK / 8
	IFG1 &= ~OFIFG;				// Flag de falha do oscilador resetada
}

void initializePORT1(void){

	P1SEL |= RXD + TXD ;       					// P1.1 = RXD, P1.2=TXD
	P1SEL2 |= RXD + TXD ;      					// P1.1 = RXD, P1.2=TXD
	P1DIR |= RED_LED + GREEN_LED; // P1.0, P1.6, P1.3 and P1.4 as output
	P1OUT &= 0x00;

}





