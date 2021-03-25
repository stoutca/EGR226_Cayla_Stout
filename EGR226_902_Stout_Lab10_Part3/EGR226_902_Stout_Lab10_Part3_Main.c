/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/24/2021
Project: Lab 10 Part 3 Temperature Display Using ADC on the LCD
Description: This program will display the changing temperature
read from the temperature sensor using the ADC converter located
within the MSP432 on the LCD
 *************************************************************/

#include "msp.h"
#include <string.h> //include the string library
#include <stdio.h>
#include <TimerLibInterrupts.h> //include timer interrupt library
#include <LCD_lib.h> //include the custom LCD library

volatile uint32_t timeFlag;

void ADCSetup(void);

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	SysTick_Init(); //initialize the systick timer
    ADCSetup(); //initialize the ADC converter on P5.5
    LCDPinInit(); //initialize the pins connected to the LCD
    LCD_init(); //start up the LCD

    float Vin; //variable for holding the value of the voltage initially read from the ADC converter
    float tempC; //variable for holding the temperature read by the sensor in celcius

    char temperatureC[50]; //string to hold the value of the temperature
    char currentTempString[] = "Current Temp. is"; //string variable to store data to be printed on first line of LCD
    __enable_irq(); //enable global interrupts
    delay_ms(60); //delay 60 milliseconds before first command

    //move cursor to home position and clear the LCD
    commandWrite(0x01); //clear display and set cursor to home
    delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position
    printString(currentTempString); //print the opening string on the first line of the LCD
    delay_ms(500); //delay 500 milliseconds

    while(1)
    {

        if(timeFlag) //if the sampling interrupt from systick counted down half a second
        {
            ADC14->CTL0 |=1; //start a conversion
            while(!ADC14->IFGR0); // wait until conversion complete
            Vin = ADC14->MEM[5]; // immediately store value in a variable
            tempC = ((Vin * .2)-500)/10; //convert the result voltage into temperature in celcius, from the formula tempC = (Vin * .2)-500 all /10
            sprintf(temperatureC, "      %0.1f", tempC); //save the results of the temperature to the third decimal place to the string temperatureC
            timeFlag = 0; //reset flag triggered by interrupt
            //move cursor to beginning of line 2 of LCD
            commandWrite(0xC0);
            delay_ms(5);
            //print the temperature to the LCD at line 2
            printString(temperatureC);
            //print the degree symbol
            dataWrite(0xDF);
            delay_ms(500); //delay 500 milliseconds to allow the LCD to print the needed string
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

