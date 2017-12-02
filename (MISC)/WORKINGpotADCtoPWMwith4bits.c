/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 * 
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP430F552x Demo - ADC12, Sample A0, Set P1.0 if A0 > 0.5*AVcc
//
//   Description: A single sample is made on A0 with reference to AVcc.
//   Software sets ADC12SC to start sample and conversion - ADC12SC
//   automatically cleared at EOC. ADC12 internal oscillator times sample (16x)
//   and conversion. In Mainloop MSP430 waits in LPM0 to save power until ADC12
//   conversion complete, ADC12_ISR will force exit from LPM0 in Mainloop on
//   reti. If A0 > 0.5*AVcc, P1.0 set, else reset.
//
//                MSP430F552x
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//     Vin -->|P6.0/CB0/A0  P1.0|--> LED
//
//   Bhargavi Nisarga
//   Texas Instruments Inc.
//   April 2009
//   Built with CCSv4 and IAR Embedded Workbench Version: 4.21
//******************************************************************************

#include <msp430f5529.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  ADC12CTL0 = ADC12SHT02 + ADC12ON;         // Sampling time, ADC12 on
  ADC12CTL1 = ADC12SHP;                     // Use sampling timer
  ADC12IE = 0x01;                           // Enable interrupt
  ADC12CTL0 |= ADC12ENC;
  P6SEL |= 0x01;                            // P6.0 ADC option select
  P1DIR |= 0x01 + BIT5 + BIT4 + BIT3 + BIT2;                            // P1.0 output, GPIO outs
  P2DIR |= BIT0; //turn on LED wired to UART
  P2SEL |= BIT0; //change P3SEL to P3SEL0
  TA1CCR0 = 4096; //PWM period
  TA1CCTL1 = OUTMOD_7; //CCR1 set/reset
  TA1CCR1 = ADC12MEM0; //PWM starting duty cycle. TA0CCR1 / TA0CCR0 = 0.5 = 50% duty cycle
  TA1CTL = TASSEL_2 + MC_1; //up-counter mode.

  while (1)
  {
    ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    
    __bis_SR_register(LPM0_bits + GIE);     // LPM0, ADC12_ISR will force exit
    __no_operation();                       // For debugger
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(ADC12IV,34))
  {
  case  6:                                  // Vector  6:  ADC12IFG0
    if ((ADC12MEM0 >= 255) & (ADC12MEM0 < 510)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT &= ~BIT5; P1OUT &= ~BIT4; P1OUT &= ~BIT3; P1OUT |= BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 510) & (ADC12MEM0 < 765)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT &= ~BIT5; P1OUT &= ~BIT4; P1OUT |= BIT3; P1OUT &= ~BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 765) & (ADC12MEM0 < 1020)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT &= ~BIT5; P1OUT &= ~BIT4; P1OUT |= BIT3; P1OUT |= BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 1020) & (ADC12MEM0 < 1275)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT &= ~BIT5; P1OUT |= BIT4; P1OUT &= ~BIT3; P1OUT &= ~BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 1275) & (ADC12MEM0 < 1530)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT &= ~BIT5; P1OUT |= BIT4; P1OUT &= ~BIT3; P1OUT |= BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 1530) & (ADC12MEM0 < 1785)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT &= ~BIT5; P1OUT |= BIT4; P1OUT |= BIT3; P1OUT &= ~BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 1785) & (ADC12MEM0 < 2040)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT &= ~BIT5; P1OUT |= BIT4; P1OUT |= BIT3; P1OUT |= BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 2040) & (ADC12MEM0 < 2295)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT |= BIT5; P1OUT &= ~BIT4; P1OUT &= ~BIT3; P1OUT &= ~BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 2295) & (ADC12MEM0 < 2550)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT |= BIT5; P1OUT &= ~BIT4; P1OUT &= ~BIT3; P1OUT |= BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 2550) & (ADC12MEM0 < 2805)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT |= BIT5; P1OUT &= ~BIT4; P1OUT |= BIT3; P1OUT &= ~BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 2805) & (ADC12MEM0 < 3060)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT |= BIT5; P1OUT &= ~BIT4; P1OUT |= BIT3; P1OUT |= BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 3060) & (ADC12MEM0 < 3315)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT |= BIT5; P1OUT |= BIT4; P1OUT &= ~BIT3; P1OUT &= ~BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 3315) & (ADC12MEM0 < 3570)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT |= BIT5; P1OUT |= BIT4; P1OUT &= ~BIT3; P1OUT |= BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 3570) & (ADC12MEM0 < 3825)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT |= BIT5; P1OUT |= BIT4; P1OUT |= BIT3; P1OUT &= ~BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    if ((ADC12MEM0 >= 3825) & (ADC12MEM0 < 4080)) {                // ADC12MEM = A0 > 0.5AVcc?
      P1OUT |= BIT5; P1OUT |= BIT4; P1OUT |= BIT3; P1OUT |= BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    else {
      P1OUT &= ~BIT5; P1OUT &= ~BIT4; P1OUT &= ~BIT3; P1OUT &= ~BIT2;
      TA1CCR1 = ADC12MEM0;
    }
    __bic_SR_register_on_exit(LPM0_bits);   // Exit active CPU
  default: break; 
  }
}
