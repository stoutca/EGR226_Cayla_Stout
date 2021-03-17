/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/12/2021
Project: Lab 8 Part 3 Generating a PWM signal by reading the
Duty Cycle from a keypad
Description: This program controls the PWM of a DC motor using
the the Timer A module and variables
 *************************************************************/

#include <TimerLib.h>
#include "msp.h"

void motorPinSetup(void); //function prototype to initialize the motor pins
uint8_t Read_Keypad(void);
void keypad_init(void);

uint8_t num, pressed; //Global variables for storing the number that was pressed on the keypad and whether or not the keypad had been pressed


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    SysTick_Init(); //initialize the SysTick timer
    keypad_init();
    motorPinSetup(); //setup the the motor GPIO pins

    int period = setPeriod(40); //send the frequency to the function to find the period
    float dC = 0; //variable for controlling the dutyCycle of the motor PWM, which is the factor that affects its spee.
    int dutyCycle = setDutyCycle(dC, period);
    timerAInit(period, dutyCycle);

    while(1)
    {
        if(pressed = Read_Keypad()) //if the keypad was pressed
        {
            if((num != 10) && (num != 12) && (num != 11)) //if no special character * or # was pressed or 0 wasn't pressed
            {
                dC = (float)num/10; //turn the pressed number into a percentage for the duty cycle
                dutyCycle = setDutyCycle(dC, period);
                timerAInit(period, dutyCycle);
            }
            else if(num == 11) //if 0 was entered
            {
                TIMER_A0->CCR[1] = 0; // CCR1 PWM duty cycle
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
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~(BIT4);
    P2->DIR |= BIT4; // P2.4 set TA0.1
}

/****| keypad_init | *****************************************
 * Brief: This function intitializes the keypad.  Port 5 4,5,6,7 are
 * connected to the rows 1, 2, 3, 4, Port 3 5,6,7 are connected
 * to the columns 1, 2, 3 of the keypad.
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void keypad_init (void)
{
    //Rows of keypad-------------------------------------------------
    //Row 1
    P5->SEL0 &= ~BIT4;
    P5->SEL1 &= ~BIT4; //set P5.4 as GPIO
    P5->DIR &= ~BIT4; //set up P5.4 as input
    P5->REN |= BIT4; //enable internal resistors on MSP
    P5->OUT |= BIT4; //enable pullup resistor

    //Row 2
    P5->SEL0 &= ~BIT5;
    P5->SEL1 &= ~BIT5; //set P5.5 as GPIO
    P5->DIR &= ~BIT5; //set up P5.5 as input
    P5->REN |= BIT5; //enable internal resistors on MSP
    P5->OUT |= BIT5; //enable pullup resistor

    //Row 3
    P5->SEL0 &= ~BIT6;
    P5->SEL1 &= ~BIT6; //set P5.6 as GPIO
    P5->DIR &= ~BIT6; //set up P5.6 as input
    P5->REN |= BIT6; //enable internal resistors on MSP
    P5->OUT |= BIT6; //enable pullup resistor

    //Row 4
    P5->SEL0 &= ~BIT7;
    P5->SEL1 &= ~BIT7; //set P5.7 as GPIO
    P5->DIR &= ~BIT7; //set up P5.7 as input
    P5->REN |= BIT7; //enable internal resistors on MSP
    P5->OUT |= BIT7; //enable pullup resistor

    //Columns of keypad---------------------------------------------------
    //Column 1
    P3->SEL0 &= ~BIT5;
    P3->SEL1 &= ~BIT5; //set P3.5 as GPIO

    //Column 2
    P3->SEL0 &= ~BIT6;
    P3->SEL1 &= ~BIT6; //set P3.6 as GPIO

    //Column 3
    P3->SEL0 &= ~BIT7;
    P3->SEL1 &= ~BIT7; //set P3.7 as GPIO
}

/****| Read_Keypad | *****************************************
 * Brief: This function reads input presses from the keypad.
 * param:
 * N/A
 * return:
 * uint8_t variable that tells the program if a key has been pressed
 * on the keypad
 *************************************************************/

uint8_t Read_Keypad(void)
{
    uint8_t col, row;
    for (col = 0; col < 3; col++ )
    {
        P3->DIR = 0x00; // Set Columns to inputs
        P3->DIR |= BIT(5+col); // Set column to output
        P3->OUT &= ~BIT(5+col); // Set column to LOW
        delay_ms(10); // Delay the while loop
        row = P5->IN & 0xF0; // read all rows
        while (!(P5IN & BIT4) | !(P5IN & BIT5) | !(P5IN & BIT6) | !( P5IN & BIT7) );

        if (row != 0xF0)
            break; // if one of the input is low, some key is pressed.
    }

    P3->DIR = 0x00; // Set Columns to inputs
    if ( col == 3)
        return 0;
    if (row == 0xE0)
        num = col + 1; // key in row 0
    if (row == 0xD0)
        num = 3 + col +1; // key in row 1
    if (row == 0xB0)
        num = 6 + col +1; // key in row 2
    if (row == 0x70)
        num = 9 + col+1; // key in row 3
    return 1;
}


