/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/24/2021
Project: Lab 10 Part 1 Potentiometer Display Using ADC
Description: This program will display the changing voltage
from the potentiometer using the built-in MSP432 ADC Converter
 *************************************************************/

#include "msp.h"
#include <TimerLib.h> //include interrupt timer library
#include <stdio.h>

void ADCSetup(void); //function prototype to set up ADC on P5.5

void main(void)
{
    float result; //variable to hold result from potentiometer moving

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    SysTick_Init(); //initialize the SysTick timer with interrupts
    ADCSetup(); //initialize the ADC on P5.5

    while(1)
    {
        ADC14->CTL0 |=1; //start a conversion
        while(!ADC14->IFGR0); // wait until conversion complete
        result = ADC14->MEM[5]; // immediately store value in a variable
        printf("Result is:\n\t%g\n", result);
        result = result * .0002; //convert the result into a voltage, from the formula Vin = result*(3.3V)/(2^resolution)
        printf("Voltage is:\n\t%g\n", result); //print the result to the console
        delay_ms(500); //delay 500 milliseconds
    }

}
//code from lecture 15 of EGR 226

/****| ADCSetup | ********************************************
 * Brief: This function initializes the ADC converter located on
 * P5.5 of the MSP
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void ADCSetup(void)
{
    ADC14->CTL0 = 0x00000010; //power on and disabled during configuration
    ADC14->CTL0 |= 0x04D80300; // S/H pulse mode, MCLCK, 32 sample clocks,
    //sw trigger, /4 clock divide
    ADC14->CTL1 = 0x00000030; // 14-bit resolution
    ADC14->MCTL[5] = 0; // A0 input, single ended, vref=avcc
    P5->SEL1 |= 0x20; // configure pin 5.5 for ADC use
    P5->SEL0 |= 0x20;
    ADC14->CTL1 |= 0x00050000; //start converting at mem reg 5
    ADC14->CTL0 |= 2; //enable ADC after configuration
}
