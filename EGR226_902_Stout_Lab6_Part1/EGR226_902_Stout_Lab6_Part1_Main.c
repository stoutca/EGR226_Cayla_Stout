/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 2/24/2021
Project: Lab 6 Part 1 Keypad Entry Program
Description: This program calls a subroutine that scans a 4x3
matrix keypad and returns a unique number for each key pressed.
The character corresponding to the key pressed is printed to
the CCS console. Port 4 0,1,2,3 are connected to the rows
Port 4 4,5,6 are connected to the columns of the keypad
 *************************************************************/

#include "msp.h"
#include <stdio.h> //include this library to use functions like printf()

void keypad_init (void); // prototype for GPIO initialization
uint8_t Read_Keypad (void); // prototype for keypad scan subroutine
void Print_Keys(int); // Print Key pressed
uint8_t num, pressed; //Global variables

void SysTick_Init (void);
void SysTick_delay (uint16_t delay);


void main(void)
{
    SysTick_Init();
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    keypad_init(); // Call initialization functions for functionality

    while (1)
    {
        pressed = Read_Keypad(); // Call Function to read Keypad
        if (pressed) //if the keypad was pressed
            Print_Keys(num); //print the pressed numbers to the screen
        SysTick_delay(10); // 10ms delay through the loop before reading keypad again
    }

}

/****| SysTick_Init | ************************************
 * Brief: This function initializes the function to
 * delay the program using the SysTick
 * system timer.
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void SysTick_Init (void)
{ //initialization of systic timer
    SysTick -> CTRL = 0; // disable SysTick During step
    SysTick -> LOAD = 0x00FFFFFF; // max reload value
    SysTick -> VAL = 0; // any write to current clears it
    SysTick -> CTRL = 0x00000005; // enable systic, 3MHz, No Interrupts
}

/****| SysTick_delay | ************************************
 * Brief: This function delays the program using the SysTick
 * clock.
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void SysTick_delay (uint16_t delay)
{ // Systick delay function
    SysTick -> LOAD = ((delay * 3000) - 1); //delay for 1 msecond per delay value
    SysTick -> VAL = 0; // any write to CVR clears it
    while((SysTick->CTRL & BIT(16)) == 0); // wait for flag to be SET
}

/****| keypad_init | *****************************************
 * Brief: This function intitializes the keypad.  Port 4 0,1,2,3 are
 * connected to the rows, Port 4 4,5,6 are connected to the columns
 * of the keypad.
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void keypad_init (void)
{
    //Rows of keypad-------------------------------------------------
    P4->SEL0 &= ~BIT0;
    P4->SEL1 &= ~BIT0; //set P4.0 as GPIO
    P4->DIR &= ~BIT0; //set up P4.0 as input
    P4->REN |= BIT0; //enable internal resistors on MSP
    P4->OUT |= BIT0; //enable pullup resistor

    P4->SEL0 &= ~BIT1;
    P4->SEL1 &= ~BIT1; //set P4.1 as GPIO
    P4->DIR &= ~BIT1; //set up P4.1 as input
    P4->REN |= BIT1; //enable internal resistors on MSP
    P4->OUT |= BIT1; //enable pullup resistor

    P4->SEL0 &= ~BIT2;
    P4->SEL1 &= ~BIT2; //set P4.2 as GPIO
    P4->DIR &= ~BIT2; //set up P4.2 as input
    P4->REN |= BIT2; //enable internal resistors on MSP
    P4->OUT |= BIT2; //enable pullup resistor

    P4->SEL0 &= ~BIT3;
    P4->SEL1 &= ~BIT3; //set P4.3 as GPIO
    P4->DIR &= ~BIT3; //set up P4.3 as input
    P4->REN |= BIT3; //enable internal resistors on MSP
    P4->OUT |= BIT3; //enable pullup resistor

    //Columns of keypad---------------------------------------------------
    P4->SEL0 &= ~BIT4;
    P4->SEL1 &= ~BIT4; //set P4.4 as GPIO


    P4->SEL0 &= ~BIT5;
    P4->SEL1 &= ~BIT5; //set P4.5 as GPIO


    P4->SEL0 &= ~BIT6;
    P4->SEL1 &= ~BIT6; //set P4.6 as GPIO
}

/****| Print_keys() | *****************************************
* Brief: This function prints the keys that the user inputs
* into the keypad.
* param:
* GLOBAL integer variable called "num" that stores the value
* of the number input in the Read_Keypad() function
* return:
* N/A
*************************************************************/

void Print_Keys(num)
{
    //if num is a special character, then the number for the special character must be identified and num corrected to the character
    if(num == 10)
        printf("[*]\n");
    if(num == 11)
        num = 0;
    if(num == 12)
        printf("[#]\n");

    if((num != 10) && (num != 12)) //if the num value is not a special character, print the number
    printf("[%d]\n", num);
}

/****| Read_Keypad() | *****************************************
* Brief: scans a 4x3 matrix keypad and returns a unique number
* for each key pressed. The character corresponding to the key
* pressed is printed to the CCS console.
* param:
* N/A
* return:
* 1 if the keypad was pressed, 0 if the keypad was not pressed
***************************************************************/

uint8_t Read_Keypad(void)
{
    uint8_t col, row;
    for ( col = 0; col < 3; col++ )
    {
        P4->DIR = 0x00; // Set Columns to inputs
        P4->DIR |= BIT(4+col); // Set column 3 to output
        P4->OUT &=~ BIT (4+col); // Set column 3 to LOW
        SysTick_delay(10); // Delay the while loop
        row = P4->IN & 0x0F; // read all rows
        while (!(P4IN & BIT0) | !(P4IN & BIT1) | !(P4IN & BIT2) | !( P4IN & BIT3) );

        if (row != 0x0F)
            break; // if one of the input is low, some key is pressed.
    }

    P4->DIR = 0x00; // Set Columns to inputs
    if ( col == 3)
        return 0;
    if (row == 0x0E)
        num = col + 1; // key in row 0
    if (row == 0x0D)
        num = 3 + col +1; // key in row 1
    if (row == 0x0B)
        num = 6 + col +1; // key in row 2
    if (row == 0x07)
        num = 9 + col+1; // key in row 3
    return 1;
}
