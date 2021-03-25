/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Project: Library for Timer Functions
Description: This .h file contains the function prototypes
for the SysTick initialization and functions to delay the
program by millisecond or microsecond increments. It also
contains function prototypes for timerA initializations
and DC motor uses.
*************************************************************/

#ifndef TIMERLIB_H_
#define TIMERLIB_H_

void delay_ms(unsigned ms); //function to delay the program for a certain number of milliseconds using the Systick timer
void delay_micro(unsigned microsec); //function to delay the program for a certain number of microseconds
void SysTick_Init(void); //function to initialize SysTick timer
void timerAInit(int, int); //function prototype to initialize the Timer A modules
int setPeriod(int);
int setDutyCycle(float,int);
void stopMotor(void);

#endif /* TIMERLIB_H_ */
