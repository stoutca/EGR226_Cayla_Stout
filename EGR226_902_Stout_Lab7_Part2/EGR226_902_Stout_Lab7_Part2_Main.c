/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/3/2021
Project: Lab 7 Part 2 Interfacing the MSP432 with a Liquid Crystal Display (LCD)
Description: This printed the author's first and last name
to the first and second line, respectively, and the course
number to the third and fourth line.
 *************************************************************/

#include "msp.h"
#include "LCD_Lib.h" //include LCD library file to initialize and run LCD
#include "TimerLib.h" //include timer library


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    SysTick_Init();
    LCDPinInit(); //call function to initialize the pins to the LED

    LCD_init(); //call function to initialize the LCD

    char firstString[] = "     Cayla            EGR"; //first half of text for lines 1 and 3 of LCD
    char secondString[] = "     Stout            226"; //second half of text for lines 2 and 4 of LCD

    //printing the texts to the LCD in order of the lines
    delay_ms(60); //wait for program to fully initialize before sending first command
    printString(firstString); //print the first half of the string (lines 1 and 3 on the LCD)
    commandWrite(0xC0); //skip to the second line, as addresses 20-3F are missing in the addressing
    printString(secondString); //print the second half of the string (lines 2 and 4 on the LCD);

    while(1)
    {

    }
}

