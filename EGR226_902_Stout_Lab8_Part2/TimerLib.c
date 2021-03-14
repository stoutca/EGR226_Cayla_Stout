/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/3/2021
Project: Library for Timer Functions
RGB LEDs on the MSP432
Description: This .h file contains the function prototypes
for the SysTick initialization and functions to delay the
program by millisecond or microsecond increments
*************************************************************/

#include <TimerLib.h> //include timer library
#include "msp.h" //include msp library

/****| SysTick_Init | ************************************
 * Brief: This function initializes the function to
 * delay the program using the SysTick
 * system timer.
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void SysTick_Init (void)
{ //initialization of systic timer
    SysTick -> CTRL = 0; // disable SysTick During step
    SysTick -> LOAD = 0x00FFFFFF; // max reload value
    SysTick -> VAL = 0; // any write to current clears it
    SysTick -> CTRL = 0x00000005; // enable systic, 3MHz, No Interrupts
}
//code from EGR 226 Systick Lecture Slide

/****| delay_ms | ************************************
 * Brief: This function delays the program for 1 millisecond
 * using the SysTick
 * clock.
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void delay_ms (unsigned ms)
{ // Systick delay function
    SysTick -> LOAD = ((ms * 3000) - 1); //delay for 1 millisecond per delay value
    SysTick -> VAL = 0; // any write to CVR clears it
    while((SysTick->CTRL & BIT(16)) == 0); // wait for flag to be SET
}
//code from EGR 226 Systick Lecture Slide

/****| delay_micro | ************************************
 * Brief: This function delays the program for a
 * certain amount of microseconds using the SysTick
 * clock.
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void delay_micro (unsigned microsec)
{ // Systick delay function
    SysTick -> LOAD = (microsec*3 - 1); // delay*3 for 1 microsecond
    SysTick -> VAL = 0; //clears counter
    while((SysTick -> CTRL & BIT(16)) == 0); //wait for COUNT flag to be set
}
//code from EGR 226 Section 10 keypad lecture slides







