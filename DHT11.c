/**
 * \file DHT11.c
 * \author Mateus Menezes
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <msp430g2553.h>
#include <msp430.h>
#include <stdint.h>
#include "config.h"
#include "DHT11.h"

#define DHPIN ( BIT7 )
#define _PORT (  1   )

#if _PORT == 1
#define PORTDIR P1DIR
#define PORTOUT P1OUT
#define PORTIN  P1IN
#endif

#if _PORT == 2
#define PORTDIR P2DIR
#define PORTOUT P2OUT
#define PORTIN  P2IN
#endif

void DHTbegin(void){

   	TACTL = TASSEL_2 + MC_1;		// Clock source = SMCLK + up mode
    TACCR0 = 1000;					// Period = 1ms
    TACTL |= TACLR;					// Timer halted

}

DHT_STATUS DHTread(char *_humidity, char *_temperature){

	uint8_t checksum;
	unsigned char idx = 0;
	unsigned char cnt = 7;
	unsigned char bits[5];
	int i;

	for(i = 0; i < 5; i++)
		bits[i] = 0;

	TACCTL0 &= ~CCIFG;			// Clear TimerA0 interrupt flag

	PORTDIR |= DHPIN;			// DHPIN as output
	PORTOUT &= ~DHPIN;			// DHPIN at low level

	/* Wait 18ms */
	TACCR0 = 18000;
	TACTL |= MC_1;				// Up mode
	while(!(TACCTL0 & CCIFG));	// Wait counter to reach TACCR0
	TACCTL0 &= ~CCIFG;			// Clear TACCTL0 flag
	TACTL |= TACLR;				// Reset counter and stop

	PORTOUT |= DHPIN;			// DHPIN at high level

	/* Wait 40us */
	TACCR0 = 40;
	TACTL |= MC_1;
	while(!(TACCTL0 & CCIFG));
	TACCTL0 &= ~CCIFG;
	TACTL |= TACLR;

	PORTDIR &= ~DHPIN;			// DHPIN as input

	/* Response of the DHT11 */
	TACCR0 = 100;				// Set timeout to 100us
	TACTL |= MC_1;
	while(!(PORTIN & DHPIN)){	// Wait DHPIN to go to high level
		if(TACCTL0 & CCIFG){	// If passed 100us, timeout!
			TACCTL0 &= ~CCIFG;
			return TIMEOUT;
		}
	}
	TACCTL0 &= ~CCIFG;
	TACTL |= TACLR;

	TACCR0 = 100;
	TACTL |= MC_1;
	while(PORTIN & DHPIN){		// Wait DHPIN to go to low level
		if(TACCTL0 & CCIFG){	// If passed 100us, timeout!
			TACCTL0 &= ~CCIFG;
			return TIMEOUT;
		}
	}
	TACCTL0 &= ~CCIFG;
	TACTL |= TACLR;

	for(i = 0; i < 40; i++){

		TACCR0 = 100;
		TACTL |= MC_1;
		while(!(PORTIN & DHPIN)){
			if(TACCTL0 & CCIFG){
				TACCTL0 &= ~CCIFG;
				return TIMEOUT;
			}
		}
		TACCTL0 &= ~CCIFG;
		TACTL |= TACLR;

		/* Start to transmit 1-bit data
		 * If passed 100us, timeout!
		 */
		TACCR0 = 100;
		TACTL |= MC_1;
		while(PORTIN & DHPIN){		// Wait to DHPIN to go to high level
			if(TACCTL0 & CCIFG){
				TACCTL0 &= ~CCIFG;
				return TIMEOUT;
			}
		}
		TACCTL0 &= ~MC_3;			// Timer stopped
		char timer = TAR;			// Value of the counter
		TACCTL0 &= ~CCIFG;
		TACTL |= TACLR;

		if(timer > 40)				// If value greater than 40us, bit 1
			bits[idx] |= (1 << cnt);

		if(cnt == 0){				// If received 8 bits, next byte
			cnt = 7;
			idx++;
		}
		else
			cnt--;


	}

	/* The DHT11 protocol frame
	 * 1º byte = humidity
	 * 2º byte = 0
	 * 3º byte = temperature
	 * 4º byte = 0
	 * 5º byte = checksum
	 */

	*_humidity = bits[0];
	*_temperature = bits[2];

	checksum = bits[0] + bits[2];

	if(checksum != bits[4])
		return CHECKSUM_ERROR;

	return DHT_OK;

}
