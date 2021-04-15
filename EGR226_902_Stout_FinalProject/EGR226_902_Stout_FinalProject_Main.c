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
 * 5 - Disarm pin state
 * 6 - Correct pin entered, unarm door state
 * 7 - Alarm on state
 * 8 - Alarm audio on state
 * 9 - Alarm audio off state
 * 10 - Close Door Menu
 * 11 - Prompt to arm door state
 * 12 - Leave door unarmed state
 * 13 - Check User Pin for arming door state
 * 14 - Arm closed door state
 * 15 -
 *
 *
 **************************************************************/
//needed libraries
#include "msp.h"
#include <LCD_Lib.h>
#include <string.h> //include the string library
#include <stdio.h>
#include <TimerLibInterrupts.h> //include timer interrupt library

void LEDPinInit(void);
void motorPinInit(void);
void ADCSetup(void);
void keypad_init(void);
uint8_t Read_Keypad(void);


volatile float Vin; //global variable for storing the value of the ADC conversion, which happens on a SysTick interrupt
volatile uint32_t num, ADCInterrupt; //global variable to use with the keypad, flag for ADC systick interrupt

void main(void)
{
    int state;

    //time initializations
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    SysTick_Init(); //initialize the systick timer
    ADCSetup(); //set up the ADC
    Timer32_Init(); //initialize timer 32

    //Pin Initializations
    LCDPinInit(); //initialize the pins for the LCD
    LEDPinInit(); //initialize pins for the LED
    motorPinInit();
    keypad_init(); //initialize the keypad

    LCD_init(); //initialize the LCD

    //turn off LEDs, but leave P2.0 on as system starts armed

    //close door - armed initially
    int armed = 1; //flag to keep track of if the door is armed
    int alarmSound = 0; //flag to tell if the alarm is beeping or not
    int alarmFlag = 0; //flag to tell if alarm is on or not
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

    //disarming/arming door strings
    char disarmPin[] = "5675"; //pin to arm and disarm the door
    char promptPin1[] = "  Please enter  "; //prompt to user on first line
    char promptPin2[] = "The Disarm Pin: "; //prompt to user on second line
    char acceptedPin[] = "Pin Accepted :)"; //print on LCD if pin is accepted
    char alarmOnString1[] = "Alarm On: Enter "; //if pin was wrong, print on first line
    char alarmOnString2[] = " Pin to Disarm  "; //if pin was wrong, pring on second line

    //strings for rearming/leaving disarmed closed door
    char armOption1[] = "  1) Enter Pin  "; //print on line 1
    char armOption2[] = "2) Leave Unarmed"; //print on line 2

    //Variables for servo
    int Period_Servo = setPeriod(50); //variable to hold the period of the 50Hz servo
    float DC_Servo;
    timerA2Init(Period_Servo); //set the timer A2 module for the servo motor and DC motor to run off of. This will be the period for TA2.1 and TA2.2
    timerA21Init(0); //start with the servo off to start

    //Variables for PWM LCD Backlight
    float DC_LCD1;
    int DC_LCD; //variables to hold preliminary and final duty cycle values
    int Period_LCD = setPeriod(100); //variable to hold period of 100 Hz signal
    timerA0Init(Period_LCD); //set timerA0 module for RBG LEDs and LCD brightness
    timerA04Init(Period_LCD); //start with the LCD at full brightness

    state = 0; //start the system in state 0, which is the "main menu" state
    delay_ms(60); //allow 60 milliseconds for LCD to set

    //initialize NVIC interrupts for certain pins
    NVIC_EnableIRQ( T32_INT2_IRQn ); //Enable Timer32_2 interrupt.
    __enable_irq(); //enable global interrupts

    while(1)
    {
        //check if systick ADC interrupt flag has triggered
        if(ADCInterrupt == 1)
        {
            ADCInterrupt = 0;
            DC_LCD1 = (Vin * 0.0002)/3.3; //save the decimal value of Vin to the duty cycle for the LCD
            DC_LCD = setDutyCycle(DC_LCD1, Period_LCD); //Get the duty cycle, in counts, for the adjusted LCD brightness
            timerA04Init(DC_LCD); //set the new DC for the PWM controlling the LCD brightness
        }

        switch(state)
        {

        //main menu state
        case(0):
                        //clear the LED and move the cursor back to the home scren
                        commandWrite(0x01); //clear display and set cursor to home
        delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position

        //print the main menu options to the LCD:
        printString(mainMenuString);

        //Option 1: Door menu
        commandWrite(0xC0);
        printString(doorOption);

        //Option 2: Motor Menu
        commandWrite(0x90);
        printString(motorOption);

        //Option 3: Lights Menu
        commandWrite(0xD0);
        printString(motorOption);

        //if 1 is pressed on keypad, change state to 1 - Door Menu


        //if 2 is pressed on keypad, change state to 2 - Motor Menu


        //if 3 is pressed on keypad, change state to 3 - Lights Menu

        break;

        //Door Menu State
        case(1):
                            //clear screen and move cursor to home position


                            //Display options for door menu state:
                            //Option 1: Open Door


                            //Option 2: Close Door


                            //don't accept any input unless it is a 1 or 0 - keep accepting input until desired number is entered
                            //if 1 is pressed, change state to 4 - open door state


                            //if 2 is pressed, change state to 10 - close door state

                            break;

        //Open Door State
        case(4):

                            //clear screen, move cursor to home position

                            //move servo to closed position - 180 degrees


                            //move to state 5 to prompt user to disarm door

                        break;

        //Disarm Door State
        case(5):
                            //Check armed flag. If true,
                            //Prompt user to enter the right pin to disarm door. Pin is 5675


                            //Check input pin, compare to real pin
                            //Start 5 second timer with timer32


                            //if pin is wrong, continue the timer


                            //if pin is correct, stop timer, switch to state 6 - unarmed door state


                            //if timer reaches 5 seconds before correct pin is entered, move to state 7 - alarm on state


                            //if the armed flag was false, switch to state 7 to turn on alarm

                        break;

        //unarmed door state
        case(6):

                            //Clear screen, move cursor to home position


                            //print acceptance string to LCD on first line


                            //set the flag to sound the alarm to 0


                            //turn on Green Onboard LED and turn off red


                            //if the alarm sound was going off, go to state 9 - alarm audio off state

                break;

        //alarm on state
        case(7):

                            //Clear screen of LCD, move cursor to home position


                            //print to the LCD that the alarm is on


                            //set alarmFlag to 1 to turn on alarm system


                            //set armed flag to 1 to tell program that they system is still armed


                            //change state to state 8 - alarm audio on state
                break;

        //alarm on state
        case(8):

                            //check the alarmFlag value. If 1, turn on alarmSound


                            //trip alarmSound flag, setting it to 1 to alert program that the piezzo buzzer is making noise


                            //change state to state 5 - disarming with pin state to prompt the user for the pin more

                break;

        //alarm audio off state
        case(9):

                            //check alarmSound flag. If true, set it to 0


                            //also stop the alarm sound


                            //change state to the main menu state 0
                            //state = 0;
                break;

        //Close door state
        case(10):

                            //clear screen and reset cursor in home position

                            //If the door was open before choosing this option:
                            //move servo to 0 degrees by sending 5% duty cycle to servo PWM, closes door
                            //turn on Green LED

                            //change state to 11 - Prompting the user to arm the door
                break;

        //prompting the user to arm the door state
        case(11):

                            //print the menu options to the LCD:
                            //Option 1: Enter the pin to arm the door


                            //Option 2: Leave the door unarmed


                            //if 1 is entered, change state to 13 - accept and check pin


                            //if 2 is entered, change state to 12 - set door to unarmed

                break;

        //Leave door unarmed state
        case(12):

                            //clear LCD screen and move cursor to home position


                            //set the armed flag to 0 to signify that the door is not armed


                            //turn P2.0 off, turn on P2.1

                            //change state to main menu state 0
                            //state = 0;
                break;

        //check user pin for disarming door state
        case(13):

                            //clear LCD screen and move cursor to home position


                            //Prompt user for pin. Don't accept any input until a viable option is entered


                            //Check the input and compare it to the real pin - 5675


                            //If pin was correct, move to state 14 - setting door to armed state


                            //if pin was not correct, stay in state
                            //state = 13;
                break;

        //armed and closed door state
        case(14):

                            //Clear screen


                            //set armed flag to 1 to tell system that the door is closed and armed


                            //Turn off green LED, turn on red LED

                            //change state to main menu state 0
                            //state = 0;
                break;


        case(15):


                break;
        }
    }
}

