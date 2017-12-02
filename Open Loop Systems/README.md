# Open Loop: Temperature Sensor Fan-Speed Controller

Similar to the RGB LED implementation using PWM signals, the duty cycle of the PWM signal from the MSP430G2553 corresponds to the measured ADC value from the input pin (P1.3). The PWM duty cycle comes from 16-if statements that wait for the ADC value to fit within their conditions, thus stepping the duty cycle 16 times from 0% to 100%.  The MSP430 code allows UART characters to be sent and received from the PC via a program called CoolTerm. The MSP430 will print the ADC values, refreshing at a rapid pace. Single character commands from “a” – “z” and “.” (sets desired temperature back to default, 460) are used to set a desired ADC value from 350 to 600. The UART will also send back character arrays after each command confirming that it has been received, and indicating what the new desired temperature has been changed to. Lastly, four LEDs can be wired to the GPIO pins for illuminating the 4-bit binary counter (binary number placements in same order as stated from pins above) made from the 16 if-statements. 

### Installing on MSP430G2553

A 4-pin computer fan with a PWM input pin is wired with leads to the breadboard, and this fan is given GND and +12V connections. The fan controller schematic is recreated onto a breadboard to be interfaced with the MSP430. The MSP430’s VCC and GND pins are wired to the breadboard, the four GPIO pins (P2.4, P2.3, P2.1, P2.0) are wired to four white LEDs, pin P1.6 is wired to the PWM input pin of the fan, and P1.3 is wired to the output of the op-amp used in this design’s schematic. On the MSP430 board, the two black female jacks on the RX and TX pins must be flipped from vertical to horizontal for the UART to function. The fan is wired to a +12V power supply. It's GND pin is wired to all other grounds. A +20V voltage supply is provided for the LM7805 to provide heat for the sensor. The sensor is placed as close as possible to the LM7805 (no tape or other mounting device, it causes external heat dissipation).

Pins used:

```
P1.3 = ADC input pin from LM35
P1.6 = PWM output to the fan
P2.0 = GPIO LED, bit 0 of the 4-bit counter
P2.1 = GPIO LED, bit 1 of the 4-bit counter
P2.3 = GPIO LED, bit 2 of the 4-bit counter
P2.4 = GPIO LED, bit 3 of the 4-bit counter
```

### Code-Breakdown

Below are concise descriptions for each of the listed "void" functions within the code:

```
init_uart(); = initializes all pins and peripherals for creating the UART. TX interrupt is enabled.
```

```
init_timer(); = initializes all pins and peripherals for creating a PWM output signal to the fan. The frequency is at 800, and the duty cycle begins at 0.
```

```
main(void); = disable watchdog timer. initialize GPIO, UART, PWM, and ADC10, pins and peripherals. In the for-loop, delay the loop to prevent LED flickering. Sample and convert the voltage value at the ADC pin into a digital value. Set up the 16 different ADC range if-statements for creating the 4-bit LED counter. Within each if-statement, increase the PWM duty cycle by 50 up to 800 (i.e. 0, 100, 150, 200, 250… 750, 800). Go to low power mode and force exit the ADC10 interrupt at the end of the loop.
```

```
__interrupt void ADC10_ISR(void); = clear the low power mode bit, exit it and return to main.
```

```
__interrupt void Timer1_A0(void); = control the speed of the ADC conversions, as well as the refresh rate for the ADC values being printed over UART to the PC. Store latest ADC value into the ADC10SA register address. Enable the TX UART interrupt.
```

```
__interrupt void USCI0TX_ISR(void); = output the current ADC value over UCA0TXBUF in two separate buffer variables. The buffers run through the printing characters one at a time. After both messages have been sent and printed, reset/end the interrupt.
```

### Software Tools Used

* [Code Composer Studio](https://dev.ti.com/) - Code compiling program (CCS). 
* [CoolTerm](http://freeware.the-meiers.org/) - Used to print characters from TX/RX UART to PC