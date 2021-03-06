/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 2/17/2021
Project: Lab 5 Part 2 LED flashing program
Description: This program will cycle through the red, yellow,
and green LED colors every time the button on P2.7 is
pressed.
 *************************************************************/

#include "msp.h"

//function prototypes
void InitializeLEDs(void);
void InitializeButtons(void);
int DebouncePress(void);
int DebounceRelease(void);
void SysTick_Init (void);
void SysTick_delay (uint16_t delay);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer

    //flags for keeping track of pushes
    int flag = 0; //the value of this flag will change depending on the cycle the LED is going through with each button push
    int time_delay = 1000; //variable to control ms delay between light changes
    InitializeLEDs(); //initialize the LEDs
    InitializeButtons(); //initialize the pull up resistor on button P1.1
    SysTick_Init();

    //set LEDs to low (off) to begin
    P2->OUT &= ~BIT5; //set red LED to low
    P3->OUT &= ~BIT0; //set yellow LED to low
    P5->OUT &= ~BIT7; //set green LED to low

    while(1) //loop for program to run in
    {
        if(DebouncePress())
        {
            while(DebounceRelease() == 1) //run the code while the button is not released. Will leave the loop when the button is released
            {
                if(flag == 0) //if it is the first button push
                {
                    P3->OUT &= ~BIT0; //set yellow LED to low
                    P5->OUT &= ~BIT7; //set green LED to low
                    P2->OUT |= BIT5; //Turn Red LED on
                    flag = 1; //set flag to 1
                }
                SysTick_delay(time_delay);
                if(DebounceRelease() == 0)
                    break;
                if(flag == 1)
                {
                    P2->OUT &= ~BIT5; //set red LED to low
                    P5->OUT &= ~BIT7; //set green LED to low
                    P3->OUT |= BIT0; //Turn yellow LED on
                    flag = 2; //set flag to 2
                }
                SysTick_delay(time_delay); //delay the program by the variable amount time delay
                if(DebounceRelease() == 0)
                    break;
                if(flag == 2)
                {
                    P2->OUT &= ~BIT5; //set red LED to low
                    P3->OUT &= ~BIT0; //set yellow LED to low
                    P5->OUT |= BIT7; //Turn green LED on
                    flag = 0; //set flag to 0 to reset cycle
                }
                SysTick_delay(time_delay); //delay the program by the variable amount time delay
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
    P2->SEL0 &= ~BIT5; //Configure P2.0 as simple I/O
    P2->SEL1 &= ~BIT5;
    P2->DIR |= BIT5; //set P2.0 as output pin

    P3->SEL0 &= ~BIT0; //configure P2.1 as simple I/O
    P3->SEL1 &= ~BIT0;
    P3->DIR |= BIT0; //set P2.1 as output pin

    P5->SEL0 &= ~BIT7; //configure P2.2 as simple I/0
    P5->SEL1 &= ~BIT7;
    P5->DIR |= BIT7; //set P2.2 as output pin

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
    P2->SEL0 &= ~BIT7; //Configure P1.1 as I/O
    P2->SEL1 &= ~BIT7;
    P2->DIR &= ~BIT7; //set P1.1 as input
    P2->REN |= BIT7; //enable pull resistor
    P2->OUT |= BIT7; //set pull up resistor

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
    if((P2->IN & BIT7) == 0x00) //check if button pushed
    {
        SysTick_delay(10); //pause for 10 m-sec for switch bounce
        if((P2->IN & BIT7) == 0x00) //check of button is still pushed
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
    if((P2->IN & BIT7) == 0x80) //check if button is released
    {
        SysTick_delay(10); //pause for 10 m-sec for switch bounce
        if((P2->IN & BIT7) == 0x80) //check of button is still released
            pinValue = 0;
    }
    return pinValue; //return 1 if pushed- 0 if not pushed
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
