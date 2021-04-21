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
 * M - main menu state
 * D - door menu state
 * X - motor menu state
 * L - LED menu state
 * O - Open Door Menu
 * C - Close Door Menu
 * L - Lights Menu
 * r - Red LED Menu
 * b - Blue LED Menu
 * g - Green LED Menu
 * d - Lose State (death state)
 * A - Try Again State
 * W - Win State
 * P - Power out state
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
#include <PinInitLib.h>

uint8_t Read_Keypad(void);
uint16_t randNum(); //random number generator

volatile uint16_t Vin; //global variable to be read from ADC interrupt handler
volatile uint16_t attacks = 0; //global variable for storing how many times the user has been "attacked"
volatile uint32_t ADCInterrupt, alarmFlag, endFlag, alarmSound;
volatile uint32_t firstTime, RGBLightFlagOn, RGBOn, stopMotorflag, powerDownFlag; //global variables used in interrupts to handle various functions from the sensors
uint8_t num, pressed; //global variables for the keypad presses
volatile uint32_t openDoor = 0; //volatile variable for whether or not the door is closed. Starts closed
volatile float power = 100; //variable for the overall power. Battery drains slowly depending on what the user has on

void main(void)
{
    char state;

    uint16_t stateChangeflag = 1; //flag to tell program when a state change has occurred. This helps prevent blinking on the LCD

    uint16_t rand;

    RGBOn = 0; //start with the RGB LEDs off

    endFlag = 0; //start the program with the ending flag at 0

    uint16_t randGenerated = 0; //variable tell program when to generate a random number

    uint16_t tryAgainFlag = 0; //flag to tell the program if the user has used the program once before and is running through it again

    uint16_t pwmChangeflag = 0; //flag to tell the program that the keypad was used to change PWM of the RGB LEDs

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
    P1->OUT &= ~BIT7; //turn off green led
    P1->OUT |= BIT6; //turn on red LED

    //buzzer pin initialization
    P7->SEL0 |= BIT4;
    P7->SEL1 &= ~BIT4;
    P7->DIR |= BIT4; // P7.4 set TA1.4

    //strings for main menu
    char powerString[17]; //string to display text on line 1
    char doorOption[] = " Option 1: Door "; //print on line 1
    char motorOption[] = "Option 2: Motor"; //print on line 2
    char lightOption[] = "Option 3: Lights"; //print on line 3

    //strings for door menu
    char doorMenu[] = "    Door Menu   "; //text for first line of door menu
    char openDoorOption[] = " Option 1: Open "; //second line of door menu
    char closeDoorOption[] = "Option 2: Close "; //third line of door menu

    //strings for lights menu
    char lightMenu[] = "   Lights Menu  "; //first line
    char redLEDOption[] = "   1) Red LED   "; //second line
    char greenLEDOption[] = "  2) Green LED  "; //third line
    char blueLEDOption[] = "  3) Blue LED   "; // fourth line

    //RGB LED PWM variables and strings
    char DCPrompt[] = "Please Enter DC";
    int userInput[4];
    userInput[0] = 0; //initialize the user input array as zero to prevent random numbers from interfering with calculations of duty cycle later on
    userInput[1] = 0;
    userInput[2] = 0;
    userInput[3] = '\0';

    //variables for RGB LED PWM
    int tempNum1, tempNum2; //variable to temporarily hold the value of num
    int count = 0;
    int invalidInput = 0; //flag to catch if the user entered invalid input
    float finalDC; //variable to hold final input of user for PWM
    uint16_t finalDCCount;
    uint16_t LEDPeriod;
    char inval[] = " DC Not Valid  ";

    //Variables for servo
    uint16_t Period_Servo = setPeriod(50); //variable to hold the period of the 50Hz servo
    uint16_t DC_Servo = setDutyCycle(0.1, Period_Servo); //set the duty cycle at 10% to start the servo at a 2ms period for the closed degree position
    timerA2Init(Period_Servo); //set the timer A2 module for the servo motor and DC motor to run off of. This will be the period for TA2.1 and TA2.2
    timerA21Init(DC_Servo); //start with the servo closed to start

    //variables for motor menu
    float DC_Motor = 0; //initialize the PWM as 0 to start
    timerA22Init(0); //set the duty cycle
    uint16_t finalDCMotor; //variable to hold the final duty cycle of the motor PWM in clock counts
    int motorOnflag = 0; //flag to keep track if the motor is running or not. 0 to start

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

    //RGB Previous PWM Variables if LEDs are turned off. Start at 0
    uint16_t prevPWMRed = 0;
    uint16_t prevPWMGreen = 0;
    uint16_t prevPWMBlue = 0;

    //piezzo buzzer start as off
    timerA1Init(0); //make the period 0
    timerA14Init(0);
    int noteCounts; //variable for holding the frequency of the note in clock cycle counts for 3 MHz clock

    //Game over characters
    char gameOver[] = "  Game Over :(  "; //print on first line
    char tryAgain[] = "   Try Again?   "; //print on second line after delay
    char gameOverOpt1[] = "  1) Try Again  "; //print on third line
    char gameOverOpt2[] = " 2) Give Up >:( "; //print on fourth line

    //Winner strings
    char congrats[] = "Congratulations!"; //first line
    char win[] = "A Winner is You!"; //second line
    char happyTryAgain[] = "   Try Again?   ";
    char happyOptions[] = "  1) Yes, 2) No ";

    state = 'M'; //start the system in state 0, which is the "main menu" state
    delay_ms(60); //allow 60 milliseconds for LCD to set

    //initialize NVIC interrupts for certain pins
    NVIC_EnableIRQ(T32_INT2_IRQn); //Enable Timer32_2 interrupt.
    NVIC_EnableIRQ(T32_INT1_IRQn); //Enable Timer32_1 interrupt.
    NVIC_EnableIRQ(PORT3_IRQn); //enable port 3 interrupts on NVIC

    __enable_irq(); //enable global interrupts

    while(1)
    {
        //ADC conditionals
        //check if timer32 ADC interrupt flag has triggered
        if((ADCInterrupt == 1) && (powerDownFlag == 0))
        {
            ADCInterrupt = 0; //reset interrupt flag
            DC_LCD1 = (Vin * 0.0002)/3.3; //save the decimal value of Vin to the duty cycle for the LCD by converting Vin to a voltage and dividing it by the reference voltage 3.3V
            DC_LCD = setDutyCycle(DC_LCD1, Period_LCD); //Get the duty cycle, in counts, for the adjusted LCD brightness
            timerA04Init(DC_LCD); //set the new DC for the PWM controlling the LCD brightness
        }

        //state conditionals
        //if the user has been attacked 5 times and survived - 5 times since the attack variable updates twice every time it enters the interrupt
        if(attacks == 10)
        {
            //switch to win state
            state = 'W';
            stateChangeflag = 1; //tell program that the state is changing
        }
        //if the user ran out of power
        if((power < 1) && (state != 'A') && (state != 'W'))
        {
            powerDownFlag = 1;
            state = 'P'; //change state to power down state
            power = 0; //set power to 0
        }

        //if the user "died" and the try again screen and winning screen are not trying to display
        if((endFlag == 1) && (state != 'A') && (state != 'W'))
        {
            state = 'd'; //death state
        }

        //check if the user selected the option to try again, reset all beginning variables
        if(tryAgainFlag == 1)
        {
            endFlag = 0; //start the program with the ending flag at 0

            randGenerated = 0; //reset the random number generator

            tryAgainFlag = 0; //flag to tell the program if the user has used the program once before and is running through it again

            DC_Servo = setDutyCycle(0.1, Period_Servo); //set the duty cycle at 10% to start the servo at a 2ms period for the 180 degree position

            timerA21Init(DC_Servo); //start with the servo closed to start

            //reset motor to 0
            timerA22Init(0);

            power = 100; //power reset

            attacks = 0; //reset attack counter

            state = 'M'; //start the system in state 0, which is the "main menu" state

            stateChangeflag = 1; //tell program that the state is changing

            powerDownFlag = 0;

            openDoor = 0; //tell program door is closed

            motorOnflag = 0;

            //turn off LEDs, but leave P1.6 on as system starts armed
            P1->OUT &= ~BIT7; //turn off green led
            P1->OUT |= BIT6; //turn on red LED

            //turn off RGB LED ports
            timerA01Init(0);
            timerA02Init(0);
            timerA03Init(0);

            RGBOn = 0; //tell program RGB pins are off

            //set variables that hold the previous PWM as zero too
            prevPWMRed = 0;
            prevPWMGreen = 0;
            prevPWMBlue = 0;

            userInput[0] = 0; //initialize the user input array as zero to prevent random numbers from interfering with calculations of duty cycle later on
            userInput[1] = 0;
            userInput[2] = 0;
            userInput[3] = '\0';
        }

        //RGB Light conditionals
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
            if((firstTime == 1) && (powerDownFlag == 0)) //if it is the first time through the program and the power is still on
            {
                //reset the PWM
                TIMER_A0->CCR[1] = prevPWMGreen;
                TIMER_A0->CCR[2] = prevPWMRed;
                TIMER_A0->CCR[3] = prevPWMBlue;
                firstTime = 0;
                RGBOn = 1; //tell program that the lights are back on
            }
        }

        //if the PWM of the LEDs was changed and the LEDs are on (seperate function from pushbutton interrupt flags)
        if((pwmChangeflag == 1) && (RGBOn == 1) && (powerDownFlag == 0))
        {
            TIMER_A0->CCR[1] = prevPWMGreen;
            TIMER_A0->CCR[2] = prevPWMRed;
            TIMER_A0->CCR[3] = prevPWMBlue;
        }

        //motor conditionals
        //if the user pressed the emergency stop button to turn the motor off
        if(stopMotorflag == 1)
        {
            stopMotorflag = 0; //reset flag
            motorOnflag = 0; //tell program the motor is off
            timerA22Init(0); //set the duty cycle to zero
        }


        //alarm conditionals
        //if the alarm is on and the interrupt has triggered for the 5 second expiration
        if(alarmSound == 1)
        {
            noteCounts = frequencyConvert(16); //convert low C into clock cycle counts
            Tone(noteCounts); //play the pitch
            delay_ms(300); //delay 0.3 second to play low pitch
            noTone(); //turn off alarm sound
            alarmSound = 0; //reset sound flag
        }

        //if a number hasn't been generated yet
        if(randGenerated == 0)
        {
            srand(time(0));
            rand = randNum();
            randGenerated = 1; //tell the program a number has been generated
        }

        //if a random number is generated and the try again state and win state aren't trying to display
        if((randGenerated) && (state != 'A') && (state != 'W'))
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

        //check if the door is closed and the alarm is sounding
        if((openDoor == 0) && (alarmFlag == 1))
        {
            alarmFlag = 0; //tell the program the door is closed, so there is no alarm
        }

        //power conditionals
        //drain power according to condition
        //if the LCD percent duty cycle is between 25 and 50%
        if((.25 <= DC_LCD1 <= 0.5) && (powerDownFlag == 0))
        {
            power = power - 0.002; //drain the power slightly
        }
        //if the percent duty cycle is between 50 and 75%
        else if((0.5 < DC_LCD1 <= 0.75) && (powerDownFlag == 0))
        {
            power = power - 0.005; //drain the power slightly more
        }

        //if the percent duty cycle is between 75 and 100%
        else if((0.75 < DC_LCD1 <= 1) && (powerDownFlag == 0))
        {
            power = power - 0.008; //drain the power even more
        }

        //if door is closed
        if((openDoor == 0) && (powerDownFlag == 0))
        {
            power = power - .1; //drain the power
        }

        //if the lights are on, drain power. If the program is restarting or just powering on so all the previous PWMs are zero, don't drain the power as nothing is on
        if((RGBOn == 1) && (powerDownFlag == 0) && ((prevPWMRed != 0) || (prevPWMBlue != 0) || (prevPWMGreen != 0)))
        {
            power = power - 0.08;
        }

        //if fan (motor) is on, drain power
        if((motorOnflag == 1) && (powerDownFlag == 0))
        {
            power = power - 0.1;
        }

        sprintf(powerString,"  Power: %0.1f%%  ", power); //save the power value into the powerString

        switch(state)
        {

        //main menu state
        case 'M':
            if((stateChangeflag == 1) && (powerDownFlag == 0))
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
            if(powerDownFlag == 0)
            {
                commandWrite(0x80); //move cursor to home position
                printString(powerString); //print the updated power value
                commandWrite(0xA0); //move cursor off screen
                delay_ms(50); //delay 50 ms to make the change happen

                pressed = Read_Keypad();
                if((pressed) && (powerDownFlag == 0)) //if the keypad is pressed and there's still power
                {
                    //if 1 is pressed on keypad, change state to 1 - Door Menu
                    if(num == 1)
                    {
                        state = 'D';
                        stateChangeflag = 1;
                    }

                    //if 2 is pressed on keypad, change state to 2 - Motor Menu
                    else if (num == 2)
                    {
                        state = 'X';
                        stateChangeflag = 1; //set this flag to alert the program that the state has changed
                    }

                    //if 3 is pressed on keypad, change state to 3 - Lights Menu
                    else if(num == 3)
                    {
                        state = 'L';
                        stateChangeflag = 1; //set this flag to alert the program that the state has changed
                    }

                    //if the input from the keypad was not acceptable because it was not 1, 2, or 3
                    else if((num!=1) || (num!=2) || (num!=3))
                    {
                        state = 'M'; //stay in the same state and but don't set changestate flag
                    }
                }
            }

            break;

            //Door Menu State
        case 'D':
            if((stateChangeflag == 1) && (powerDownFlag == 0)) //if the state has been changed
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
            if((pressed) && (powerDownFlag == 0))
            {
                //if 1 is pressed, change state to 4 - open door state
                if(num == 1)
                {
                    state = 'Y';
                    stateChangeflag = 1;
                }

                //if 2 is pressed, change state to 5 - close door state
                else if(num == 2)
                {
                    state = 'C';
                    stateChangeflag = 1;
                }

                //if neither 1 nor 2 was pressed
                else if((num!=1) || (num!=2))
                {
                    state = 'D'; //stay in the current state
                }

            }

            break;

            //Open Door State
        case 'Y':
            //if the power isn't off yet
            if(powerDownFlag == 0)
            {
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
                state = 'M';
                stateChangeflag = 1;
            }

            break;

            //Close Door State
        case 'C':
            //if the power isn't off yet
            if(powerDownFlag == 0)
            {
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
                state = 'M';
            }

            break;

            //Light Menu State
        case 'L':
            if((stateChangeflag == 1) && (powerDownFlag == 0)) //if the state has been changed
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
            if((pressed) && (powerDownFlag == 0))
            {
                //if 1 is pressed, change state to 6 - red LED state
                if(num == 1)
                {
                    state = 'r';
                    stateChangeflag = 1;
                }

                //if 2 is pressed, change state to 7 - green LED state
                else if(num == 2)
                {
                    state = 'g';
                    stateChangeflag = 1;
                }

                //if 3 is pressed, change state to 8 - blue LED state
                else if(num == 3)
                {
                    state = 'b';
                    stateChangeflag = 1;
                }
                //if neither 1 nor 2 nor 3 was pressed
                else if((num!=1) || (num!=2) || (num != 3))
                {
                    state = 'L'; //stay in the current state
                }

            }

            break;


            //red LED state
        case 'r':

            //if the power is still on
            if(powerDownFlag == 0)
            {
                count = 0; //reset count value for user input
                //clear userInput array
                userInput[0] = 0;
                userInput[1] = 0;
                userInput[2] = 0;
                userInput[3] = '\0';

                invalidInput = 0;
                if(stateChangeflag == 1)
                {
                    commandWrite(0x01); //clear display and set cursor to home
                    delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position

                    //print prompt to LCD
                    printString(DCPrompt);
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


                    }
                    delay_ms(10); // 10ms delay through the loop before reading keypad again
                }
                while(num != 12); //while num doesn't equal 12 (the # sign), print the numbers and store them into a number array

                //put contents of array into one number
                //if three or more numbers were entered by user:
                if(count >= 3)
                    finalDC = ((userInput[0]*100) + (userInput[1]*10) + (userInput[2])); //save first element in 100s place, 2nd element in 10s place, 3rd number in single digit place

                //if less than three numbers were entered by user - 2 numbers
                else if(count == 2)
                    finalDC = ((userInput[0])*10) + (userInput[1]); //save first and second array element as first number in tens place and second number in ones place

                //if one number was entered by user
                else if(count == 1)
                    finalDC = userInput[0]; //save first number of array as is, single digit number

                //check if the number is between 0 and 100
                if(finalDC > 100)
                    invalidInput = 1;

                //if the input was valid, update the duty cycle of the LED
                if(invalidInput == 0)
                {
                    LEDPeriod = setPeriod(200);
                    finalDC = finalDC/100; //convert this value to a decimal
                    finalDCCount = setDutyCycle(finalDC, LEDPeriod);
                    prevPWMRed = finalDCCount; //set duty cycle
                    pwmChangeflag = 1; //tell the program that the PWM of the RGB LEDs have changed
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
                state = 'M';
                stateChangeflag = 1; //alert program that a state change is happening
            }

            break;

            //green LED state
        case 'g':

            //if the power is still on
            if(powerDownFlag == 0)
            {
                count = 0; //reset the count of user inputs

                //clear userInput array
                userInput[0] = 0;
                userInput[1] = 0;
                userInput[2] = 0;
                userInput[3] = '\0';

                invalidInput = 0; //start assuming the input is valid

                if(stateChangeflag == 1)
                {
                    commandWrite(0x01); //clear display and set cursor to home
                    delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position

                    //print prompt to LCD
                    printString(DCPrompt);
                    delay_ms(100); //delay 100 milliseconds for LCD to update
                    stateChangeflag = 0;
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

                    }
                    delay_ms(10); // 10ms delay through the loop before reading keypad again
                }
                while(num != 12); //while num doesn't equal 12 (the # sign), print the numbers and store them into a number array

                //put contents of array into one number
                //if three or more numbers were entered by user:
                if(count >= 3)
                    finalDC = ((userInput[0]*100) + (userInput[1]*10) + (userInput[2])); //save first element in 100s place, 2nd element in 10s place, 3rd number in single digit place

                //if less than three numbers were entered by user - 2 numbers
                else if(count == 2)
                    finalDC = ((userInput[0])*10) + (userInput[1]); //save first and second array element as first number in tens place and second number in ones place

                //if one number was entered by user
                else if(count == 1)
                    finalDC = userInput[0]; //save first number of array as is, single digit number


                //check if the number is between 0 and 100
                if(finalDC > 100)
                    invalidInput = 1;

                //if the input was valid, update the duty cycle of the LED
                if(invalidInput == 0)
                {
                    LEDPeriod = setPeriod(200);
                    finalDC = finalDC/100; //convert this value to a decimal
                    finalDCCount = setDutyCycle(finalDC, LEDPeriod);
                    prevPWMGreen = finalDCCount; //set duty cycle
                    pwmChangeflag = 1; //tell the program that the PWM of the RGB LEDs have changed
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
                state = 'M';
                stateChangeflag = 1; //alert program that a state change is happening
            }

            break;

            //blue LED state
        case 'b':
            //if the power is still on
            if(powerDownFlag == 0)
            {
                count = 0; //reset count variable
                //clear userInput array
                userInput[0] = 0;
                userInput[1] = 0;
                userInput[2] = 0;
                userInput[3] = '\0';

                invalidInput = 0;
                if(stateChangeflag == 1)
                {
                    commandWrite(0x01); //clear display and set cursor to home
                    delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position

                    //print prompt to LCD
                    printString(DCPrompt);
                    delay_ms(100); //delay 100 milliseconds for LCD to update
                    stateChangeflag = 0;
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


                    }
                    delay_ms(10); // 10ms delay through the loop before reading keypad again
                }
                while(num != 12); //while num doesn't equal 12 (the # sign), print the numbers and store them into a number array

                //put contents of array into one number
                //if three or more numbers were entered by user:
                if(count >= 3)
                    finalDC = ((userInput[0]*100) + (userInput[1]*10) + (userInput[2])); //save first element in 100s place, 2nd element in 10s place, 3rd number in single digit place

                //if less than three numbers were entered by user - 2 numbers
                else if(count == 2)
                    finalDC = ((userInput[0])*10) + (userInput[1]); //save first and second array element as first number in tens place and second number in ones place

                //if one number was entered by user
                else if(count == 1)
                    finalDC = userInput[0]; //save first number of array as is, single digit number

                //check if the number is between 0 and 100
                if(finalDC > 100)
                    invalidInput = 1;

                //if the input was valid, update the duty cycle of the LED
                if(invalidInput == 0)
                {
                    LEDPeriod = setPeriod(200);
                    finalDC = finalDC/100; //convert this value to a decimal
                    finalDCCount = setDutyCycle(finalDC, LEDPeriod);
                    prevPWMBlue = finalDCCount; //set duty cycle
                    pwmChangeflag = 1; //tell the program that the PWM of the RGB LEDs have changed
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
                state = 'M';
                stateChangeflag = 1; //alert program that a state change is happening
            }

            break;

            //state for taking motor input
        case 'X':
            if((stateChangeflag == 1) && (powerDownFlag == 0))
            {
                commandWrite(0x01); //clear display and set cursor to home
                delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position

                //print prompt to LCD
                printString(DCPrompt);
                delay_ms(100); //delay 100 milliseconds for LCD to update
                stateChangeflag = 0;
            }

            pressed = Read_Keypad(); //read the keypad
            if((pressed) && (powerDownFlag == 0)) //if it was pressed
            {
                if(num == 11) //if the user pressed the zero key
                    num = 0; //change the value of num to reflect the 0

                switch(num) //build switch statement based on the value of "num"
                {
                case 0:
                    DC_Motor = 0;
                    motorOnflag = 0;
                    break;

                case 1:
                    DC_Motor = 0.10;
                    motorOnflag = 1; //signify motor is on
                    break;

                case 2:
                    DC_Motor = 0.20;
                    motorOnflag = 1; //signify motor is on
                    break;

                case 3:
                    DC_Motor = 0.30;
                    motorOnflag = 1; //signify motor is on
                    break;

                case 4:
                    DC_Motor = 0.40;
                    motorOnflag = 1; //signify motor is on
                    break;

                case 5:
                    DC_Motor = 0.50;
                    motorOnflag = 1; //signify motor is on
                    break;

                case 6:
                    DC_Motor = 0.60;
                    motorOnflag = 1; //signify motor is on
                    break;

                case 7:
                    DC_Motor = 0.70;
                    motorOnflag = 1; //signify motor is on
                    break;

                case 8:
                    DC_Motor = 0.80;
                    motorOnflag = 1; //signify motor is on
                    break;

                case 9:
                    DC_Motor = 0.90;
                    motorOnflag = 1; //signify motor is on
                    break;

                }

                finalDCMotor = setDutyCycle(DC_Motor, Period_Servo); //get duty cycle in clock counts for 50Hz frequency
                timerA22Init(finalDCMotor); //set the PWM of the motor


                if((num != 10) && (num!=12)) //if the user didn't press the pound key or the astrick
                {
                    //change state
                    state = 'M';
                    stateChangeflag = 1;
                }
                else
                    state = 'X'; //stay in state
            }

            break;

            //death state - or endFlag state
        case 'd':
            rand = 0; //reset the rand number
            randGenerated = 1; //reset the random generator flag to not generate more numbers

            //mute current notes
            noTone();
            delay_ms(100); //delay for 100 milliseconds

            //display game over screen
            //move cursor to home position
            commandWrite(0x01);
            delay_ms(5);

            //play death sound
            deathTune();
            delay_ms(2000); //delay 2 seconds

            state = 'A'; //change to ask user if they want to play again
            stateChangeflag = 1;

            break;

            //try again state
        case 'A':

            //if the power was down, bring back the backlight PWM
            if(powerDownFlag == 1)
                timerA04Init(Period_LCD);

            if(stateChangeflag == 1)
            {
                commandWrite(0x01); //clear display and set cursor to home
                delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position

                //print the main menu options to the LCD:
                printString(gameOver);
                delay_ms(1000); //delay for 1 second

                //Option 1: Door menu
                commandWrite(0xC0); //line 2
                printString(tryAgain);

                //Option 2: Motor Menu
                commandWrite(0x90); //line 3
                printString(gameOverOpt1);

                //Option 3: Lights Menu
                commandWrite(0xD0); //line 4
                printString(gameOverOpt2);

                delay_ms(100); //give LCD a chance to print strings
                stateChangeflag = 0;
            }

            pressed = Read_Keypad();
            if(pressed) //if the keypad is pressed
            {
                //if 1 is pressed on keypad, tell program user wants to try again
                if(num == 1)
                {
                    tryAgainFlag = 1;
                    powerDownFlag = 0; //reset power down flag if it was tripped
                    state = 'M'; //set the state to the main state
                }

                //if 2 is pressed on keypad,
                else if (num == 2)
                {
                    exit(0); //exit the program
                }

                else if((num!=1) && (num!=2))
                {
                    state = 'A';
                }
            }

            break;

            //power down state
        case 'P':
            //turn off all LEDs
            timerA01Init(0);
            timerA02Init(0);
            timerA03Init(0);
            timerA04Init(0);

            //turn off motor and servo
            timerA21Init(375);
            timerA22Init(0);

            //turn off green and red LEDs
            P1->OUT &= ~BIT7; //turn off green led
            P1->OUT &= ~BIT6; //turn off red LED

            //set flag for power is down, nothing will work
            powerDownFlag = 1;

            state = 'M';

            break;

            //win state
        case 'W':

            rand = 0; //reset the rand number
            randGenerated = 1; //reset the random generator flag to not generate more numbers

            //mute current notes
            noTone();
            delay_ms(100); //delay for 100 milliseconds

            //clear screen and move mouse to home position
            if(stateChangeflag == 1)
            {
                commandWrite(0x01); //clear display and set cursor to home
                delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position

                //print the congrats message to user
                printString(congrats);

                commandWrite(0xC0); //line 2
                printString(win); //print the win options to user

                //play win sound
                winTune();

                //Prompt user if they want to play again
                commandWrite(0x90); //line 3
                printString(happyTryAgain);

                //Option 3: Lights Menu
                commandWrite(0xD0); //line 4
                printString(happyOptions);

                delay_ms(100); //give LCD a chance to print strings
                stateChangeflag = 0;

                attacks = 0; //reset attacks
            }

            pressed = Read_Keypad();
            if(pressed) //if the keypad is pressed
            {
                //if 1 is pressed on keypad, tell program user wants to try again
                if(num == 1)
                {
                    tryAgainFlag = 1;
                }

                //if 2 is pressed on keypad,
                else if (num == 2)
                {
                    exit(0); //exit the program
                }

                //stay in state if any number other than 1 or 2 was pressed
                else if((num!=1) && (num!=2))
                {
                    state = 'W';
                }


                break;
            }

        }
    }
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
        stopMotorflag = 1;
    }

    P3->IFG = 0; //clear all port 3 interrupt flags
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
    //if the user did not close the door before the timer expired or the power was down
    if((openDoor == 1) || (powerDownFlag == 1))
    {
        endFlag = 1;
        alarmFlag = 0; //turn off the alarm
    }
    //if user did close door in time
    else
        attacks++; //increment attacks  since the user survived
    alarmSound = 1; //turn off alarm sound
    //turn off buzzer sound
    TIMER32_1->INTCLR = 1; //Clear interrupt flag so it does not interrupt again immediately.
}
