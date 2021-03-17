/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/12/2021
Project: Lab 8 Part 2 Controlling the Speed of a DC Motor using
TimerA in PWM Mode
Description: This program controls the PWM of a DC motor using
the the Timer A module and variables
 *************************************************************/

#include <TimerLib.h>
#include "msp.h"

void motorPinSetup(void); //function prototype to initialize the motor pins


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    motorPinSetup(); //setup the the motor GPIO pins

    int period = setPeriod(40); //send the frequency to the function to find the period
    float dC = 1; //variable for controlling the dutyCycle of the motor PWM, which is the factor that affects its spee.
    int dutyCycle = setDutyCycle(dC, period);
    timerAInit(period, dutyCycle);

    while(1)
    {
        dutyCycle = setDutyCycle(dC, period);
        TIMER_A0->CCR[1] = dutyCycle; // CCR1 PWM duty cycle
    }
}

/****| motorPinSetup | **************************************
 * Brief: This function initializes the GPIO for the timerA pin
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void motorPinSetup(void)
{
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~(BIT4);
    P2->DIR |= BIT4; // P2.4 set TA0.1
}










