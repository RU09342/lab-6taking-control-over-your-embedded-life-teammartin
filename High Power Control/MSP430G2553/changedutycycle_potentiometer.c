#include <msp430g2553.h>

//High Power Control. the main.c file changes frequency with potentiometer
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    /*** GPIO Set-Up ***/
    P1DIR |= BIT2 + BIT0 + BIT6; // PWM output, onboard red LED and green LED
    P1SEL |= BIT2;				// set P1.2 to be the PWM output pin

    /*** Timer0_A Set-Up ***/
    TA1CTL |= TACLR;		// reset timer
	TA1CTL  = TASSEL_2		// SMCLK
			+ ID_0			// input divider = 1
			+ MC_2;			// continuous mode, interrupt disabled
	TA1CCTL0 = OUTMOD_2		// compare mode
			+ CCIFG;
    DCOCTL = 0;							// Select lowest DCO settings
	BCSCTL1 = CALBC1_1MHZ;				// Set DCO to 1 MHz
	DCOCTL = CALDCO_1MHZ;
    TA0CTL |= TASSEL_2 | MC_1 | ID_3;
    TA0CCR0 |= 1000;
    TA0CCTL1 |= OUTMOD_7;
    TA0CCR1 |= 0;

	/*** ADC10 Set-Up ***/
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE + REFON; // ADC10ON, interrupt enabled
    ADC10CTL1 = INCH_3 + SHS_0 + ADC10DIV_3 + ADC10SSEL_3 + CONSEQ_0;	// clock source = SMCLK. Channel A3, P1.3.
    ADC10AE0 |= BIT0;                         // PA.1 ADC option select. used for potentiometer

	unsigned int a = 0, delay = 50000, sec = 0;
    for (;;)
    {
        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
        __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
		
		a = 0; sec = 0;                         // Delays to remove LED flickering
        while (a < delay) { a++; }
        while ((a >= delay) && (sec < 10)) { a = 0; sec++; }
        
        if (ADC10MEM < 50) {			// if less than half of the ADC10MEM range of values
            P1OUT |= BIT0; P1OUT &= ~BIT6; //red LED on. green LED off.
            TA0CCR1 = ADC10MEM; // make the ADC10 value equal to the timer register that creates the duty cycle until after 100.
        }
		if ((ADC10MEM >= 50) & (ADC10MEM < 100)) {
            P1OUT &= ~BIT0; P1OUT |= BIT6; //red LED off. green LED on.
            TA0CCR1 = ADC10MEM;
        }
        if (ADC10MEM >= 100) {					//ADC10MEM can go slightly higher than 100, but the cap is made here.
            P1OUT |= BIT0; P1OUT |= BIT6; //both LEDs on.
            TA0CCR1 = 100; // TA0CCR1 = ADC10MEM
        }
    }
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}