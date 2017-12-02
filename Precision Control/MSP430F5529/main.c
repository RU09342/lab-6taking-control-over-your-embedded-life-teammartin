#include <msp430f5529.h>

//Precision Control

/*** initialize prototype functions ***/
void pinOutput(int values);
void trianglewave();
void Counter8bit();

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	    // Stop watchdog timer

	/*** Initialize GPIO pins ***/
	P2DIR |= BIT0;                            // P2.0 output
    P2SEL |= BIT0;                            // P2.0 for TA1 CCR1 Output Capture
	P6DIR |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4; // Initialize GPIO pins for R2R ladder
	P3DIR |= BIT5 + BIT6;
    P7DIR |= BIT0;
	
	/*** Timer1_A1 Set-Up ***/
    TA1CTL |= TASSEL_2 | MC_1 | ID_3; // SMCLK, up-mode, divide by 2^3
    TA1CCR0 |= 1000; // 2^12 bits, 12 bit ADC. 1000 Hz output frequency. Change this number to change frequency.
    TA1CCTL1 |= OUTMOD_7; //P2.0
    TA1CCR1 = 500; // 50% duty cycle. Complement the changing of frequency by changing this as well.

	/*** Begin outputting a triangle wave or a step/staircase function, choose only one. ***/
	//Counter8bit();
	trianglewave();

	return 0;
}

void Counter8bit(){
    int i = 0;  // for-loop counters
    int j = 0;	// for-loop delays used in creating pauses in 8-bit counter.

    while(1){
        for(i = 0; i < 256; i=i+16){   // Increment up to 256 in 17 total steps
			if(i > 255) {			//fix i value if i goes beyond 255 (keeping it below 1000 for the timer reg).
				i = 255;
			}
			pinOutput(i);           // Set the R2R ladder D/A circuit to increment the 8-bit binary number of 0-255
			i *= 125;								
			i = i >> 5;				// 255*125 = 31875 = 111110010000011. Ans >> 5 = 1111100100 = 996.
			TA1CCR1 = i;			// converted i index fits within 0-1000 range.
            for(j = 0; j < 1000; j++); // delay to create steps
        }
        for(i = 255; i >= 0; i=i-32){  // Decrement index until 0 (8 bit 0x00) is reached
            if(i < 0) {             //fix i value if i goes below 0 (keeping it above 0 for the timer reg)
				i = 0;
			}
			pinOutput(i);           // Set the R2R ladder D/A circuit to decrement the 8-bit binary number of 0-255
			i *= 125;								
			i = i >> 5;				// 255*125 = 31875 = 111110010000011. Ans >> 5 = 1111100100 = 996.
			TA1CCR1 = i;			// converted i index fits within 0-1000 range.
            for(j = 0; j < 1000; j++); // delay to create steps
        }
    }
}

void trianglewave(){

    int i = 0;   // for-loop counters

    while(1){
        for(i = 0; i < 256; i++){   // Increment up to 256 one integer at a time.
            if(i > 255) {
				i = 255;
			}
			i *= 125;								
			i = i >> 5;				// 255*125 = 31875 = 111110010000011. Ans >> 5 = 1111100100 = 996.
			TA1CCR1 = i;			// converted i index fits within 0-1000 range.
            //pinOutput(i);           // This line is not needed since the triangle wave does not solely enter the R2R ladder
        }                           
        for(i = 255; i >= 0; i--){  // Decrement down to 0 one integer at a time.
            if(i < 0) {
				i = 0;
			}
			i *= 125;								
			i = i >> 5;				// 255*125 = 31875 = 111110010000011. Ans >> 5 = 1111100100 = 996.
			TA1CCR1 = i;			// converted i index fits within 0-1000 range.
            //pinOutput(i);           // This line is not needed since the triangle wave does not solely enter the R2R ladder
        }
    }
}

void pinOutput(int i){

    int bit0 = i & BIT0;			// Mask the bits of input into corresponding i values
    int bit1 = i & BIT1;
    int bit2 = i & BIT2;
    int bit3 = i & BIT3;
    int bit4 = i & BIT4;
    int bit5 = i & BIT5;
    int bit6 = i & BIT6;
    int bit7 = i & BIT7;

    if (bit0) { P6OUT |= BIT0; }					//Each bit# integer corresponds to the current bit of each input and is sent to its own pin. 
    else { P6OUT &= ~BIT0;                          //Turn each pin off if integer does not correspond.
    }
    if (bit1) { P6OUT |= BIT1; }
    else { P6OUT &= ~BIT1;
    }
    if (bit2) { P6OUT |= BIT2; }
    else { P6OUT &= ~BIT2;
    }
    if (bit3) { P6OUT |= BIT3; }
    else { P6OUT &= ~BIT3;
    }
    if (bit4) { P6OUT |= BIT4; }
    else { P6OUT &= ~BIT4;
    }
    if (bit5) { P7OUT |= BIT0; }
    else { P7OUT &= ~BIT0;
    }
    if (bit6) { P3OUT |= BIT6; }
    else { P3OUT &= ~BIT6;
    }
    if (bit7) { P3OUT |= BIT5; }
    else { P3OUT &= ~BIT5;
    }
}

