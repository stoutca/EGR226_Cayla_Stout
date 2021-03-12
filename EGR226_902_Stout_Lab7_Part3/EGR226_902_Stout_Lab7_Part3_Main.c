/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/3/2021
Project: Lab 7 Part 2 Interfacing the MSP432 with a Liquid Crystal Display (LCD)
- Scrolling Marquee
Description: This printed the string "Laboratory Over" across the
LCD first line and then scrolled it at 0.5 second intervals
to the left.
 *************************************************************/

#include "msp.h"
#include "LCD_Lib.h" //include LCD library file to initialize and run LCD
#include "TimerLib.h" //include timer library
#include <string.h> //include string library


void main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    SysTick_Init();
    LCDPinInit(); //call function to initialize the pins to the LED

    LCD_init(); //call function to initialize the LCD

    char fullString[] = "Laboratory Over"; //line of text to print on fist line of LCD
    int i,j,k,l,m;
    char tempString[17];
    char wrapFullString[] = "                Laboratory Over"; //string that will scroll in from the left side of the screen
    strcpy(tempString, fullString); //copy the strings
    char tempString2[17];
    char tempString3[33];
    strcpy(tempString3, wrapFullString);
    char bufferString[] = "                "; //line string buffer
    delay_ms(60); //delay 60 milliseconds before first command

    char tempChar;
    printString(fullString); //print the string to the LCD
    delay_ms(500); //delay half of a second before the shifting starts)

    //printing the texts to the LCD in order of the lines
    while(1)
    {

        for(k = 0; k < 15; k++)
        {
            for(j = 0; fullString[j] != '\0'; j++)
            {
                if(fullString[j+1] != '\0')
                    fullString[j] = fullString[j + 1]; //shift the whole array up to the left 1 place
                else
                {
                    fullString[j] = ' ';
                }
            }
            commandWrite(0x01); //clear display and set cursor to home
            delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position
            printString(fullString);
            delay_ms(500); //wait half a second before displaying next line
        }
        strcpy(fullString, tempString); //reset the original string

        commandWrite(0x01); //clear the LCD and set cursor to the home position
        delay_ms(5); //5 millisecond delay needed to fully set the cursor to the home position
        printString(bufferString); //print the buffer
        delay_ms(500); //delay half of a second to make 1 second delay between text scrolling

        for(l = 0; l < 16; l++)
        {
            //loop to shift the wrapFullString to the left by one character
            for(i = 0; wrapFullString[i] != '\0'; i++)
            {
                if(wrapFullString[i+1] != '\0') //if the end of the string is not the next assignment for shifting the array
                    wrapFullString[i] = wrapFullString[i+1]; //shift the second string to the left
                else
                    wrapFullString[i] = ' '; //save the character before the null character as a space
            }
            //loop to save the first 16 characters of wrapFullString and print them after wrapFullString has been shifted
            for(m = 0; m < 16; m++)
            {
                tempString2[m] = wrapFullString[m]; //assign the current 16 values of the long string to the temporary string
            }
            if(tempString2[16] != '\0') //if the 16th element in the string is not already a null character
            {
                tempChar = tempString[16]; //save the 17th character in the string to a temporary character
                tempString2[16] = '\0'; //save the 16th element (the last element) as a null character
            }
            commandWrite(0x01); //clear the screen
            delay_ms(5); //delay needed to reset the LCD and not have garbage print
            printString(tempString2); //print the temporary string that holds the first 16 characters of the long string
            delay_ms(500);
            tempString[16] = tempChar; //reset the 16th character
        }

        strcpy(wrapFullString, tempString3); //copy the original wrapFullString back into wrapFullString from tempString3
    }
}


