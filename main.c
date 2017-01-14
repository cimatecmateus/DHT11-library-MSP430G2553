#include <msp430g2553.h>
#include <msp430.h>
#include <intrinsics.h>
#include <stdint.h>
#include <stddef.h>
#include "uart.h"
#include "config.h"
#include "DHT11.h"

#ifndef TIMER0_A1_VECTOR
#define TIMER0_A1_VECTOR TIMERA1_VECTOR
#define TIMER0_A0_VECTOR TIMERA0_VECTOR
#endif

void delay_ms(int x_ms);

char humidity = 0;
char temperature = 0;

void main(void){

	WDTCTL = WDTPW + WDTHOLD; //Turn off watchdog

	if (CALBC1_8MHZ ==0xFF || CALDCO_8MHZ == 0xFF)	// If error in the calibration data, trap!
		while(1);

    configureClock();
    initializePORT1();
    DHTbegin();

    uart_config_t config;
    config.baud = 9600;				// Set UART baud rate to 9600

    if(uart_init(&config) != 0)		//If error in the uart config, trap!
    	while(1);

	_BIS_SR(GIE);					// GIE enable

	while(1){

		if(DHTread(&humidity, &temperature) == DHT_OK){

			P1OUT ^= GREEN_LED;			// Toggle green led
			P1OUT &= ~RED_LED;			// Turn off red led

			uart_putchar('t');			// Send 't' character
			uart_putchar(temperature);	// Send temperature from DHT11 sensor

			uart_putchar('h');			// Send 'h' character
			uart_putchar(humidity);		// Send humidity from DHT11 sensor

		}

		else{

			P1OUT ^= RED_LED;		// Toggle red led
			P1OUT &= ~GREEN_LED;	// Turn off green led

		}

		delay_ms(1000);		// Wait 2 seconds
	}
}

void delay_ms(int x_ms){
	TA1CCR0 = 1000;					// Set interrupt period to 1ms
	TA1CTL |= TASSEL_2;				// Clock source = SMCLK
	int i;
	for(i = 0; i < x_ms; i++){
		TA1CTL |= MC_1;				// Turn on timer at up mode
		while(!(TA1CCTL0 & CCIFG));	// Wait TAR to count until TACCR0
		TA1CCTL0 &= ~CCIFG;			// Clear interrupt flag
		TA1CTL |= TACLR;			// Clear TAR and stop it
	}
}





