/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/17/2021
Project: Lab 9 Part 3 Button Flashing 7 Segement LED
Description: This program will flash the 7 segment LED through
the numbers 0-9 by adding one whenever button 1 is pressed and
decrementing one whenever button 2 is pressed.
 *************************************************************/

#include "msp.h"
#include <TimerLib.h>

void Port3Interrupts_Init(void); //prototype for pushbutton pin initialization function
void SevenSegmentLEDPinInit(void); //prototype for 7 segment LED pin initialization
void switchNum(int);

volatile int numCount = 0; //global variable numCount initially set to 0

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    SevenSegmentLEDPinInit(); //initialize 7 segment LED pins
    SysTick_Init(); //enable Systick timer
    Port3Interrupts_Init(); //set up pins to buttons that will interrupt program

    NVIC_EnableIRQ(PORT3_IRQn); //enable port 3 interrupts on NVIC
    __enable_irq(); //enable all turned on interrupts

    switchNum(numCount); //initialize the 7 segment LED to display 0 upon startup

    while(1)
    {
        switchNum(numCount); //display the value of numCount on the LED
    }
}

/****| SevenSegmentLEDPinInit | ******************************
 * Brief: This function initializes the GPIO for the 7 segment
 * LED. P4.0 is connected to LED a, P4.1 is connected to LED b,
 * P4.2 is connected to LED c, P4.3 is connected to LED d, P4.4
 * is connected to LED e, P4.5 is connected to LED f, and P4.6
 * is connected to LED g
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void SevenSegmentLEDPinInit(void)
{
    //LED a
    P4->SEL0 &= ~BIT0; //Configure P4.0 as I/O
    P4->SEL1 &= ~BIT0;
    P4->DIR |= BIT0; //set P4.0 as output

    //LED b
    P4->SEL0 &= ~BIT1; //Configure P4.1 as I/O
    P4->SEL1 &= ~BIT1;
    P4->DIR |= BIT1; //set P4.1 as output

    //LED c
    P4->SEL0 &= ~BIT2; //Configure P4.2 as I/O
    P4->SEL1 &= ~BIT2;
    P4->DIR |= BIT2; //set P4.2 as output

    //LED d
    P4->SEL0 &= ~BIT3; //Configure P4.3 as I/O
    P4->SEL1 &= ~BIT3;
    P4->DIR |= BIT3; //set P4.3 as output

    //LED e
    P4->SEL0 &= ~BIT4; //Configure P4.4 as I/O
    P4->SEL1 &= ~BIT4;
    P4->DIR |= BIT4; //set P4.4 as output

    //LED f
    P4->SEL0 &= ~BIT5; //Configure P4.5 as I/O
    P4->SEL1 &= ~BIT5;
    P4->DIR |= BIT5; //set P4.5 as output

    //LED g
    P4->SEL0 &= ~BIT6; //Configure P4.6 as I/O
    P4->SEL1 &= ~BIT6;
    P4->DIR |= BIT6; //set P4.6 as output
}

/****| switchNum | ******************************************
 * Brief: This function changes the number displayed on the
 * 7-segment LED by changing the LED lights turned on in the
 * overall LED.
 * param:
 * (int) type variable called count that holds the value of the
 * number desired to display on the LED
 * return:
 * N/A
 *************************************************************/

