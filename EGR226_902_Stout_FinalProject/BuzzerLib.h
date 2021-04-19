#include <TimerLibInterrupts.h>
#include "msp.h"

#ifndef BUZZERLIB_H_
#define BUZZERLIB_H_

void Tone(uint16_t tone);
uint16_t frequencyConvert(uint16_t frequency);
void noTone(void);


#endif /* BUZZERLIB_H_ */