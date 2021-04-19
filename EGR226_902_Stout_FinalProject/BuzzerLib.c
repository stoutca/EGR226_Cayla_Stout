#include "msp.h"
#include <stdio.h>
#include <stdlib.h>
#include <TimerLibInterrupts.h>

/****| Tone | ************************************************
 * Brief: This function plays a given note on the piezzo buzzer
 * given the period of the desired note frequency in clock
 * cycles (according to the TA1 SMCLK) and duration of the note
 * in milliseconds
 * param:
 * uint16_t type variables tone and duration
 * return:
 * N/A
 *************************************************************/
void Tone(uint16_t tone)
{
    timerA14Init(tone); //set the tone period as the timer period and PWM
}

/****| frequencyConvert |**************************************
 * Brief: This function converts the frequency of a given note
 * to clock cycle counts
 * param:
 * uint16_t type variables tone and duration
 * return:
 * uint16_t type variable of the note frequency in clock cycles
 *************************************************************/
uint16_t frequencyConvert(uint16_t frequency)
{
    uint16_t counts;
    float period_seconds = 1/(float)frequency; //set the period in seconds given the frequency in hertz
    counts = (float)period_seconds/(0.000000333);
    return counts;
}

void noTone(void)
{
    timerA14Init(0); //stop the period so the note frequency stops
    delay_ms(100);
}


