/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 2/10/2021
Project: Library for Initializing Pushbutton 1 and the three
RGB LEDs on the MSP432
Description: This .c file contains the function definitions 
to initialize the first push button on P1.1 and the RBG
LEDs on P2.0, P2.1, and P2.2
*************************************************************/

#include "msp.h"

/****| InitializeLEDs | *************************************
* Brief: This function initializes the LEDs and their respective
* ports to outputs.
* param:
* N/A
* return:
* N/A
*************************************************************/

void InitializeLEDs(void)
{
    P2->SEL0 &= ~BIT0; //Configure P2.0 as simple I/O
    P2->SEL1 &= ~BIT0;
    P2->DIR |= BIT0; //set P2.0 as output pin

    P2->SEL0 &= ~BIT1; //configure P2.1 as simple I/O
    P2->SEL1 &= ~BIT1;
    P2->DIR |= BIT1; //set P2.1 as output pin

    P2->SEL0 &= ~BIT2; //configure P2.2 as simple I/0
    P2->SEL1 &= ~BIT2;
    P2->DIR |= BIT2; //set P2.2 as output pin

    return;
}

/****| InitializeButtons | **********************************
* Brief: This function initializes the button on P1.1 and its
* respective port to inputs and enables the pull up resistor
* built into the MSP.
* param:
* N/A
* return:
* N/A
*************************************************************/

void InitializeButtons(void)
{
    P1->SEL0 &= ~BIT1; //Configure P1.1 as I/O
    P1->SEL1 &= ~BIT1;
    P1->DIR &= ~BIT1; //set P1.1 as input
    P1->REN |= BIT1; //enable pull resistor on P1.1
    P1->OUT |= BIT1; //set pull up resistor on P1.1

    return;
}
