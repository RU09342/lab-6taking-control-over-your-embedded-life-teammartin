# Precision Control: Triangle Wave and FFT, 8-bit Counter to R2R Ladder

This program generates both a triangle wave output and an 8-bit binary counter that feeds into an R2R ladder for generating a staircase wave function. The R2R ladder is created onto a breadboard, following the provided schematic, and the 8 input pins are wired to their respective GPIO output pins to create a circuit that slowly increases in voltage for each binary number until the maximum 8-bit binary number of 255 has been reached, and then incrementally go to the minimum 8-bit binary number of 0 in the same fashion. There is also an op-amp wired to the output of the R2R ladder resistors, which serves to revive the lost current from within the various resistors for being accurately measured by the oscilloscope.  

### Installing on MSP430F5529

If the triangle wave is desired to be extracted from the program, then line 28 (Counter8bit();) is commented out and line 29 (trianglewave();) is uncommented. A wire is connected directly to P2.0 the triangle wave's output pin and fed to an oscilloscope for validation and testing for rising and falling linearities. If the R2R ladder implementation is desired to be extracted, then line 29 is uncommented and line 28 is commented out. The 8 GPIO pins are wired to the R2R ladder input pins that represent the binary bit position in the 8-bit binary number representing the value of 0 to 255 (the i variable). The output is then received out of the op-amp that inputs the output of the R2R ladder's sequence of resistors. A wire is connected directly to to the output of this op-amp and fed to an oscilloscope for validation of a step function / staircase function. 

Pins used:

```
P2.0 = triangle wave output pin
P6.0 = GPIO pin bit 0 of R2R ladder
P6.1 = GPIO pin bit 1 of R2R ladder
P6.2 = GPIO pin bit 2 of R2R ladder
P6.3 = GPIO pin bit 3 of R2R ladder
P6.4 = GPIO pin bit 4 of R2R ladder
P7.0 = GPIO pin bit 5 of R2R ladder
P3.6 = GPIO pin bit 6 of R2R ladder
P3.5 = GPIO pin bit 7 of R2R ladder
```

### Code-Breakdown

Below are concise descriptions for each of the listed "void" functions within the code:

```
int main(void); = Watchdog timer is turned off. The GPIO pins are initialized. Timer1_A1 is initialized for generating a 1000 Hz output on startup with a 50% duty cycle. Depending on which function is commented out, either the Counter8bit function will be executed or the trianglewave function will be executed.
```

```
void Counter8bit(); = Loop counter and loop delay variables i and j are defined. In the while-loop, two for-loops are designed to increment and then decrement the counter by 16 digits per iteration, giving 17*2 = 34 steps for the outputting 8 GPIO pins to output voltages from 0V to almost 3.3V and back down to 0V. The output is received after the R2R output has gone through a current feedback/forwarding op-amp. The pinOutput function is executed within each for-loop and for each i value. The i value (max is 255) is multiplied by 125 and shifted by 5 bits to convert the 0-255 range to a 0-1000 range (255*125>>5 = 996). The TA1CCR1 timer register for duty cycle is then set to the new i value. The for-loop delays here only serve to provide enough time to create a horizontal voltage signal in between each step, mainly for the pinOutput function to complete in the delaying time.
```

```
void trianglewave(); = Loop counter variable i is defined. In the while-loop, two for-loops are designed to increment and then decrement the counter by 1 digit per iteration, giving 256 steps for the outputting triangle wave from P2.0 to output voltages from 0V to almost 3.3V and back down to 0V with a reasonably smooth wave. The output is received directly at the triangle wave output pin. The i value (max is 255) is multiplied by 125 and shifted by 5 bits to convert the 0-255 range to a 0-1000 range (255*125>>5 = 996). The TA1CCR1 timer register for duty cycle is then set to the new i value.
```

```
void pinOutput(); = the 8 separate bit variables are initialized to be equivalent to i, the 8-bit binary number that counts up to 255, and ANDed with the corresponding bit. The 8 if-statements set the GPIO pin to be on if the respective bit variable has a value of 1. Each if-statement has an else-statement turning the GPIO pin off when the respective bit variable goes back to a value of 0.
```

### Software Tools Used

* [Code Composer Studio](https://dev.ti.com/) - Code compiling program (CCS). 
