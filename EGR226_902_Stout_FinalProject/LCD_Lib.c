/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Date: 3/4/2021
Project: EGR 226 902
Description: This .c file contains  the library for all
functions that use the LCD, from initializing to sending data
to the LCD pins from the MSP 432 pins. P4.1 is for the RS pin,
P4.2 for the E pin, and P4.4 - P4.7 for DB4-DB7
*************************************************************/

#include "msp.h" //include msp library
#include "LCD_Lib.h" //include LCD library
#include "TimerLibInterrupts.h" //include timer library

/****| LCD_Init | *******************************************
 * Brief: This function initializes the LCD to tell it to
 * operate in 4-bit mode
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void LCD_init(void)
{
    commandWrite(0x3);
    delay_ms(10);
    commandWrite(0x3);
    delay_micro(200);
    commandWrite(0x3);
    delay_ms(10);
    commandWrite(0x2);
    delay_micro(100);
    commandWrite(0x28); //send 2 and then 8 since these commands are only 1 nibble wide
    delay_micro(100);
    commandWrite(0x0F);
    delay_micro(100);
    commandWrite(0x01);
    delay_micro(100);
    commandWrite(0x06);
    delay_micro(100);
}

/****| LCDPinInit | *******************************************
 * Brief: This function initializes the pins in the MSP that connects
 * to DB4-DB7. P4.4-P4.7 connects to DB4-DB7
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void LCDPinInit(void)
{
    //RS pin
    P6->SEL0 &= ~BIT0;
    P6->SEL1 &= ~BIT0; //configure P6.0 as GPIO
    P6->DIR |= BIT0; //configure the direction register of P6.0 to be output

    //Enable pin
    P6->SEL0 &= ~BIT1;
    P6->SEL1 &= ~BIT1; //configure P6.1 as GPIO
    P6->DIR |= BIT1; //configure the direction register of P6.1 to be output

    //DB4 pin
    P4->SEL0 &= ~BIT4;
    P4->SEL1 &= ~BIT4; //configure P4.4 as GPIO
    P4->DIR |= BIT4; //configure the direction register of P4.4 to be output

    //DB5 pin
    P4->SEL0 &= ~BIT5;
    P4->SEL1 &= ~BIT5; //configure P4.5 as GPIO
    P4->DIR |= BIT5; //configure the direction register of P4.5 to be output

    //DB6 pin
    P4->SEL0 &= ~BIT6;
    P4->SEL1 &= ~BIT6; //configure P4.6 as GPIO
    P4->DIR |= BIT6; //configure the direction register of P4.6 to be output

    //DB7 pin
    P4->SEL0 &= ~BIT7;
    P4->SEL1 &= ~BIT7; //configure P4.7 as GPIO
    P4->DIR |= BIT7; //configure the direction register of P4.7 to be output

}

/****| PulseEnablePin | *******************************************
 * Brief: This function pulses 0V, to HIGH, to 0V to the enable pin.
 * The enable pin is connected to P4.2
 * param:
 * N/A
 * return:
 * N/A
 *************************************************************/

void PulseEnablePin (void)
{
    P4->OUT &=~BIT2; // make sure pulse starts out at 0V from P4.2
    delay_micro(10); //delay 10 microseconds
    P4->OUT |=BIT2; //pulse the enable pin to 1
    delay_micro(10); //wait 10 microseconds
    P4->OUT &=~BIT2; //make pulse back to 0V
    delay_micro(10); //wait for 10 milliseconds
}

/****| pushNibble | *******************************************
 * Brief: This function receives a nibble (half of a byte) from
 * pushByte and sends the nibble to DB4-DB7
 * param:
 * type (uint8_t) variable called nibble, half of a byte from
 * pushByte
 * return:
 * N/A
 *************************************************************/

void pushNibble(uint8_t nibble)
{
    P4->OUT &=~0xF0; // clear P4.4-P4.7
    P4->OUT |= (nibble & 0x0F) << 4; // port pins P4.4 - P4.7 wired to D4 - D7, so shift the received nibble to the right 4 bits
    PulseEnablePin(); //call the function PulseEnablePin()
}

/****| pushByte | *******************************************
 * Brief: This function receives a byte from the commandWrite()
 * or dataWrite()
 * param:
 * type (uint8_t) variable called byte, 8 bits of instruction or data values
 * pushByte
 * return:
 * N/A
 *************************************************************/

void pushByte (uint8_t byte)
{
    uint8_t nibble;
    nibble = (byte & 0xF0) >> 4;
    pushNibble(nibble);
    nibble = byte & 0x0F;
    pushNibble(nibble);
    delay_micro(100);
}

/****| commandWrite | *******************************************
 * Brief: This function writes a byte command to the LCD and calls
 * pushByte to send the data
 * param:
 * type (uint8_t) variable called command, 8 bit command
 * return:
 * N/A
 *************************************************************/

void commandWrite(uint8_t command)
{
    P4->OUT &= ~BIT1; //set the RS pin to 0, RS pin is P4.1
    pushByte(command);
}

/****| dataWrite | *******************************************
 * Brief: This function writes a byte of data to the LCD using
 * 8 bits of data
 * param:
 * type (uint8_t) variable called command, 8 bit data value
 * return:
 * N/A
 *************************************************************/

void dataWrite(uint8_t data)
{
    P4->OUT |= BIT1; //set the RS pin to High, RS pin is P4.1
    pushByte(data);
}

/****| printString | *******************************************
 * Brief: This function breaks apart the string that the user wants
 * displayed on the LCD using a for loop.
 * param:
 * char array (string)
 * return:
 * N/A
 *************************************************************/

void printString(char Display[])
{
    int i; //variable to use in loop

    for(i = 0; Display[i] != '\0'; i++) //loop through the string as long as c and the address of the text *c doesn't become a null
    {
        dataWrite(Display[i]);//send the byte for the character to the dataWrite() function
    }
}




