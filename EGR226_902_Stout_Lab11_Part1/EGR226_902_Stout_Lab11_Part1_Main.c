/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/31/2021
Project: Lab 11 Part 1 IR LED Emitter at 10 Hz Frequency
Description: This program will output a 10 Hz PWM that is
emitted by the IR LED
 *************************************************************/

#include "msp.h"
#include <TimerLib.h> //include the libraries for the Systick timer and Timer A

void IRLED_pin(void); //prototype for function to set pins for IR LED

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    SysTick_Init(); //initialize the systick timer
    IRLED_pin(); //initialize the IR LED pin

    //code to create 10 Hz PWM for IR LED
    int period = setPeriod(10); //send the frequency to the function to find the period
    float dC = 0.5; //variable for controlling the dutyCycle of the motor PWM, which is the factor that affects its spee.
    int dutyCycle = setDutyCycle(dC, period);
    timerAInit(period, dutyCycle);

    while(1)
    {

    }


}

/****| IRLED_pin | **********************************************
 * Brief: This function initializes the IR LED on P2.4, connecting
 * the timer A module 0.1 to supply PWM to the LED
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void IRLED_pin()
{
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~(BIT4);
    P2->DIR |= BIT4; // P2.4 set TA0.1
}