void switchNum(int count)
{
    switch(count) //switch case to handle each possible value of count, 0-9
    {
    //0 displayed on LED
    case 0:
        P4->OUT |= BIT0; //turn on LED a
        P4->OUT |= BIT1; //turn on LED b
        P4->OUT |= BIT2; //turn on LED c
        P4->OUT |= BIT3; //turn on LED d
        P4->OUT |= BIT4; //turn on LED e
        P4->OUT |= BIT5; //turn on LED f
        P4->OUT &= ~BIT6; //turn off LED g
        break;

        //1 displayed on LED
    case 1:
        P4->OUT &= ~BIT0; //turn off LED a
        P4->OUT |= BIT1; //turn on LED b
        P4->OUT |= BIT2; //turn on LED c
        P4->OUT &= ~BIT3; //turn off LED d
        P4->OUT &= ~BIT4; //turn off LED e
        P4->OUT &= ~BIT5; //turn off LED f
        P4->OUT &= ~BIT6; //turn off LED g
        break;

        //2 displayed on LED
    case 2:
        P4->OUT |= BIT0; //turn on LED a
        P4->OUT |= BIT1; //turn on LED b
        P4->OUT &= ~BIT2; //turn off LED c
        P4->OUT |= BIT3; //turn on LED d
        P4->OUT |= BIT4; //turn on LED e
        P4->OUT &= ~BIT5; //turn off LED f
        P4->OUT |= BIT6; //turn on LED g
        break;

        //3 displayed on LED
    case 3:
        P4->OUT |= BIT0; //turn on LED a
        P4->OUT |= BIT1; //turn on LED b
        P4->OUT |= BIT2; //turn on LED c
        P4->OUT |= BIT3; //turn on LED d
        P4->OUT &= ~BIT4; //turn off LED e
        P4->OUT &= ~BIT5; //turn off LED f
        P4->OUT |= BIT6; //turn on LED g
        break;

        //4 displayed on LED
    case 4:
        P4->OUT &= ~BIT0; //turn off LED a
        P4->OUT |= BIT1; //turn on LED b
        P4->OUT |= BIT2; //turn on LED c
        P4->OUT &= ~BIT3; //turn off LED d
        P4->OUT &= ~BIT4; //turn off LED e
        P4->OUT |= BIT5; //turn on LED f
        P4->OUT |= BIT6; //turn on LED g
        break;

        //5 displayed on LED
    case 5:
        P4->OUT |= BIT0; //turn on LED a
        P4->OUT &= ~BIT1; //turn off LED b
        P4->OUT |= BIT2; //turn on LED c
        P4->OUT |= BIT3; //turn on LED d
        P4->OUT &= ~BIT4; //turn off LED e
        P4->OUT |= BIT5; //turn on LED f
        P4->OUT |= BIT6; //turn on LED g
        break;

        //6 displayed on LED
    case 6:
        P4->OUT |= BIT0; //turn on LED a
        P4->OUT &= ~BIT1; //turn off LED b
        P4->OUT |= BIT2; //turn on LED c
        P4->OUT |= BIT3; //turn on LED d
        P4->OUT |= BIT4; //turn on LED e
        P4->OUT |= BIT5; //turn on LED f
        P4->OUT |= BIT6; //turn on LED g
        break;

        //7 displayed on LED
    case 7:
        P4->OUT |= BIT0; //turn on LED a
        P4->OUT |= BIT1; //turn on LED b
        P4->OUT |= BIT2; //turn on LED c
        P4->OUT &= ~BIT3; //turn off LED d
        P4->OUT &= ~BIT4; //turn off LED e
        P4->OUT &= ~BIT5; //turn off LED f
        P4->OUT &= ~BIT6; //turn off LED g
        break;

        //8 displayed on LED
    case 8:
        P4->OUT |= BIT0; //turn on LED a
        P4->OUT |= BIT1; //turn on LED b
        P4->OUT |= BIT2; //turn on LED c
        P4->OUT |= BIT3; //turn on LED d
        P4->OUT |= BIT4; //turn on LED e
        P4->OUT |= BIT5; //turn on LED f
        P4->OUT |= BIT6; //turn on LED g
        break;

        //9 displayed on LED
    case 9:
        P4->OUT |= BIT0; //turn on LED a
        P4->OUT |= BIT1; //turn on LED b
        P4->OUT |= BIT2; //turn on LED c
        P4->OUT &= ~BIT3; //turn off LED d
        P4->OUT &= ~BIT4; //turn off LED e
        P4->OUT |= BIT5; //turn on LED f
        P4->OUT |= BIT6; //turn on LED g
        break;
    }
}


/****| PORT3_IRQHANDLER | ******************************************
 * Brief: This function set up interrupt handler for port 3, where
 * buttons 1 and 2 are both connected. Inside it
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void PORT3_IRQHandler(void)
{
    uint8_t debounce = 0; //variable to help debounce button presses

    while(debounce <= 100) //while the debounce variable is less than or equal to 100
    {
        if((~P3->IN & BIT2) || (~P3->IN & BIT3)) //debounce the button presses by making sure that the button is held down and still pressed for at least 100 cycles
            debounce++; //increment debounce
        else //if neither button 1 nor button 2 is being held down, reset the debounce to 0
            debounce = 0;
    }
    if(P3->IFG & BIT2) //if button 1 was pushed, setting the interrupt on P3.2
    {

        if(numCount < 9) //if numCount is less than the maximum number 9
        {
            numCount++; //increment the numCount variable
        }
        else if(numCount == 9) //if numCount is already at the maximum cap 9
        {
            numCount = 0; //reset the value of numCount at the smallest number in the sequence
        }

    }

    if(P3->IFG & BIT3) //if button 2 was pushed, setting interrupt on P3.3
    {

        if(numCount > 0) //if numCount is greater than 0, the smallest possible number in the sequence
        {
            numCount--; //decrement numCount
        }
        else if(numCount == 0) //if numCount is already at the smallest possible number in the sequence
        {
            numCount = 9; //restart the sequence at the maximum number
        }

    }

    P3->IFG = 0; //clear all port 3 interrupt flags
}

/****| Port3Interrupts_Init | ******************************************
 * Brief: This function initializes the pins that the pushbuttons
 * are connected to as GPIO, inputs, internal pull up resistors enabled,
 * with pin interrupt to trigger from high to low due to pull up
 * resistors.
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void Port3Interrupts_Init(void)
{
    P3->SEL0 &= ~BIT2; //set up P3.2 as GPIO, input, pull-up resistor
    P3->SEL1 &= ~BIT2;
    P3->DIR &= ~BIT2;
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
