/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 2/10/2021
Project: Lab 4 Part 1 LED flashing program
Description: This program will cycle through the red, blue,
and green LED colors every time the button on P1.1 is
pressed.
*************************************************************/

#include "msp.h"

//function prototypes
void InitializeLEDs(void);
void InitializeButtons(void);
int DebouncePress(void);
int DebounceRelease(void);
void delay(int);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer

    //flags for keeping track of pushes
    int flag = 0; //the value of this flag will change depending on the cycle the LED is going through with each button push
    int time_delay = 1000; //variable to control ms delay between light changes
    InitializeLEDs(); //initialize the LEDs
    InitializeButtons(); //initialize the pull up resistor on button P1.1


    //set LEDs to low (off) to begin
    P2->OUT &= ~BIT0; //set red LED to low
    P2->OUT &= ~BIT1; //set green LED to low
    P2->OUT &= ~BIT2; //set blue LED to low

    while(1) //loop for program to run in
    {
        if(DebouncePress())
            {
                while(DebounceRelease() == 1) //run the code while the button is not released. Will leave the loop when the button is released
                {
                    if(flag == 0) //if it is the first button push
                    {
                        P2->OUT &= ~BIT1; //set green LED to low
                        P2->OUT &= ~BIT2; //set blue LED to low
                        P2->OUT |= BIT0; //Turn P2.0 Red LED on
                        flag = 1; //set flag to 1
                    }
                    delay(time_delay); //delay the program by the variable amount time delay
                    if(DebounceRelease() == 0)
                        break;
                    if(flag == 1)
                    {
                        P2->OUT &= ~BIT0; //set red LED to low
                        P2->OUT &= ~BIT2; //set blue LED to low
                        P2->OUT |= BIT1; //Turn P2.0 Green LED on
                        flag = 2; //set flag to 2
                    }
                    delay(time_delay); //delay the program by the variable amount time delay
                    if(DebounceRelease() == 0)
                        break;
                    if(flag == 2)
                    {
                        P2->OUT &= ~BIT0; //set red LED to low
                        P2->OUT &= ~BIT1; //set green LED to low
                        P2->OUT |= BIT2; //Turn P2.0 Red LED on
                        flag = 0; //set flag to 0 to reset cycle
                    }
                    delay(time_delay); //delay the program by the variable amount time delay
                    if(DebounceRelease() == 0)
                        break;
                }
            }
    }
}



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

/****| DebouncePress | **************************************
* Brief: This function tests if the button was pressed, and
* if it was, debounces the press with a delay.
* param:
* N/A
* return:
* 0 if the button was not pushed, 1 if the button was pushed
* after being debounced.
*************************************************************/

int DebouncePress(void)
{
    int pin_Value = 0; //initialize variable as low
    if((P1->IN & BIT1) == 0x00) //check if button pushed
    {
         __delay_cycles(30000);
        if((P1->IN & BIT1) == 0x00) //check of button is still pushed
            pin_Value = 1;
    }
    return pin_Value; //return 1 if pushed- 0 if not pushed
}

/****| DebounceRelease | ************************************
* Brief: This function tests if the button was released after
* being pressed, and if it was, debounces the release with a
* delay.
* param:
* N/A
* return:
* 0 if the button was not pushed, 1 if the button was pushed.
*************************************************************/

int DebounceRelease(void)
{
    int pinValue = 1; //initialize variable as low
    if((P1->IN & BIT1) == 0x02) //check if button is released
    {
         //pause for a variable amount of time for switch bounce
        __delay_cycles(30000);
        if((P1->IN & BIT1) == 0x02) //check of button is still released
            pinValue = 0;
    }
    return pinValue; //return 1 if pushed- 0 if not pushed
}

/****| delay | **********************************************
* Brief: This function accepts an int parameter input and
* creates a delay for that integer amount of time.
* param:
* (int) time amount to delay the program by
* return:
* N/A
*************************************************************/
void delay(int num_milliseconds)
{
    int i;
    for(i = num_milliseconds; i > 0; i--) //delay that can change with the value of the wait parameter. The parameter is the number of miliseconds you want to wait for
    {
        __delay_cycles(3000); //wait one milisecond inside the for loop
    }
}
