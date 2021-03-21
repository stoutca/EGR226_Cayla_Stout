/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/17/2021
Project: Lab 9 Part 2 One Second Flashing 7 Segment LED
Description: This program will flash the 7 segment LED through
the numbers 0-9 at half second intervals using SysTick Timer
interrupts.
 *************************************************************/

#include <TimerLibInterrupts.h>
#include "msp.h"

void SevenSegmentLEDPinInit(void);
void switchNum(int);

volatile uint32_t timeFlag;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    SysTick_Init(); //initialize the SysTick timer with interrupts
    SevenSegmentLEDPinInit(); //initialize the pins for the 7 segment LED

    __enable_irq(); //enable global interrupts

    int numCount = 0; //start the number displayed on the LED as a 0
    switchNum(numCount); //display a 0 on the LED

    while(1)
    {
        if(timeFlag) //if the interrupt was triggered and set the time out flag every half second
        {
            //increment the value of numCount as long as it is less than 9
            if(numCount < 9)
            {
                numCount++;
                switchNum(numCount); //change the value displayed on the LED
            }
            else if(numCount == 9) //if the count has reached the maximum number that can be displayed - 9
            {
                numCount = 0; //reset the count to 0
                switchNum(numCount); //change the value displayed on the LED
            }

            timeFlag = 0; //reset the interrupt time flag
        }
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

/****| SysTick_Handler | *************************************
 * Brief: This function handles the IRQ for the interrupts in
 * the SysTick timer module
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void SysTick_Handler(void)
{
    timeFlag = 1;
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
