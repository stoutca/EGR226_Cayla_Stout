/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/31/2021
Project: Lab 11 Part 2 IR LED Emitter at 10 Hz Frequency
Description: This program will output a 10 Hz PWM that is
emitted by the IR LED, and the receiving photo diode will
pick up that frequency. If the photodiode is receiving light,
the onboard red LED will turn on. If something comes between
the emitter and receiver, the red LED will turn off.
 *************************************************************/

#include "msp.h"
#include "TimerLib.h" //include the libraries for the Systick timer and Timer A

void IRLED_pin(void); //prototype for function to set pins for IR LED
void photodiode_pin(void); //prototype for function to set pins for photodiode

volatile uint32_t triggered, currentedge; //global flag variables to tell program if timerA0 input capture interrupt happened


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    SysTick_Init(); //initialize the systick timer
    IRLED_pin(); //initialize the IR LED pin
    photodiode_pin(); //initialize the photodiode pin


    //code to create 10 Hz PWM for IR LED
    int period = setPeriod(10); //send the frequency to the function to find the period
    float dC = 0.5; //variable for controlling the dutyCycle of the motor PWM, which is the factor that affects its spee.
    int dutyCycle = setDutyCycle(dC, period);
    timerAInit(period, dutyCycle); //set TA0.1 to pwm
    timerA2Init(period); //set TA2.2 to input capture for 10 Hz
    NVIC_EnableIRQ(TA2_N_IRQn); //enable NVIC interrupts for TA2.2

    uint16_t lastedge, P;

    //configure red onboard LED
    P2->SEL0 &= ~BIT0; //Configure P2.0 as simple I/O
    P2->SEL1 &= ~BIT0;
    P2->DIR |= BIT0; //set P2.0 as output pin
    P2->OUT &= ~BIT0; //start with the red LED off

    __enable_irq(); //enable all interrupts

    while(1)
    {
        if(triggered)
        {
            TIMER_A2->CCTL[2] &= ~1; // Clear the TA2.2 interrupt flag
            P = currentedge - lastedge;
            lastedge = currentedge;
            if((35635 < P) && (P < 39375)) // within 5% of 10Hz period
                P2->OUT |= BIT0; //turn on LED if the interrupt occurred and the capture was detected
            else
            {
                P2->OUT &= ~BIT0; //turn off LED if the interrupt did not occur and the receiver did not read the emitter
            }
            triggered = 0;
        }

        else if((triggered == 0))
        {
            delay(1);
            if(triggered == 0)
                P2->OUT &= ~BIT0; //turn off LED if the interrupt did not occur and the receiver did not read the emitter
        }


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


/****| photodiode_pin | **********************************************
 * Brief: This function initializes the photodiode on P5.7, connecting
 * the timer A module 2.2 to receive frequencies of 10 Hz
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void photodiode_pin()
{
    P5->SEL0 |= BIT7;
    P5->SEL1 &= ~BIT7;
    P5->DIR &= ~BIT7; //set P5.7 as input
    P5->REN |= BIT7; //enable pull resistor on P5.7
    P5->OUT |= BIT7; //set pull up resistor on P5.7

}



void TA2_N_IRQHandler(void) // Timer A0 CCR2 interrupt service routine
{
    currentedge = TIMER_A2->CCR[2];
    TIMER_A2->CCTL[2] &= ~1; // Clear the TA2.2 interrupt flag
    triggered = 1;

} //code from precise timing lecture of EGR 226 section 10
