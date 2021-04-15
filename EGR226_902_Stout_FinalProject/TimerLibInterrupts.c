/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/3/2021
Project: Library for Timer Functions
Description: This .h file contains the function prototypes
for the SysTick initialization and functions to delay the
program by millisecond or microsecond increments. Timer A
initializations and uses with a DC motor are also included here
*************************************************************/

#include <TimerLibInterrupts.h> //include timer library
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

void SysTick_Init(void)
{ //initialization of systic timer
    SysTick -> CTRL = 0; // disable SysTick During step
    SysTick -> LOAD = 0x00FFFFFF; // max reload value for 0.5s interrupts
    SysTick -> VAL = 0; // any write to current clears it
    SysTick -> CTRL = 0x00000005; // enable systic, 3MHz, Interrupts
}
//code from EGR 226 Systick Lecture Slide

/****| Timer32_Init | ************************************
 * Brief: This function initializes the function to
 * delay the program using the SysTick
 * system timer.
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void Timer32_Init()
{ //initialization of systic timer
    TIMER32_2->CONTROL = 0b11100000; //period mode, with interrupt, no prescaler, 16 bit mode, one shot mode
    TIMER32_2->LOAD = 1501501; //half second on 3 MHz clock
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

/****| timerA0Init | **********************************************
 * Brief: This function initializes the timer A0 module. This instance
 * is mainly used by the LEDs
 * param:
 * integer type variable period
 * return:
 * N/A
 *************************************************************/

void timerA0Init(int T)
{
    TIMER_A0->CCR[0] = T - 1; // PWM Period (# cycles of clock) for 100 Hz
    TIMER_A0->CTL = 0x02D4; // SMCLK selected, 1/8 clock divider, set up mode to count, Clear TAR to start
}

/****| timerA01Init | **********************************************
 * Brief: This function initializes the timer A0.1 module for the red
 * LED
 * param:
 * integer type variable for the variable duty cycle
 * return:
 * N/A
 *************************************************************/

void timerA01Init(int DC)
{
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set mode 7
    TIMER_A0->CCR[1] = DC; // CCR1 PWM duty cycle
}

/****| timerA04Init | **********************************************
 * Brief: This function initializes the timer A0.1 module for brightness
 * of the LCD
 * param:
 * integer type variable for the variable duty cycle
 * return:
 * N/A
 *************************************************************/

void timerA04Init(int DC)
{
    TIMER_A0->CCTL[4] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set mode 7
    TIMER_A0->CCR[4] = DC; // CCR4 PWM duty cycle
}

/****| setPeriod | **********************************************
 * Brief: This function initializes the timer A module
 * param:
 * integer type variable for the variable duty cycle
 * return:
 * N/A
 *************************************************************/
int setPeriod(int frequency)
{
    int period = (float)(((1/(float)frequency))/(0.0000026667)); //gets the period into seconds based on SMCLK frequency - 3MHz/8 = 375kHz = period of 2.6667 microseconds
    return period;
}

/****| setDutyCycle| **********************************************
 * Brief: This function sets the dutyCycle of the timer A module
 * param:
 * float type duty cycle value and int period value
 * return:
 * int value of the duty cycle in counts
 *************************************************************/

int setDutyCycle(float DC, int P)
{
    int dCycle = (int)((float)P * DC); //calculate the value of the duty cycle in counts from the given period in counts
    if(dCycle != 0) //if the duty cycle percentage isn't 0
        dCycle--; //decrement dCycle so it doesn't have a greater value than the period
    return dCycle;
}

/****| stopMotor | **********************************************
 * Brief: This function stops the motor by setting the PWM to 0
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void stopMotor(void)
{
    TIMER_A0->CCR[0] = 0; // set the motor to 0
}

/****| timer2Init | **********************************************
 * Brief: This function initializes the timer A2 module PWM for
 * use in the DC motor and Servo
 * param:
 * integer type variable for the period of a 50 Hz frequency
 * return:
 * N/A
 *************************************************************/

void timerA2Init(int T)
{
    TIMER_A2->CCR[0] = T - 1; // PWM Period (# cycles of clock)
    TIMER_A2->CTL = 0x02D4; // SMCLK selected, 1/8 clock divider, set up mode to count, Clear TAR to start

}

/****| timerA21Init | **********************************************
 * Brief: This function initializes the timer A module as input capture
 * param:
 * integer type variable for the variable duty cycle
 * return:
 * N/A
 *************************************************************/

void timerA21Init(int DC)
{
    TIMER_A2->CCR[1] = DC; // CCR1 PWM duty cycle
    TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set mode 7
}



