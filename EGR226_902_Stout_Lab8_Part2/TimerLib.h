/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/3/2021
Project: Library for Timer Functions Lab 7
RGB LEDs on the MSP432
Description: This .h file contains the function prototypes
for the SysTick initialization and functions to delay the
program by millisecond or microsecond increments
*************************************************************/

#ifndef TIMERLIB_H_
#define TIMERLIB_H_

void delay_ms(unsigned ms); //function to delay the program for a certain number of milliseconds using the Systick timer
void delay_micro(unsigned microsec); //function to delay the program for a certain number of microseconds
void SysTick_Init(void); //function to initialize SysTick timer

#endif /* TIMERLIB_H_ */