void LEDPinInit()
{
    //P2.0 red onboard LED


    //P2.1 green onboard LED

    //LCD Background PWM pin
    P2->SEL0 &= ~BIT7;
    P2->SEL1 &= ~(BIT7);
    P2->DIR |= BIT7; // P2.4 set TA2.1

}

void motorPinInit()
{
    //DC Motor

    //Servo Motor
    P5->SEL0 |= BIT6;
    P5->SEL1 &= ~(BIT6);
    P5->DIR |= BIT4; // P2.4 set TA2.1
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
        row = P5->IN & 0xF0; // read all rows
        while (!(P5IN & BIT4) | !(P5IN & BIT5) | !(P5IN & BIT6) | !( P5IN & BIT7) );

        if (row != 0xF0)
            break; // if one of the input is low, some key is pressed.
    }

    P3->DIR = 0x00; // Set Columns to inputs
    if ( col == 3)
        return 0;
    if (row == 0xE0)
        num = col + 1; // key in row 0
    if (row == 0xD0)
        num = 3 + col +1; // key in row 1
    if (row == 0xB0)
        num = 6 + col +1; // key in row 2
    if (row == 0x70)
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
    TIMER32_2->INTCLR = 1; //Clear interrupt flag so it does not interrupt again immediately.
    ADC14->CTL0 |=1; //start a conversion
    while(!ADC14->IFGR0); // wait until conversion complete
    Vin = ADC14->MEM[5]; // immediately store value in a variable
    ADCInterrupt = 1;
    TIMER32_2->LOAD = 1501501; //Set to a countdown of 3 second on 3 MHz clock
}



