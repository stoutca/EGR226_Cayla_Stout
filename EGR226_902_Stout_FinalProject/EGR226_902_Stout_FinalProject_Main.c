/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 4/21/2021
Project: Final Project
Description: This program will run a security system with a
lockable door (servo motor) that makes a sound when armed and
runs off of a pin to lock and unlock, and uses the MSP432 to
control menu operation and the parts of the system. There are
also lights controlled by a PWM and on/off switch, a motor that
runs off of PWM and can be set by the user in the menu, and
an emergency off button for that motor. The LCD is used to
display this menu and any output from the system sensors, and
it will "fall asleep" after the keypad is idle for all systems
for 10 seconds. The LCD brightness can be controlled at anytime
using PWM, the ADC, and potentiometers to dim or increase the
brightness.
 *************************************************************/

/*******States for switch states*******************************
 * 0 - main menu state
 * 1 - door menu state
 * 2 - motor menu state
 * 3 - LED menu state
 * 4 - Open Door Menu
 * 5 - Close Door Menu
 * 6 -
 *
 *
 **************************************************************/
//needed libraries
#include "msp.h"
#include <LCD_Lib.h>
#include <string.h> //include the string library
#include <stdio.h>
#include <stdlib.h>
#include <TimerLibInterrupts.h> //include timer interrupt library
#include <time.h> //time library to create random seeds for the random number generator

void LEDPinInit(void);
void motorPinInit(void);
void ADCSetup(void);
void keypad_init(void);
uint8_t Read_Keypad(void);
uint16_t randNum(); //random number generator


volatile uint16_t Vin; //global variable for storing the value of the ADC conversion, which happens on a SysTick interrupt
volatile uint32_t ADCInterrupt, alarmFlag, endFlag; //global variable flag for ADC timer32 interrupt, flag to sound alarm when "monster" attacks from vent
uint8_t num, pressed; //Global variables for storing the number that was pressed on the keypad and whether or not the keypad had been pressed

