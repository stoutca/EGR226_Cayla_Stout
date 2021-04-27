/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Project: Final Project
Description: This .h file contains  the library for all LED, motor,
pushbutton, and keypad pin initializations
 *************************************************************/

#ifndef PININITLIB_H_
#define PININITLIB_H_
#include "msp.h"

void keypad_init(void);
void PushbuttonPinInit(void);
void motorPinInit(void);
void LEDPinInit(void);


#endif /* PININITLIB_H_ */
