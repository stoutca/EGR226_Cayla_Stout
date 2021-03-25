/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/24/2021
Project: Lab 10 Part 2 Temperature Display Using ADC
Description: This program will display the changing temperature
read from the temperature sensor using the ADC converter located
within the MSP432
 *************************************************************/

#include "msp.h"
#include <TimerLibInterrupts.h> //include interrupt timer library
#include <stdio.h>

void ADCSetup(void); //function prototype to set up ADC on P5.5

volatile uint32_t timeFlag;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    SysTick_Init(); //initialize the systick timer with interrupts
    ADCSetup(); //initialize the ADC converter on P5.5
    float Vin; //variable for holding the value of the voltage initially read from the ADC converter
    float tempC; //variable for holding the temperature read by the sensor in celcius

    __enable_irq(); //enable global interrupts

    while(1)
    {
        if(timeFlag) //if the sampling interrupt from systick counted down half a second
        {
            ADC14->CTL0 |=1; //start a conversion
            while(!ADC14->IFGR0); // wait until conversion complete
            Vin = ADC14->MEM[5]; // immediately store value in a variable
            tempC = ((Vin * .2)-500)/10; //convert the result voltage into temperature in celcius, from the formula tempC = (Vin * .2)-500 all /10
            printf("Temperature is:\n\t%g\n", tempC); //print the result temperature to the console
            delay_ms(500); //delay 500 milliseconds
            timeFlag = 0; //reset flag triggered by interrupt
        }
    }
}

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

/****| SysTick_Handler | *************************************
 * Brief: This function handles the IRQ for the interrupts in
 * the SysTick timer module
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void SysTick_Handler(void)
{
    timeFlag = 1; //samples will occur twice a second
}