void main(void)
{
    int state;

    int stateChangeflag = 1; //flag to tell program when a state change has occured. This helps prevent blinking on the LCD

    uint16_t rand;

    endFlag = 0; //start the program with the ending flag at 0

    int randGenerated = 0;

    int tryAgainFlag = 0; //flag to tell the program if the user has used the program once before and is running through it again

    //time initializations
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    SysTick_Init(); //initialize the systick timer
    ADCSetup(); //set up the ADC

    //Pin Initializations
    LCDPinInit(); //initialize the pins for the LCD
    LEDPinInit(); //initialize pins for the LED
    motorPinInit();
    keypad_init(); //initialize the keypad

    LCD_init(); //initialize the LCD
    Timer32_Init(); //initialize timer 32

    //turn off LEDs, but leave P1.6 on as system starts armed
    P1->OUT &= ~BIT7; //turn off green led
    P1->OUT |= BIT6; //turn on red LED

    //close door - armed initially
    int alarmSound = 0; //flag to tell if the alarm is beeping or not
    int openDoor = 0; //flag to tell program that the door is closed

    //strings for main menu
    char mainMenuString[] = "    Main Menu    "; //string to display text on line 1
    char doorOption[] = " Option 1: Door "; //print on line 1
    char motorOption[] = "Option 2: Motor "; //print on line 2
    char lightOption[] = "Option 3: Lights"; //print on line 3

    //strings for door menu
    char doorMenu[] = "    Door Menu   "; //text for first line of door menu
    char openDoorOption[] = " Option 1: Open "; //second line of door menu
    char closeDoorOption[] = "Option 2: Close "; //third line of door menu

    //Variables for servo
    uint16_t Period_Servo = setPeriod(50); //variable to hold the period of the 50Hz servo
    uint16_t DC_Servo = setDutyCycle(0.1, Period_Servo); //set the duty cycle at 10% to start the servo at a 2ms period for the 0 degree position
    timerA2Init(Period_Servo); //set the timer A2 module for the servo motor and DC motor to run off of. This will be the period for TA2.1 and TA2.2
    timerA21Init(DC_Servo); //start with the servo closed to start

    //Variables for PWM LCD Backlight
    float DC_LCD1;
    uint16_t DC_LCD; //variable to hold final duty cycle values
    uint16_t Period_LCD = setPeriod(200); //variable to hold period of 200 Hz signal
    timerA0Init(Period_LCD); //set timerA0 module for RBG LEDs and LCD brightness
    timerA04Init(Period_LCD); //start with the LCD at full brightness

    state = 0; //start the system in state 0, which is the "main menu" state
    delay_ms(60); //allow 60 milliseconds for LCD to set

    //initialize NVIC interrupts for certain pins
    NVIC_EnableIRQ(T32_INT2_IRQn); //Enable Timer32_2 interrupt.
    NVIC_EnableIRQ(T32_INT1_IRQn); //Enable Timer32_1 interrupt.

    __enable_irq(); //enable global interrupts

    while(1)
    {
        //check if the user selected the option to try again, reset all beginning variables
        if(tryAgainFlag == 1)
        {
            endFlag = 0; //start the program with the ending flag at 0

            randGenerated = 0;

            tryAgainFlag = 0; //flag to tell the program if the user has used the program once before and is running through it again

            DC_Servo = setDutyCycle(0.1, Period_Servo); //set the duty cycle at 5% to start the servo at a 1ms period for the 0 degree position

            timerA21Init(DC_Servo); //start with the servo closed to start

            state = 0; //start the system in state 0, which is the "main menu" state

            //turn off LEDs, but leave P1.6 on as system starts armed
            P1->OUT &= ~BIT7; //turn off green led
            P1->OUT |= BIT6; //turn on red LED
        }
        //check if systick ADC interrupt flag has triggered
        if(ADCInterrupt == 1)
        {
            ADCInterrupt = 0; //reset interrupt flag
            DC_LCD1 = (Vin * 0.0002)/3.3; //save the decimal value of Vin to the duty cycle for the LCD by converting Vin to a voltage and dividing it by the reference voltage 3.3V
            DC_LCD = setDutyCycle(DC_LCD1, Period_LCD); //Get the duty cycle, in counts, for the adjusted LCD brightness
            timerA04Init(DC_LCD); //set the new DC for the PWM controlling the LCD brightness
        }

        //if a number hasn't been generated yet
        if(randGenerated == 0)
        {
            srand(time(0));
            rand = randNum();
            randGenerated = 1; //tell the program a number has been generated
        }

        //if a random number is generated
        if(randGenerated)
        {
            randGenerated = 0; //preemptively set the flag to 0
            //if the random number generated is 21
            if(rand == 7)
            {
                //activate the alarm in the vent, acting like something is hunting the user
                alarmFlag = 1;

                Timer32_1_Init(); //start the one-shot 5 second timer
                //start alarm sound
                //user has 5 seconds to close door

                randGenerated = 1; //reset the flag to 1 so no new numbers will be generated
            }
            //if the random number was not 21, continue looking for a random number in the next cycle
        }

        //if the user "died"
        if(endFlag == 1)
        {
            //state = ("death state");
        }

        switch(state)
        {

        //main menu state
        case 0:
            if(stateChangeflag == 1)
            {
                //clear the LED and move the cursor back to the home scren
                commandWrite(0x01); //clear display and set cursor to home
                delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position

                //print the main menu options to the LCD:
                printString(mainMenuString);

                //Option 1: Door menu
                commandWrite(0xC0); //line 2
                printString(doorOption);

                //Option 2: Motor Menu
                commandWrite(0x90); //line 3
                printString(motorOption);

                //Option 3: Lights Menu
                commandWrite(0xD0); //line 4
                printString(lightOption);

                delay_ms(100); //give LCD a chance to print strings
                stateChangeflag = 0;
            }

            pressed = Read_Keypad();
            if(pressed) //if the keypad is pressed
            {
                //if 1 is pressed on keypad, change state to 1 - Door Menu
                if(num == 1)
                {
                    state = 1;
                    stateChangeflag = 1;
                }

                //if 2 is pressed on keypad, change state to 2 - Motor Menu
                else if (num == 2)
                {
                    //state = 2;
                    //stateChangeflag = 1; //set this flag to alert the program that the state has changed
                }

                //if 3 is pressed on keypad, change state to 3 - Lights Menu
                else if(num == 3)
                {
                    //state = 3;
                    //stateChangeflag = 1; //set this flag to alert the program that the state has changed
                }

                //if the input from the keypad was not acceptable because it was not 1, 2, or 3
                else if((num!=1) || (num!=2) || (num!=3))
                {
                    //state = 0; //stay in the same state and but don't set changestate flag
                }

            }

            break;

            //Door Menu State
        case 1:
            if(stateChangeflag == 1) //if the state has been changed
            {
                //clear screen and move cursor to home position
                commandWrite(0x01); //clear display and set cursor to home
                delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position

                //Display options for door menu state:
                printString(doorMenu);

                //Option 1: Open Door
                commandWrite(0xC0); //line 2
                printString(openDoorOption);

                //Option 2: Close Door
                commandWrite(0x90); //line 3
                printString(closeDoorOption);

                delay_ms(100); //delay 500 ms so the LCD can print everything and so the user cannot press buttons more than 2 times a second
                stateChangeflag = 0; //reset the flag so the program knows that the state has been activated
            }

            //don't accept any input unless it is a 1 or 0 - keep accepting input until desired number is entered
            pressed = Read_Keypad(); //get keypad input
            if(pressed)
            {
                //if 1 is pressed, change state to 4 - open door state
                if(num == 1)
                {
                    state = 4;
                    stateChangeflag = 1;
                }

                //if 2 is pressed, change state to 5 - close door state
                else if(num == 2)
                {
                    state = 5;
                    stateChangeflag = 1;
                }

                //if neither 1 nor 2 was pressed
                else if((num!=1) || (num!=2))
                {
                    state = 1; //stay in the current state
                }

            }

            break;

            //Open Door State
        case 4:
            //clear screen, move cursor to home position
            commandWrite(0x01); //clear display and set cursor to home
            delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position

            //move servo to closed position - 180 degrees
            //change PWM of servo to 2ms - 10% duty cycle
            DC_Servo = setDutyCycle(0.05, Period_Servo); //calculate the duty cycle in counts
            timerA21Init(DC_Servo); //set the duty cycle in the servo PWM

            //turn on green LED, red LED off
            P1->OUT &= ~BIT6;
            P1->OUT |= BIT7;

            //move to state 1 to main menu
            state = 0;
            stateChangeflag = 1;

            break;

            //Close Door State
        case 5:
            //If the door is open and the alarm is sounding
            if((openDoor == 1) && (alarmFlag == 1))
            {
                alarmFlag = 0; //user closed the door, so deactivate alarm flag
                //turn off buzzer sound
            }
            //move servo to 0 degrees by sending 5% duty cycle to servo PWM, closes door
            DC_Servo = setDutyCycle(0.1, Period_Servo); //set new duty cycle in counts
            timerA21Init(DC_Servo); //set new duty cycle in servo PWM

            //turn on red LED, green LED off
            P1->OUT |= BIT6;
            P1->OUT &= ~BIT7;

            //change state to main menu state
            state = 0;

            break;

            //unarmed door state
        case 6:

            break;

        }
    }
}

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
    P2->DIR |= BIT7; // P2.4 set TA2.1

    //RBG pin init to PWM


}

