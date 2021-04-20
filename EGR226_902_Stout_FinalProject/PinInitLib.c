
#include "msp.h"
#include <PinInitLib.h>


/****| LEDPinInit | ********************************************
 * Brief: This function initializes the LEDs and LED LCD backlight
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void LEDPinInit()
{
    //P1.6 red LED
    P1->SEL0 &= ~BIT6; //set as GPIO output
    P1->SEL1 &= ~BIT6;
    P1->DIR |= BIT6;

    //P1.7 green LED
    P1->SEL0 &= ~BIT7; //set as GPIO output
    P1->SEL1 &= ~BIT7;
    P1->DIR |= BIT7;

    //LCD Background PWM pin
    P2->SEL0 |= BIT7;
    P2->SEL1 &= ~BIT7;
    P2->DIR |= BIT7; // P2.4 set TA0.4

    //RBG pin init to PWM
    //Green LED
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~BIT4;
    P2->DIR |= BIT4; // P2.4 set TA0.1

    //Red LED
    P2->SEL0 |= BIT5;
    P2->SEL1 &= ~BIT5;
    P2->DIR |= BIT5; // P2.5 set TA0.2

    //Blue LED
    P2->SEL0 |= BIT6;
    P2->SEL1 &= ~BIT6;
    P2->DIR |= BIT6; // P2.6 set TA0.3
}

/****| motorPinInit | ********************************************
 * Brief: This function initializes the servo and DC motors attached to
 * P5.6 and P5.7
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void motorPinInit()
{
    //DC Motor
    P5->SEL0 |= BIT7;
    P5->SEL1 &= ~BIT7;
    P5->DIR |= BIT7; // P6.6 set TA2.2

    //Servo Motor
    P5->SEL0 |= BIT6;
    P5->SEL1 &= ~(BIT6);
    P5->DIR |= BIT6; // P5.6 set TA2.1
}

/****| PushbuttonPinInit | ********************************************
 * Brief: This function initializes the servo and DC motors attached to
 * P5.6 and P5.7
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/
void PushbuttonPinInit()
{
    P3->SEL0 &= ~BIT2; //set up P3.2 as GPIO, input, pull-up resistor
    P3->SEL1 &= ~BIT2;
    P3->DIR &= ~BIT2; //input
    P3->REN |= BIT2;
    P3->OUT |= BIT2;
    P3->IES |= BIT2; //Set pin interrupt to trigger from high to low (starts high due to pull up resistor)
    P3->IE |= BIT2; //Set pin interrupt on for P3.2

    P3->SEL0 &= ~BIT3; //set up P3.3 as GPIO, input, pull-up resistor
    P3->SEL1 &= ~BIT3;
    P3->DIR &= ~BIT3;
    P3->REN |= BIT3;
    P3->OUT |= BIT3;
    P3->IES |= BIT3; //Set pin interrupt to trigger from high to low (starts high due to pull up resistor)
    P3->IE |= BIT3; //Set pin interrupt on for P3.3

    P3->IFG = 0; //clear all port 3 interrupt flags
}

/****| keypad_init | *****************************************
 * Brief: This function intitializes the keypad.  Port 4 0,1,2,3 are
 * connected to the rows 1, 2, 3, 4, Port 3 5,6,7 are connected
 * to the columns 1, 2, 3 of the keypad.
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void keypad_init()
{
    //Rows of keypad-------------------------------------------------
    //Row 1
    P4->SEL0 &= ~BIT0;
    P4->SEL1 &= ~BIT0; //set P4.0 as GPIO
    P4->DIR &= ~BIT0; //set up P4.0 as input
    P4->REN |= BIT0; //enable internal resistors on MSP
    P4->OUT |= BIT0; //enable pullup resistor

    //Row 2
    P4->SEL0 &= ~BIT1;
    P4->SEL1 &= ~BIT1; //set P4.1 as GPIO
    P4->DIR &= ~BIT1; //set up P4.1 as input
    P4->REN |= BIT1; //enable internal resistors on MSP
    P4->OUT |= BIT1; //enable pullup resistor

    //Row 3
    P4->SEL0 &= ~BIT2;
    P4->SEL1 &= ~BIT2; //set P4.2 as GPIO
    P4->DIR &= ~BIT2; //set up P4.2 as input
    P4->REN |= BIT2; //enable internal resistors on MSP
    P4->OUT |= BIT2; //enable pullup resistor

    //Row 4
    P4->SEL0 &= ~BIT3;
    P4->SEL1 &= ~BIT3; //set P4.3 as GPIO
    P4->DIR &= ~BIT3; //set up P4.3 as input
    P4->REN |= BIT3; //enable internal resistors on MSP
    P4->OUT |= BIT3; //enable pullup resistor

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
