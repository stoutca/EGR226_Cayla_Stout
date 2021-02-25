/* This program calls a subroutine that scans a 4x3 matrix keypad
 * and returns a unique number for each key pressed. The character
 * corresponding to the key pressed is printed to the CCS console.
 * Port 4 0,1,2,3 are connected to the rows
 * Port 4 4,5,6 are connected to the columns of the keypad.*/

#include "msp.h"
#include <stdio.h>

void keypad_init (void); // prototype for GPIO initialization
uint8_t Read_Keypad (void); // prototype for keypad scan subroutine
void Print_Keys(int); // Print Key pressed
uint8_t num, pressed; //Global variables

void SysTick_Init (void);
void SysTick_delay (uint16_t delay);


void main(void)
{
    SysTick_Init();
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    keypad_init ( ); // Call initialization functions for functionality

    int count = 0; //variable to count the number of numbers entered into the pin

    int invalidInput = 0; //flag to catch if the user entered invalid input

    int numArray[4]; //array to hold the numbers for pin

    int tempNum1, tempNum2, tempNum3; //variable to temporarily hold the value of num

    int throughFlag = 0; //flag to help program remember if it has been through the do-while loop yet

    //prompt user for code
    printf("Please enter a 4 digit code :\n");

    while(1)
    {
        invalidInput = 0;
        //for getting 4 numbers, use a while loop until the # key is pressed (num would == 12). Then check if there were 4 numbers entered. If more than 4
        do
        {
            pressed = Read_Keypad(); // Call Function to read Keypad
            if (pressed)
            {
                Print_Keys(num);
                if((num != 10) && (count < 4) && (num != 12)) //if the user didn't enter a star or pound sign as number input and the input is less than 4 numbers
                    numArray[count] = num; //save the value of num into the array
                if((num != 12) && (num != 10)) //if the pound sign wasn't entered yet and the num is not a star
                    count++; //increment count
                if((count > 3) && (num != 12))//if count becomes higher than 3, meaning the user is entering more than 4 numbers, and the pound sign hasn't been entered
                {
                    if(num != 10) //if the user is not attempting to enter a star
                    {
                        tempNum1 = numArray[1]; //save the array values into temporary variables
                        tempNum2 = numArray[2];
                        tempNum3 = numArray[3];
                        numArray[3] = num; //set the new most recent number to the newest num entered
                        numArray[2] = tempNum3; //save the second most recent number to the previous most recent value
                        numArray[1] = tempNum2;  //save the third most recent number to the numArray[1] spot
                        numArray[0] = tempNum1; //save the fourth most recent number to the numArray[0] spot
                    }

                }
            }
            SysTick_delay(10); // 10ms delay through the loop before reading keypad again
        }
        while(num != 12); //while num doesn't equal 12 (the # sign), print the numbers and store them into a number array


        //if the user entered less than 4 numbers
        if((count < 4) && (throughFlag == 1))
        {
            printf("Please enter 4 digits for the pin: \n");
            invalidInput = 1;
        }

        //if the input is still good, print the pin to the user
        if(invalidInput == 0)
        {
            printf("The pin entered was: [%d], [%d], [%d], [%d]\n", numArray[0], numArray[1], numArray[2], numArray[3]);
            count = 0;
        }

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

    P2->DIR = 0x00; // Set Columns to inputs
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