void motorPinInit()
{
    //DC Motor

    //Servo Motor
    P5->SEL0 |= BIT6;
    P5->SEL1 &= ~(BIT6);
    P5->DIR |= BIT6; // P5.4 set TA2.1
}




/****| ADCSetup | ********************************************
 * Brief: This function initializes the ADC converter located on
 * P5.5 of the MSP
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void ADCSetup()
{
    ADC14->CTL0 = 0x00000010; //power on and disabled during configuration
    ADC14->CTL0 |= 0x04D80300; // S/H pulse mode, MCLCK, 32 sample clocks,
    //sw trigger, /4 clock divide
    ADC14->CTL1 = 0x00000030; // 14-bit resolution
    ADC14->MCTL[5] = 0; // A0 input, single ended, vref=avcc
    P5->SEL1 |= 0x20; // configure pin 5.5 for ADC use
    P5->SEL0 |= 0x20;
    ADC14->CTL1 |= 0x00050000; //start converting at mem reg 5
    ADC14->CTL0 |= 2; //enable ADC after configuration
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

/****| Read_Keypad | *****************************************
 * Brief: This function reads input presses from the keypad.
 * param:
 * N/A
 * return:
 * uint8_t variable that tells the program if a key has been pressed
 * on the keypad
 *************************************************************/

uint8_t Read_Keypad()
{
    uint8_t col, row;
    for (col = 0; col < 3; col++ )
    {
        P3->DIR = 0x00; // Set Columns to inputs
        P3->DIR |= BIT(5+col); // Set column to output
        P3->OUT &= ~BIT(5+col); // Set column to LOW
        delay_ms(10); // Delay the while loop
        row = P4->IN & 0x0F; // read all rows
        while (!(P4IN & BIT0) | !(P4IN & BIT1) | !(P4IN & BIT2) | !(P4IN & BIT3) );

        if (row != 0x0F)
            break; // if one of the input is low, some key is pressed.
    }

    P3->DIR = 0x00; // Set Columns to inputs on P3.5, P3.6, and P3.7
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

/****| T32_INT2_IRQHandler | *************************************
 * Brief: This function handles the IRQ for the interrupts in
 * the 32 bit timer and reloads the counts with half second
 * periodic one-shot count downs
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void T32_INT2_IRQHandler(void) //Interrupt Handler for Timer 2.
{
    ADC14->CTL0 |=1; //start a conversion
    while(!ADC14->IFGR0); // wait until conversion complete
    Vin = ADC14->MEM[5]; // immediately store value in a variable
    ADCInterrupt = 1; //set the global flag to tell the main() function that an interrupt has occured
    TIMER32_2->INTCLR = 1; //Clear interrupt flag so it does not interrupt again immediately.
    TIMER32_2->LOAD = 750750; //Set to a countdown of 0.25 second on 3 MHz clock
}

/****| randNum | ********************************************
 * Brief: This function generates a random number to initiate
 * the "hunt" of the monsters
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/
uint16_t randNum()
{
    int upper = 20;
    int lower = 0;
    int count = 10;
    int rand_num;
    int i;
    for (i = 0; i < count; i++) {
        rand_num = (rand() % (upper - lower + 1)) + lower;
        return rand_num;
    }
}

/****| T32_INT1_IRQHandler | *************************************
 * Brief: This function handles the IRQ for the interrupts in
 * the 32 bit timer and reloads the counts with half second
 * periodic one-shot count downs
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void T32_INT1_IRQHandler(void) //Interrupt Handler for Timer 2.
{
    //if the user did not close the door before the timer expired
    if(alarmFlag == 1)
    {
        endFlag = 1;
        //turn off buzzer sound
    }
    TIMER32_1->INTCLR = 1; //Clear interrupt flag so it does not interrupt again immediately.
}
