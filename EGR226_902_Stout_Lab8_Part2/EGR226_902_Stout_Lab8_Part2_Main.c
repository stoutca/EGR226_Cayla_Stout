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
void timerAInit(int, int); //function prototype to initialize the Timer A modules
int setPeriod(int);
int setDutyCycle(float,int);
//void setMotorSpeed(void);
void stopMotor(void);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    SysTick_Init(); //initialize the SysTick timer
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

/****| timerA | **********************************************
 * Brief: This function initializes the timer A module
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
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // use SMCLK
            TIMER_A_CTL_MC__UP | // in Up mode
            TIMER_A_CTL_CLR; // Clear TAR to start
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
    int period = (float)(((1/(float)frequency))/(0.000000333)); //gets the period into seconds

    return period;
}

int setDutyCycle(float DC, int P)
{
    int dCycle = (int)((float)P * DC);
    return dCycle;
}

void stopMotor(void)
{
    TIMER_A0->CCR[0] = 0; // set the motor to 0
}


