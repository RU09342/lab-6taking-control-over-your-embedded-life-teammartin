# High Power Control: MOSFET Switch and MOSFET-to-Relay Switch

This program is loaded onto the MSP430G2553 to create a simple TimerA0 frequency controller by using a 5 Kohm potentiometer. The ADc value is set equal to both TA0CCR0 and TA0CCR1 registers of TimerA0, thus changing the frequency and the duty cycle together, ensuring that the duty cycle remains at 50%. This generates a steady and changeable square wave (rectangular pulse train). Three if-statements also control the onboard LEDs, determined by three conditions waiting for the ADC value to match them. Using the potentiometer, both switches are tested with oscilloscope probes to see how high of a frequency both can withstand before distortion occurs. The MOSFET switch can handle up to 200 Hz before distinct rounded sides appear. The MOSFET-to-Relay switch, however, can only handle around 145-150 Hz before severe warping and bending of the outputting signal occurs (long rising signal slopes caused by the coil taking too much time to gather enough current to flip the internal switch). The CQ1-12V relay coil requires at minimum a 53.3 mA current, where the current coming out of the MSP430G2553's PWM output pin is boosted by one or even two n-channel MOSFETS (darlington pair if more current is required). In this case, a single 2N7000 NMOS is able to provide enough current. The maximum current that the relay can draw in is rated for 20A, thus the implementation of the circuit from the corresponding schematic is crucial to ensure that back EMF does not occur and immediately fries the MSP430 board (the flyback diode adjacent to the relay may not be enough discrete components to prevent back EMF, depending on the device attached to the relay). This MOSFET alone requires almost zero current to route the source voltage (GND) to the drain circuit (and to Vcc); only a sufficient voltage needs to be present. According to the datasheet, the 2N7000 can handle up to 200 mA of continuous current or 500 mA of pulsed current, but it does require at minimum a current of 75 mA to operate. In summary, the MOSFET switch can handle small battery-operated functions and can withstand a higher DC frequency input, along with less distortion near its peak frequency. The MOSFET-to-Relay switch can handle very large devices or networks of devices with power-based functions, but falter at lower frequencies with more noise being outputted.

### Installing on MSP430G2553

The provided schematics are recreated onto the breadboard before loading the coded program. The gate of the n-channel MOSFET and the parallel-wired pull-down resistor are wired to the PWM output pin P1.2. The ADC pin P1.3 is wired to the middle pin of the potentiometer. One end-pin of the potentiometer goes to Vcc and the other goes to GND, these two pins can be wired to either one. The flat-head screw on the top of the potentiometer can be twisted with a screwdriver to increase or decrease the ADC value, and subsequently the PWM frequency output. The two output of the schematic circuits are wired to an oscilloscope and voltometer to test for the maximum frequency inputs of both circuits. 

Pins used:

```
P1.2 = PWM varying frequency output pin. constant 50% duty cycle.
P1.3 = ADC input pin from middle pin of potentiometer.
```

### Code-Breakdown

Below are concise descriptions for each of the listed "void" functions within the code:

```
int main(void); = Watchdog timer is turned off. The PWM and GPIO pins are initialized. Timer0_A0 is initialized for generating a 300 Hz output on startup with a 0% duty cycle. The ADC10 peripherals are initialized, setting channel A3 or P1.3 as the ADC input pin. In the for-loop, the sampling and conversion for the next ADC value is started, and low power mode will be enabled with interrupts enabled after the loop iteration ends. Three lines with while-loops slow down the loop to let the conversion finish and to prevent the LEDs from flickering. Three if-statements control which onboard LEDs will illuminate, for confirmation that the potentiometer is either currently increasing or decreasing the ADC. The TA0CCR0 and TA0CCR1 timer registers are both changed constantly and are unaffected in between if-statements, for varying the outputting frequency.
```

```
__interrupt void ADC10_ISR(void) = The low-power mode bit is cleared and low-power mode is set to exit after the interrupt ends. The interrupt block only serves to allow for the ADC to convert successfully into a value that can be read from the ADC10MEM register/variable.
```

### Software Tools Used

* [Code Composer Studio](https://dev.ti.com/) - Code compiling program (CCS). 
