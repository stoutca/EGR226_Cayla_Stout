/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 2/10/2021
Project: Library for Debouncing Button Presses and Releases
Description: This .c file contains the function definitions 
to debounce a button press and release
*************************************************************/

#include "msp.h"

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
        __delay_cycles(30000); //pause for 10 m-sec for switch bounce
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
        __delay_cycles(30000); //pause for 10 m-sec for switch bounce
        if((P1->IN & BIT1) == 0x02) //check of button is still released
            pinValue = 0;
    }
    return pinValue; //return 1 if pushed- 0 if not pushed
}
