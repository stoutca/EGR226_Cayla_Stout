/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Project: Final Project
Description: This .h file contains function prototypes to use
initialize and use the piezzo buzzer on a timerA pin
 *************************************************************/
#include <TimerLibInterrupts.h>
#include "msp.h"

#ifndef BUZZERLIB_H_
#define BUZZERLIB_H_

void Tone(uint16_t tone);
uint16_t frequencyConvert(uint16_t frequency);
void noTone(void);
void deathTune(void);
void winTune(void);


#endif /* BUZZERLIB_H_ */
