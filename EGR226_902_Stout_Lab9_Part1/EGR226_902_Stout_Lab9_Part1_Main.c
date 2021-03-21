/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/17/2021
Project: Lab 9 Part 1 DC Motor Speed Program
Description: This program will increment or decrement the PWM
speed of a DC motor by 10% using 2 pushbuttons, and one pushbutton
will stop the motor entirely. The motor is attached to the Timer
A0.1 pin, P2.4, and the pushbuttons are connected to P3.2, P3.3,
and P6.1
 *************************************************************/

#include "msp.h"
#include <PushbuttonLib.h> //include the pushbutton library
#include <TimerLib.h> //include the timer library

void motorPinSetup(void);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    SysTick_Init(); //initialize the Systick timer
    InitializeButtons(); //initialize the pushbuttons
    motorPinSetup(); //initialize DC motor pin 2.4

    //variables for timer A pwm motor speed
    int period = setPeriod(40); //set the desired period based on a given frequency of PWM, which in this case, is 40 Hz
    float dutyCycleFactor = 0; //initialize the percent duty cycle as 0 to begin the program
    int dutyCycle = setDutyCycle(dutyCycleFactor, period); //calculate the value of the duty cycle based off of the calculated period and save it to the duty cycle variable

    timerAInit(period, dutyCycle); //initialize the timer A0.1 module with the calculated period and duty cycle

    int incrementFlag = 0; //flag to keep track of if pressing button 1 incremented the speed once per press
    int decrementFlag = 0; //flag to keep track of if pressing button 2 decremented the speed once per press

    while(1)
    {
        if(DebouncePress1()) //if button 1, the increment speed button, is pressed, as read from the debouncing function, wait for the button to be released and execute code
        {
            while(DebounceRelease1() == 1) //while button 1 is not released, execute the following code:
            {
                if((dutyCycleFactor >= 0) && (dutyCycleFactor <= 0.9) && (incrementFlag == 0)) //if the percent duty cycle is between 0 and 0.9, increase the duty cycle percent by 10%, or 0.1
                {
                    //this code caps out at a duty cycle percentage of 100%, or 1, as the on time can not be greater than the total period
                    dutyCycleFactor = dutyCycleFactor + 0.1;
                    dutyCycle = setDutyCycle(dutyCycleFactor, period); //recalculate the duty cycle based on the new percentage of on time to off time
                    timerAInit(period, dutyCycle); //set the new duty cycle in the timer A module
                    incrementFlag = 1; //set the flag so the duty cycle won't continue to increase during the few milliseconds the button is held down for
                }
            } //if the duty cycle percentage value is at 1, then the duty cycle cannot be updated further until the speed is reduced.
            incrementFlag = 0; //reset the increment flag once the loop is left
        }


        else if(DebouncePress2()) //if button 2, the decrement speed button, is pressed, wait for its release and execute the decrement speed code
        {
            while(DebounceRelease2() == 1) //while button 2 is not released, execute the following code:
            {
                if((dutyCycleFactor >= 0.1) && (dutyCycleFactor <= 1) && (decrementFlag == 0)) //if the value of the percent duty cycle is between 0.1 and 1, decrement the value of the percent duty cycle
                {
                    //this code bottoms out at a duty cycle percentage of 0%, or 0, as the on-time cannot be negative. Once the motor is stopped, its speed cannot be less than stopped.
                    dutyCycleFactor = dutyCycleFactor - 0.1; //decrement the value
                    dutyCycle = setDutyCycle(dutyCycleFactor, period); //recalculate the duty cycle based on the new percentage of on to off time
                    timerAInit(period, dutyCycle); //set the new duty cycle in the timer A module
                    decrementFlag = 1; //trip the decrementFlag so the speed won't continue to decrease after decreasing once while the button is held down
                }
            } //if the duty cycle percentage is at 0, then the duty cycle cannot be slowed down past stopped, so nothing further can happen when button 2 is pushed until the motor is sped up
            decrementFlag = 0; //reset the decrement flag after the loop has been executed once
        }

        else if(DebouncePress3()) //if button 3, or the stop button, is pressed
        {
            while(DebounceRelease3() == 1) //while button 3 is not released, execute the following code:
            {
                dutyCycleFactor = 0.0; //this block of code stops the motor, so set the percent duty cycle to 0
                dutyCycle = setDutyCycle(dutyCycleFactor, period); //calculate the new duty cycle, which should come out to 0
                timerAInit(period, dutyCycle); //stop the motor by setting the CCR[1] register to 0
            }
        }
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
    P2->SEL0 |= BIT4; //configure P2.4 to use Timer A0
    P2->SEL1 &= ~(BIT4);
    P2->DIR |= BIT4; // P2.4 set TA0.1
}
