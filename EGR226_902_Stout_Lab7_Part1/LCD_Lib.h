/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/3/2021
Project: Library for LCD Functions
RGB LEDs on the MSP432
Description: This .h file contains the function prototypes
to work the LCD on the MSP432 using P4.1 for the RS pin,
P4.2 for the E pin, and P4.4 - P4.7 for DB4-DB7
*************************************************************/

#ifndef LCD_LIB_H_
#define LCD_LIB_H_

void LCD_init(void); //function to initialize the LCD
void PulseEnablePin(void); //function to sequence the enable pin (E)
void pushNibble(uint8_t nibble); //pushes 1 nibble to D4-D7 ports on LCD
void pushByte(uint8_t byte); // This function first pushes 4 bits at a time to the LCD
void commandWrite(uint8_t command); //This functions calls the pushByte() function with one byte of command parameter to write a command to the LCD
void dataWrite(uint8_t data); //This function writes one byte of data by calling pushBute() function with data parameter
void LCDPinInit(void); //function to initialize the pins used for DB4-DB7



#endif /* LCD_LIB_H_ */
