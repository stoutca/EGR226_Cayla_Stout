/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/12/2021
Project: Lab 8 Part 1 Controlling the DC motor using the SysTick timer
Description: This program controls the PWM of a DC motor using
the SysTick timer module and variables
 *************************************************************/

#include <TimerLib.h>
#include "msp.h"

void motorInit(void); //function prototype to initialize the motor pins

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    SysTick_Init(); //initialize the SysTick timer
    motorInit(); //call function to initialize the DC motor

    float dutyCycle = 0;; //variable for controlling the dutyCycle of the motor PWM, which is the factor that affects its spee.
    //default duty cycle starts as 0, can be modified
    float T = 25; //variable for the period of the PWM. Since the frequency of the motor runs best between 30-50 Hz, 40 Hz was chosen for the frequency and the period was calculated to be 1/40 = 0.025 seconds, or 25 ms



    while(1)
    {
        float Ton = (dutyCycle/100) * T; //convert the duty cycle to a percent and multiply it by the period to get the on period
        float Toff = T-Ton; //variable for the off time of the pin

        if(Ton != 0) //if the motor is supposed to be on
            P6->OUT |= BIT0;//turn on the pin (set it high)

        //wait for the duration of the ON time using SysTick
        delay_ms(Ton);

        if(Toff != 0) //if the motor doesn't shut off
            P6->OUT &= ~BIT0; //turn off the pin (set it low)

        //wait for the remaining off time
        delay_ms(Toff);

    }
}

/****| motorInit | ************************************
 * Brief: This function initializes GPIO pin for the
 * DC motor. The DC motor is connected to P6.0
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void motorInit(void)
{
    P6->SEL0 &= ~BIT0; //configure P6.0 has GPIO
    P6->SEL1 &= ~BIT0;
    P6->DIR |= BIT0; //set P6.0 as an output
    P6->OUT &= ~BIT0; //start the motor low
}
