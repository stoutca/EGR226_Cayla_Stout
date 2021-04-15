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

#include <TimerLib2.h> //include timer library
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

/****| timerA | **********************************************
 * Brief: This function initializes the timer A module as PWM
 * param:
 * integer type variable for the variable duty cycle
 * return:
 * N/A
 *************************************************************/

void timerAInit(int T, int DC)
{

    TIMER_A0->CCR[0] = T - 1; // PWM Period (# cycles of clock) for 40 Hz
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set mode 7
    TIMER_A0->CCR[1] = DC; // CCR1 PWM duty cycle
    TIMER_A0->CTL = 0x02D4; // SMCLK selected, 1/8 clock divider, set up mode to count, Clear TAR to start
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


/****| timerA2Init | **********************************************
 * Brief: This function initializes the timer A module as input capture
 * param:
 * integer type variable for the variable duty cycle
 * return:
 * N/A
 *************************************************************/

void timerA2Init(int T)
{
    TIMER_A2->CCR[0] = T - 1; // PWM Period (# cycles of clock)
    TIMER_A2->CCTL[2] = 0x4910; // Capture rising edge, Use CCI2A, Enable capture interrupt, Enable capture mode, Synchronous capture
    TIMER_A2->CTL = 0x02E4; // SMCLK selected, 1/8 clock divider, set continuous to count, enable interrupts, Clear TAR to start
}


