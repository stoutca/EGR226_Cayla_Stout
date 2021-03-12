/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/3/2021
Project: Lab 7 Part 1 Interfacing the MSP432 with a Liquid Crystal Display (LCD)
Description: This program was a practice in correctly initializing
the LCD so that the cursor blinked in the home position.
 *************************************************************/

#include "msp.h"
#include "LCD_Lib.h" //include custom LCD library
#include "TimerLib.h" //include timer library for delay functions

void main(void)
 {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    SysTick_Init();
    LCDPinInit(); //call function to initialize the pins to the LED

    LCD_init(); //call function to initialize the LCD

    while(1)
    {

    }

}






