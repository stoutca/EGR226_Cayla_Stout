/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Project: Pushbutton Library
Description: This .c file defines the functions for initializing
and debouncing the pushbuttons used in a project's circuit
 *************************************************************/

#include <PushbuttonLib.h>
#include "msp.h"
#include <TimerLib.h>

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
    //button 1
    P3->SEL0 &= ~BIT2; //Configure P3.2 as I/O
    P3->SEL1 &= ~BIT2;
    P3->DIR &= ~BIT2; //set P3.2 as input
    P3->REN |= BIT2; //enable pull resistor on P3.2
    P3->OUT |= BIT2; //set pull up resistor on P3.2

    //button 2
    P3->SEL0 &= ~BIT3; //Configure P3.3 as I/O
    P3->SEL1 &= ~BIT3;
    P3->DIR &= ~BIT3; //set P3.3 as input
    P3->REN |= BIT3; //enable pull resistor on P3.3
    P3->OUT |= BIT3; //set pull up resistor on P3.3

    //button 3
    P6->SEL0 &= ~BIT1; //Configure P6.1 as I/O
    P6->SEL1 &= ~BIT1;
    P6->DIR &= ~BIT1; //set P6.1 as input
    P6->REN |= BIT1; //enable pull resistor on P6.1
    P6->OUT |= BIT1; //set pull up resistor on P6.1

    return;
}

/****| DebouncePress1 | **************************************
 * Brief: This function tests if button 1 was pressed, and
 * if it was, debounces the press with a delay.
 * param:
 * N/A
 * return:
 * 0 if the button was not pushed, 1 if the button was pushed
 * after being debounced.
 *************************************************************/

int DebouncePress1(void)
{
    int pin_Value = 0; //initialize variable as low
    if((P3->IN & BIT2) == 0x00) //check if button pushed
    {
        delay_ms(10); //pause for 10 m-sec for switch bounce
        if((P3->IN & BIT2) == 0x00) //check of button is still pushed
            pin_Value = 1;
    }
    return pin_Value; //return 1 if pushed- 0 if not pushed
}

/****| DebounceRelease1 | ************************************
 * Brief: This function tests if button 1 was released after
 * being pressed, and if it was, debounces the release with a
 * delay.
 * param:
 * N/A
 * return:
 * 0 if the button was not pushed, 1 if the button was pushed.
 *************************************************************/

int DebounceRelease1(void)
{
    int pinValue = 1; //initialize variable as low
    if((P3->IN & BIT2) == 0x04) //check if button is released
    {
        delay_ms(10); //pause for 10 m-sec for switch bounce
        if((P3->IN & BIT2) == 0x04) //check of button is still released
            pinValue = 0;
    }
    return pinValue; //return 1 if pushed- 0 if not pushed
}

/****| DebouncePress2 | **************************************
 * Brief: This function tests if button 2 was pressed, and
 * if it was, debounces the press with a delay.
 * param:
 * N/A
 * return:
 * 0 if the button was not pushed, 1 if the button was pushed
 * after being debounced.
 *************************************************************/

int DebouncePress2(void)
{
    int pin_Value = 0; //initialize variable as low
    if((P3->IN & BIT3) == 0x00) //check if button pushed
    {
        delay_ms(10); //pause for 10 m-sec for switch bounce
        if((P3->IN & BIT3) == 0x00) //check of button is still pushed
            pin_Value = 1;
    }
    return pin_Value; //return 1 if pushed- 0 if not pushed
}

/****| DebounceRelease2 | ************************************
 * Brief: This function tests if button 2 was released after
 * being pressed, and if it was, debounces the release with a
 * delay.
 * param:
 * N/A
 * return:
 * 0 if the button was not pushed, 1 if the button was pushed.
 *************************************************************/

int DebounceRelease2(void)
{
    int pinValue = 1; //initialize variable as low
    if((P3->IN & BIT3) == 0x08) //check if button is released
    {
        delay_ms(10); //pause for 10 m-sec for switch bounce
        if((P3->IN & BIT3) == 0x08) //check of button is still released
            pinValue = 0;
    }
    return pinValue; //return 1 if pushed- 0 if not pushed
}

/****| DebouncePress3 | **************************************
 * Brief: This function tests if button 2 was pressed, and
 * if it was, debounces the press with a delay.
 * param:
 * N/A
 * return:
 * 0 if the button was not pushed, 1 if the button was pushed
 * after being debounced.
 *************************************************************/

int DebouncePress3(void)
{
    int pin_Value = 0; //initialize variable as low
    if((P6->IN & BIT1) == 0x00) //check if button pushed
    {
        delay_ms(10); //pause for 10 m-sec for switch bounce
        if((P6->IN & BIT1) == 0x00) //check of button is still pushed
            pin_Value = 1;
    }
    return pin_Value; //return 1 if pushed- 0 if not pushed
}

/****| DebounceRelease3 | ************************************
 * Brief: This function tests if button 2 was released after
 * being pressed, and if it was, debounces the release with a
 * delay.
 * param:
 * N/A
 * return:
 * 0 if the button was not pushed, 1 if the button was pushed.
 *************************************************************/

int DebounceRelease3(void)
{
    int pinValue = 1; //initialize variable as low
    if((P6->IN & BIT1) == 0x02) //check if button is released
    {
        delay_ms(10); //pause for 10 m-sec for switch bounce
        if((P6->IN & BIT1) == 0x02) //check of button is still released
            pinValue = 0;
    }
    return pinValue; //return 1 if pushed- 0 if not pushed
}

