/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 2/17/2021
Project: Lab 5 Part 3 LED flashing program
Description: This program will cycle through the red, yellow,
and green LED colors every time the button on P2.7 is
pressed. If the reverse button on P2.6 is pressed, the lights
light up in reverse.
 *************************************************************/

#include "msp.h"

//function prototypes
void InitializeLEDs(void);
void InitializeButtons(void);
int DebouncePressButton1(void);
int DebouncePressButton2(void);
int DebounceReleaseButton1(void);
int DebounceReleaseButton2(void);
void SysTick_Init (void);
void SysTick_delay (uint16_t delay);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer

    //flags for keeping track of pushes
    int flag = 0; //the value of this flag will change depending on the cycle the LED is going through with each button push
    int time_delay = 1000; //variable to control ms delay between light changes
    int firstTimeflag = 1; //flag to tell the program if it was just initialized
    InitializeLEDs(); //initialize the LEDs
    InitializeButtons(); //initialize the pull up resistor on button P1.1
    SysTick_Init(); //initialize the Systick timer

    //set LEDs to low (off) to begin
    P2->OUT &= ~BIT5; //set red LED to low
    P3->OUT &= ~BIT0; //set yellow LED to low
    P5->OUT &= ~BIT7; //set green LED to low

    //read first button first
    while(1) //loop for program to run in
    {
        if(DebouncePressButton1())
        {
            if(firstTimeflag == 0) //if this is not the first time through the program
            {
                if(flag != 2) //when the flag arrives here after the "forward" button 1 is pushed, the flag is one value lower than it should be
                    flag = flag+1; //increment the flag value to ensure the LED sequence does not continue forward
                else if(flag == 2) //if the flag was 2 upon entering the press sequence for button 1
                    flag = 0; //the flag value in the button 2 code goes to 0 after 2, so 2+1 in this program would set the flag value back to 0 in the sequence
            }
            while(DebounceReleaseButton1() == 1) //run the code while the button is not released. Will leave the loop when the button is released
            {
                if(flag == 0) //if it is the first button push
                {
                    P3->OUT &= ~BIT0; //set yellow LED to low
                    P5->OUT &= ~BIT7; //set green LED to low
                    P2->OUT |= BIT5; //Turn Red LED on
                    SysTick_delay(time_delay);
                    flag = 1;
                    firstTimeflag = 0; //tell the program that the first time through the code has been completed if button 1 was pressed first
                }
                if(DebounceReleaseButton1() == 0)
                    break;
                if(flag == 1)
                {
                    P2->OUT &= ~BIT5; //set red LED to low
                    P5->OUT &= ~BIT7; //set green LED to low
                    P3->OUT |= BIT0; //Turn yellow LED on
                    SysTick_delay(time_delay); //delay the program by the variable amount time delay
                    flag = 2;

                }
                if(DebounceReleaseButton1() == 0)
                    break;
                if(flag == 2)
                {
                    P2->OUT &= ~BIT5; //set red LED to low
                    P3->OUT &= ~BIT0; //set yellow LED to low
                    P5->OUT |= BIT7; //Turn green LED on
                    SysTick_delay(time_delay); //delay the program by the variable amount time delay
                    flag = 0;
                }
                if(DebounceReleaseButton1() == 0)
                    break;
            }
        }

        else if(DebouncePressButton2())
        {
            if(firstTimeflag == 0) //if this is not the first time through the program
            {
                if(flag != 0) //when the flag arrives here after the "forward" button 1 is pushed, the flag is one value higher than it should be
                    flag = flag-1; //decrement the flag value to ensure the LED sequence does not continue forward
                else if(flag == 0) //if the flag was zero upon entering the button 2 pressed
                    flag = 2; //the flag value in the button 1 code goes to 2 after 0, so 0-1 in this program would set the flag value back to 2 in the sequence
            }
            while(DebounceReleaseButton2() == 1) //run the code while the button is not released. Will leave the loop when the button is released
            {
                if(flag == 0) //if it is the first button push
                {
                    P3->OUT &= ~BIT0; //set yellow LED to low
                    P5->OUT &= ~BIT7; //set green LED to low
                    P2->OUT |= BIT5; //Turn Red LED on
                    SysTick_delay(time_delay);
                    flag = 2;
                    firstTimeflag = 0; //tell the program that the first time through the code has been completed if button 2 was pressed first
                }
                if(DebounceReleaseButton2() == 0)
                    break;
                if(flag == 1)
                {
                    P2->OUT &= ~BIT5; //set red LED to low
                    P5->OUT &= ~BIT7; //set green LED to low
                    P3->OUT |= BIT0; //Turn yellow LED on
                    SysTick_delay(time_delay); //delay the program by the variable amount time delay
                    flag = 0;
                }
                if(DebounceReleaseButton2() == 0)
                    break;
                if(flag == 2)
                {
                    P2->OUT &= ~BIT5; //set red LED to low
                    P3->OUT &= ~BIT0; //set yellow LED to low
                    P5->OUT |= BIT7; //Turn green LED on
                    SysTick_delay(time_delay); //delay the program by the variable amount time delay
                    flag = 1;
                }
                if(DebounceReleaseButton2() == 0)
                    break;
            }
        }

        else if(DebouncePressButton1() && DebouncePressButton2()) //if the buttons are pushed at the same time, it acts as a reset to the program
        {
            //set LEDs to low (off)
            P2->OUT &= ~BIT5; //set red LED to low
            P3->OUT &= ~BIT0; //set yellow LED to low
            P5->OUT &= ~BIT7; //set green LED to low

            firstTimeflag = 1; //reset firstTimeflag to 1 to reset the program
            flag = 0; //reset the value of flag to start at the red LED
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

    P2->SEL0 &= ~BIT6; //Configure P1.1 as I/O
    P2->SEL1 &= ~BIT6;
    P2->DIR &= ~BIT6; //set P1.1 as input
    P2->REN |= BIT6; //enable pull resistor
    P2->OUT |= BIT6; //set pull up resistor

    return;
}

/****| DebouncePressButton1 | **************************************
 * Brief: This function tests if button 1 was pressed, and
 * if it was, debounces the press with a delay.
 * param:
 * N/A
 * return:
 * 0 if the button was not pushed, 1 if the button was pushed
 * after being debounced.
 *************************************************************/

int DebouncePressButton1(void)
{
    int pin_Value = 0; //initialize variable as low
    if((P2->IN & BIT7) == 0x00) //check if button 1 pushed
    {
        SysTick_delay(10); //pause for 10 m-sec for switch bounce
        if((P2->IN & BIT7) == 0x00) //check if button is still pushed
            pin_Value = 1;
    }
    return pin_Value; //return 1 if pushed- 0 if not pushed
}

/****| DebouncePressButton2 | **************************************
 * Brief: This function tests if button 2 was pressed, and
 * if it was, debounces the press with a delay.
 * param:
 * N/A
 * return:
 * 0 if the button was not pushed, 1 if the button was pushed
 * after being debounced.
 *************************************************************/

int DebouncePressButton2(void)
{
    int pin_Value = 0; //initialize variable as low
    if((P2->IN & BIT6) == 0x00) //check if button 2 pushed
    {
        SysTick_delay(10); //pause for 10 m-sec for switch bounce
        if((P2->IN & BIT6) == 0x00) //check if button is still pushed
            pin_Value = 1;
    }
    return pin_Value; //return 1 if pushed- 0 if not pushed
}

/****| DebounceReleaseButton1 | ************************************
 * Brief: This function tests if button 1 was released after
 * being pressed, and if it was, debounces the release with a
 * delay.
 * param:
 * N/A
 * return:
 * 0 if the button was not pushed, 1 if the button was pushed.
 *************************************************************/

int DebounceReleaseButton1(void)
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

/****| DebounceReleaseButton2 | ************************************
 * Brief: This function tests if button 2 was released after
 * being pressed, and if it was, debounces the release with a
 * delay.
 * param:
 * N/A
 * return:
 * 0 if the button was not pushed, 1 if the button was pushed.
 *************************************************************/

int DebounceReleaseButton2(void)
{
    int pinValue = 1; //initialize variable as low
    if((P2->IN & BIT6) == 0x40) //check if button is released
    {
        SysTick_delay(10); //pause for 10 m-sec for switch bounce
        if((P2->IN & BIT6) == 0x40) //check of button is still released
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
