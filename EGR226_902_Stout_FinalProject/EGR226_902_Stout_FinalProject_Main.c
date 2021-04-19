/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 4/21/2021
Project: Final Project
Description: This program will run a security system with a
door that can be opened or closed and LEDs that can be turned
on or off. The project is based off the game Five Nights at
Freddy's, so at random times, an alarm will sound, signifying
that a "monster" is at the door. If the user doesn't close
the door within 5 seconds, then they lose. If the user survives
until the 7 segment LED counts to 6, then they win. Either way,
the user can choose to try again and the program will reset.
 *************************************************************/

/*******States for switch states*******************************
 * 0 - main menu state
 * 1 - door menu state
 * 2 - motor menu state
 * 3 - LED menu state
 * 4 - Open Door Menu
 * 5 - Close Door Menu
 * 6 - Motor Menu State
 * 7 - Lights Menu
 * 8 - Red LED Menu
 * 9 - Blue LED Menu
 * 10 - Green LED Menu
 * 11 - Lose State
 * 12 - Win State
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
#include <BuzzerLib.h>

void LEDPinInit(void);
void motorPinInit(void);
void ADCSetup(void);
void keypad_init(void);
uint8_t Read_Keypad(void);
uint16_t randNum(); //random number generator
void PushbuttonPinInit(void);


volatile uint16_t Vin; //global variable for storing the value of the ADC conversion, which happens on a SysTick interrupt
volatile uint32_t ADCInterrupt, alarmFlag, endFlag, alarmSound, firstTime, RGBLightFlagOn, RGBOn; //global variable flag for ADC timer32 interrupt, flag to sound alarm when "monster" attacks from vent
uint8_t num, pressed; //Global variables for storing the number that was pressed on the keypad and whether or not the keypad had been pressed
volatile float power = 100; //variable for the overall power. Battery drains:
//-.05/cycle for whenever a door is closed
//-.03/cycle for whenever the LEDs are used
//-.02/cycle for whenever the fan is used


void main(void)
{
    int state;

    int stateChangeflag = 1; //flag to tell program when a state change has occured. This helps prevent blinking on the LCD

    uint16_t rand;

    RGBOn = 0; //start with the RGB LEDs off

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
    PushbuttonPinInit(); //initialize the pushbuttons
    motorPinInit();
    keypad_init(); //initialize the keypad

    LCD_init(); //initialize the LCD
    Timer32_Init(); //initialize timer 32

    //turn off LEDs, but leave P1.6 on as system starts armed
    P1->OUT |= BIT7; //turn off green led
    P1->OUT &= ~BIT6; //turn on red LED

    //buzzer pin initialization
    P7->SEL0 |= BIT4;
    P7->SEL1 &= ~BIT4;
    P7->DIR |= BIT4; // P2.4 set TA1.4

    //close door - armed initially
    int openDoor = 1; //flag to tell program that the door is initially open

    //strings for main menu
    char powerString[17]; //string to display text on line 1
    char doorOption[] = " Option 1: Door "; //print on line 1
    char motorOption[] = "Option 2: Motor "; //print on line 2
    char lightOption[] = "Option 3: Lights"; //print on line 3

    //strings for door menu
    char doorMenu[] = "    Door Menu   "; //text for first line of door menu
    char openDoorOption[] = " Option 1: Open "; //second line of door menu
    char closeDoorOption[] = "Option 2: Close "; //third line of door menu

    //strings for lights menu
    char lightMenu[] = "   Lights Menu  "; //first line
    char redLEDOption[] = "     Red LED    "; //second line
    char greenLEDOption[] = "    Green LED   "; //third line
    char blueLEDOption[] = "    Blue LED    "; // fourth line

    //LED PWM Enter Prompt
    char LEDPrompt[] = "Please Enter DC";
    int userInput[3];
    int tempNum1, tempNum2; //variable to temporarily hold the value of num
    int count = 0;
    int invalidInput = 0; //flag to catch if the user entered invalid input
    uint16_t finalDC; //variable to hold final input of user for PWM
    uint16_t finalDCCount;
    uint16_t LEDPeriod;
    char inval[] = " DC Not Valid  ";

    //Variables for servo
    uint16_t Period_Servo = setPeriod(50); //variable to hold the period of the 50Hz servo
    uint16_t DC_Servo = setDutyCycle(0.05, Period_Servo); //set the duty cycle at 5% to start the servo at a 1ms period for the 0 degree position
    timerA2Init(Period_Servo); //set the timer A2 module for the servo motor and DC motor to run off of. This will be the period for TA2.1 and TA2.2
    timerA21Init(DC_Servo); //start with the servo closed to start

    //Variables for PWM LCD Backlight
    float DC_LCD1;
    uint16_t DC_LCD; //variable to hold final duty cycle values
    uint16_t Period_LCD = setPeriod(200); //variable to hold period of 200 Hz signal
    timerA0Init(Period_LCD); //set timerA0 module for RBG LEDs and LCD brightness
    timerA04Init(Period_LCD); //start with the LCD at full brightness

    //turn off RGB LED ports
    timerA01Init(0);
    timerA02Init(0);
    timerA03Init(0);

    //RGB Previous PWM Variables if LEDs are turned off
    uint16_t prevPWMRed, prevPWMGreen, prevPWMBlue;

    //piezzo buzzer start as off
    timerA1Init(0); //make the period 0
    timerA14Init(0);
    int noteCounts; //variable for holding the frequency of the note in clock cycle counts for 3 MHz clock

    state = 0; //start the system in state 0, which is the "main menu" state
    delay_ms(60); //allow 60 milliseconds for LCD to set

    //initialize NVIC interrupts for certain pins
    NVIC_EnableIRQ(T32_INT2_IRQn); //Enable Timer32_2 interrupt.
    NVIC_EnableIRQ(T32_INT1_IRQn); //Enable Timer32_1 interrupt.
    NVIC_EnableIRQ(PORT3_IRQn); //enable port 3 interrupts on NVIC

    __enable_irq(); //enable global interrupts

    while(1)
    {
        //if the alarm is on and the interrupt has triggered for the 5 second expiration
        if(alarmSound == 1)
        {
            noteCounts = frequencyConvert(16); //convert low C into clock cycle counts
            Tone(noteCounts); //play the pitch
            delay_ms(300); //delay 0.3 second to play low pitch
            noTone(); //turn off alarm sound
            alarmSound = 0; //reset sound flag
        }

        //check if the door is closed and the alarm is sounding
        if((openDoor == 0) && (alarmFlag == 1))
        {
            alarmFlag = 0; //tell the program the door is closed, so there is no alarm
        }

        //if the interrupt was triggered for the pushbutton turning the LEDs off
        if(RGBLightFlagOn == 0)
        {
            RGBOn = 0; //tell the program that the LEDs are turning off

            if(firstTime == 1)
            {
                //save the current PWM into previous variables
                prevPWMGreen = TIMER_A0->CCR[1];
                prevPWMRed = TIMER_A0->CCR[2];
                prevPWMBlue = TIMER_A0->CCR[3];

                timerA01Init(0);
                timerA02Init(0);
                timerA03Init(0);
                firstTime = 0;
            }
        }

        //if the interrupt was triggered for the pushbutton to turn the LEDs back on
        else if(RGBLightFlagOn == 1)
        {
            RGBOn = 1;

            if(firstTime == 1)
            {
                //reset the PWM
                TIMER_A0->CCR[1] = prevPWMGreen;
                TIMER_A0->CCR[2] = prevPWMRed;
                TIMER_A0->CCR[3] = prevPWMBlue;
                firstTime = 0;
            }
        }


        //check if the user selected the option to try again, reset all beginning variables
        if(tryAgainFlag == 1)
        {
            endFlag = 0; //start the program with the ending flag at 0

            randGenerated = 0;

            tryAgainFlag = 0; //flag to tell the program if the user has used the program once before and is running through it again

            DC_Servo = setDutyCycle(0.05, Period_Servo); //set the duty cycle at 5% to start the servo at a 1ms period for the 0 degree position

            timerA21Init(DC_Servo); //start with the servo closed to start

            state = 0; //start the system in state 0, which is the "main menu" state

            //turn off LEDs, but leave P1.6 on as system starts armed
            P1->OUT |= BIT7; //turn on green led
            P1->OUT &= ~BIT6; //turn off red LED
        }
        //check if systick ADC interrupt flag has triggered
        if(ADCInterrupt == 1)
        {
            ADCInterrupt = 0; //reset interrupt flag
            DC_LCD1 = (Vin * 0.0002)/3.3; //save the decimal value of Vin to the duty cycle for the LCD by converting Vin to a voltage and dividing it by the reference voltage 3.3V
            DC_LCD = setDutyCycle(DC_LCD1, Period_LCD); //Get the duty cycle, in counts, for the adjusted LCD brightness
            timerA04Init(DC_LCD); //set the new DC for the PWM controlling the LCD brightness

            //if the percent duty cycle is between 25 and 50%
            if(.25 <= DC_LCD1 <= 0.5)
            {
                power = power - 0.002; //drain the power slightly
            }
            //if the percent duty cycle is between 50 and 75%
            else if(0.5 < DC_LCD1 <= 0.75)
            {
                power = power - 0.005; //drain the power slightly more
            }

            //if the percent duty cycle is between 75 and 100%
            else if(0.75 < DC_LCD1 <= 1)
            {
                power = power - 0.008; //drain the power even more
            }
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
                //start alarm sound
                //use frequency of note and convert to clock cycles based on 3 MHz SMCLK
                noteCounts = frequencyConvert(932);

                //send to tone function with duration
                Tone(noteCounts);
                //user has 5 seconds to close door
                Timer32_1_Init(); //start the one-shot 5 second timer
                rand = 0;
                randGenerated = 1; //reset the flag to 1 so no new numbers will be generated
            }
            //if the random number was not 21, continue looking for a random number in the next cycle
        }

        //if the user "died"
        if(endFlag == 1)
        {
            //state = ("death state");
        }

        //drain power according to condition
        //if door is closed
        if(openDoor == 0)
        {
            power = power - .05; //drain the power
        }

        //if the lights are on, drain power
        if(RGBOn == 1)
        {
            power = power - 0.03;
        }

        sprintf(powerString,"  Power: %0.1f%%  ", power); //save the power value into the powerString

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
                printString(powerString);

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
            commandWrite(0x80); //move cursor to home position
            printString(powerString); //print the updated power value
            commandWrite(0xA0); //move cursor off screen
            delay_ms(50); //delay 50 ms to make the change happen

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
                    state = 3;
                    stateChangeflag = 1; //set this flag to alert the program that the state has changed
                }

                //if the input from the keypad was not acceptable because it was not 1, 2, or 3
                else if((num!=1) || (num!=2) || (num!=3))
                {
                    state = 0; //stay in the same state and but don't set changestate flag
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

            //trip flag to tell program door is open
            openDoor = 1;

            //move to state 1 to main menu
            state = 0;
            stateChangeflag = 1;

            break;

            //Close Door State
        case 5:
            openDoor = 0; //tell the program the door is closed
            //If the door is open and the alarm is sounding
            if((openDoor == 0) && (alarmFlag == 1))
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

            //Light Menu State
        case 3:
            if(stateChangeflag == 1) //if the state has been changed
            {
                //clear screen and move cursor to home position
                commandWrite(0x01); //clear display and set cursor to home
                delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position

                //Display options for lights menu state:
                printString(lightMenu);

                //Option 1: Red LED
                commandWrite(0xC0); //line 2
                printString(redLEDOption);

                //Option 2: Green Door
                commandWrite(0x90); //line 3
                printString(greenLEDOption);

                //Option 3: Blue Door
                commandWrite(0xD0); //line 3
                printString(blueLEDOption);

                delay_ms(100); //delay 500 ms so the LCD can print everything and so the user cannot press buttons more than 2 times a second
                stateChangeflag = 0; //reset the flag so the program knows that the state has been activated
            }

            //don't accept any input unless it is a 1 or 0 - keep accepting input until desired number is entered
            pressed = Read_Keypad(); //get keypad input
            if(pressed)
            {
                //if 1 is pressed, change state to 6 - red LED state
                if(num == 1)
                {
                    state = 6;
                    stateChangeflag = 1;
                }

                //if 2 is pressed, change state to 7 - green LED state
                else if(num == 2)
                {
                    state = 7;
                    stateChangeflag = 1;
                }

                //if 3 is pressed, change state to 8 - blue LED state
                else if(num == 3)
                {
                    state = 8;
                    stateChangeflag = 1;
                }
                //if neither 1 nor 2 nor 3 was pressed
                else if((num!=1) || (num!=2) || (num != 3))
                {
                    state = 3; //stay in the current state
                }

            }

            break;


            //red LED state
        case 6:
            invalidInput = 1;
            if(stateChangeflag == 1)
            {
                commandWrite(0x01); //clear display and set cursor to home
                delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position

                //print prompt to LCD
                printString(LEDPrompt);
                delay_ms(100); //delay 100 milliseconds for LCD to update
            }

            //for getting 1-3 numbers, use a while loop until the # key is pressed (num would == 12). Then check if there were 4 numbers entered
            do
            {
                pressed = Read_Keypad(); // Call Function to read Keypad
                if (pressed) //if a key is pressed
                {
                    if((num != 10) && (count <= 3) && (num != 12)) //if the user didn't enter a star or pound sign as number input and the input is less than 3 numbers
                    {
                        if(num != 11) //if zero is not pressed
                            userInput[count] = num; //save the value of num into the array
                        else if(num == 11) //if zero is pressed
                            userInput[count] = 0; //save the value of zero into the array instead of 11
                    }
                    if((num != 12) && (num != 10)) //if the pound sign wasn't entered yet and the num is not a star
                        count++; //increment count
                    if(count >= 3)//if count becomes higher than 3, meaning the user is entering more than 3 numbers
                    {
                        if(num == 11)//if zero was pressed
                            num = 0; //set the value of num to actually be zero
                        if((num != 10)&&(num !=12)&&(count > 3)) //if the user is not attempting to enter a star or include pound in the array and the actual number of digits entered is greater than 4, not equal to 4
                        {
                            tempNum1 = userInput[1]; //save the array values into temporary variables
                            tempNum2 = userInput[2];
                            userInput[2] = num; //save the most recent number to the previous most recent value
                            userInput[1] = tempNum2;  //save the second most recent number to the userInput[1] spot
                            userInput[0] = tempNum1; //save the third most recent number to the userInput[0] spot
                        }
                    }
                    else if(num == 12) //if the pound key was entered
                    {
                        invalidInput = 0; //reset the validity flag to zero so input is valid again. This will be checked before printing the input
                    }


                }
                delay_ms(10); // 10ms delay through the loop before reading keypad again
            }
            while(num != 12); //while num doesn't equal 12 (the # sign), print the numbers and store them into a number array

            //put contents of array into one number
            if((userInput[2] < 10))
                finalDC = ((userInput[0]*100) + (userInput[1]*10) + (userInput[2]));
            else
                finalDC = ((userInput[0]*10) + (userInput[1]));

            //check if the number is between 0 and 100
            if(finalDC > 100)
                invalidInput = 1;

            //if the input was valid, update the duty cycle of the LED
            if(invalidInput == 0)
            {
                LEDPeriod = setPeriod(200);
                finalDCCount = setDutyCycle(finalDC, LEDPeriod);
                prevPWMRed = finalDCCount; //set duty cycle
            }

            //if the input was above 100
            else if(invalidInput == 1)
            {
                commandWrite(0x01); //clear display and set cursor to home
                delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position

                //print prompt to LCD
                printString(inval);
                delay_ms(3000); //delay 3 seconds milliseconds for user to see message
            }

            //move back to main menu
            state = 0;
            stateChangeflag = 1; //alert program that a state change is happening

            break;

        }
    }
}

/****| LEDPinInit | ********************************************
 * Brief: This function initializes the LEDs and LED LCD backlight
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

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
    P2->DIR |= BIT7; // P2.4 set TA0.4

    //RBG pin init to PWM
    //Green LED
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~BIT4;
    P2->DIR |= BIT4; // P2.4 set TA0.1

    //Red LED
    P2->SEL0 |= BIT5;
    P2->SEL1 &= ~BIT5;
    P2->DIR |= BIT5; // P2.5 set TA0.2

    //Blue LED
    P2->SEL0 |= BIT6;
    P2->SEL1 &= ~BIT6;
    P2->DIR |= BIT6; // P2.6 set TA0.3
}

/****| motorPinInit | ********************************************
 * Brief: This function initializes the servo and DC motors attached to
 * P5.6 and P5.7
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void motorPinInit()
{
    //DC Motor

    //Servo Motor
    P5->SEL0 |= BIT6;
    P5->SEL1 &= ~(BIT6);
    P5->DIR |= BIT6; // P5.4 set TA2.1
}

/****| PushbuttonPinInit | ********************************************
 * Brief: This function initializes the servo and DC motors attached to
 * P5.6 and P5.7
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/
void PushbuttonPinInit()
{
    P3->SEL0 &= ~BIT2; //set up P3.2 as GPIO, input, pull-up resistor
    P3->SEL1 &= ~BIT2;
    P3->DIR &= ~BIT2; //input
    P3->REN |= BIT2;
    P3->OUT |= BIT2;
    P3->IES |= BIT2; //Set pin interrupt to trigger from high to low (starts high due to pull up resistor)
    P3->IE |= BIT2; //Set pin interrupt on for P3.2

    P3->SEL0 &= ~BIT3; //set up P3.3 as GPIO, input, pull-up resistor
    P3->SEL1 &= ~BIT3;
    P3->DIR &= ~BIT3;
    P3->REN |= BIT3;
    P3->OUT |= BIT3;
    P3->IES |= BIT3; //Set pin interrupt to trigger from high to low (starts high due to pull up resistor)
    P3->IE |= BIT3; //Set pin interrupt on for P3.3

    P3->IFG = 0; //clear all port 3 interrupt flags
}

/****| PORT3_IRQHANDLER | ******************************************
 * Brief: This function set up interrupt handler for port 3, where
 * buttons 1 and 2 are both connected
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/
void PORT3_IRQHandler(void)
{
    uint8_t debounce = 0; //variable to help debounce button presses

    if((P3->IN & BIT2) || (P3->IN & BIT3)) //debounce the button presses by making sure that the button is held down and still pressed for at least 100 cycles
    {
        delay_ms(100);
        if((P3->IN & BIT2) || (P3->IN & BIT3))
        {
            debounce = 1;
        }
        else
            debounce = 0;
    }

    if(debounce)
    {
        if(P3->IFG & BIT2) //if button 1 was pushed, setting the interrupt on P3.2, turn off the RGB lights and save their PWM
        {
            //if the RGB lights start as on
            if(RGBOn == 1)
            {
                RGBLightFlagOn = 0;
                firstTime = 1; //flag to trigger the change of the PWM
            }
            //else if the LEDs are off
            else if(RGBOn == 0)
            {
                RGBLightFlagOn = 1;
                firstTime = 1; //flag to trigger the change of the PWM
            }
        }
    }

    if(P3->IFG & BIT3) //if button 2 was pushed, setting interrupt on P3.3
    {
        //set flag to stop the motor
    }

    P3->IFG = 0; //clear all port 3 interrupt flags
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
 * uint16_t type random number between 0 and 20
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
    }
    return rand_num;
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

void T32_INT1_IRQHandler(void) //Interrupt Handler for Timer 1.
{
    //if the user did not close the door before the timer expired
    if(alarmFlag == 1)
    {
        endFlag = 1;
        alarmFlag = 0;
    }
    alarmSound = 1; //turn off alarm sound
    //turn off buzzer sound
    TIMER32_1->INTCLR = 1; //Clear interrupt flag so it does not interrupt again immediately.
}
