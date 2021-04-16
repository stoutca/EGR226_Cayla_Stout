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

#ifndef TIMERLIBINTERRUPTS_H_
#define TIMERLIBINTERRUPTS_H_
#include "msp.h"

void delay_ms(unsigned ms); //function to delay the program for a certain number of milliseconds using the Systick timer
void delay_micro(unsigned microsec); //function to delay the program for a certain number of microseconds
void SysTick_Init(void); //function to initialize SysTick timer
void timerA0Init(uint16_t T); //function prototype to initialize the Timer A modules
void timerA04Init(uint16_t T);
void timerA2Init(uint16_t T);
//void timerA01Init(uint16_t); //function prototype to initialize the Timer A modules
void timerA21Init(uint16_t T);
uint16_t setPeriod(uint16_t frequency);
void Timer32_Init(void);
uint16_t setDutyCycle(float, uint16_t T);
void stopMotor(void);

#endif /* TIMERLIBINTERRUPTS_H_ */
