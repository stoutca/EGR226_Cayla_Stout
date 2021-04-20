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
 * uint16_t type variables tone
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
 * uint16_t type variables tone
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

/****| noTone |**************************************
 * Brief: This function stops the tune coming from the
 * Piezzo buzzer
 * N/A
 * return:
 * N/A
 * ***************************************************/

void noTone(void)
{
    timerA14Init(0); //stop the period so the note frequency stops
    delay_ms(100);
}


/****| winTune |**************************************
 * Brief: This function plays a sequence of notes, C,
 * Ab, Bb, Eb, Eb, Bb, C, and Ab, for the happy
 * win ending coming from the Piezzo buzzer
 * N/A
 * return:
 * N/A
 * ***************************************************/

void winTune()
{
    int noteCounts;

    //High C
    noteCounts = frequencyConvert(523);
    Tone(noteCounts);
    delay_ms(850); //delay for the length of the note
    noTone(); //delay no tone for 100 milliseconds
    delay_ms(100); //delay on no music for 100 ms

    //Ab
    noteCounts = frequencyConvert(415);
    Tone(noteCounts);
    delay_ms(850); //delay for the length of the note
    noTone(); //delay no tone for 100 milliseconds
    delay_ms(100); //delay on no music for 100 ms

    //Bb
    noteCounts = frequencyConvert(466);
    Tone(noteCounts);
    delay_ms(850); //delay for the length of the note
    noTone(); //delay no tone for 100 milliseconds
    delay_ms(100); //delay on no music for 100 ms

    //Eb
    noteCounts = frequencyConvert(311);
    Tone(noteCounts);
    delay_ms(1300); //delay for the length of the note
    noTone(); //delay no tone for 100 milliseconds

    delay_ms(700); //delay for 700 ms

    //Eb
    noteCounts = frequencyConvert(311);
    Tone(noteCounts);
    delay_ms(850); //delay for the length of the note
    noTone(); //delay no tone for 100 milliseconds

    //Bb
    noteCounts = frequencyConvert(466);
    Tone(noteCounts);
    delay_ms(850); //delay for the length of the note
    noTone(); //delay no tone for 100 milliseconds
    delay_ms(100); //delay on no music for 100 ms

    //High C
    noteCounts = frequencyConvert(523);
    Tone(noteCounts);
    delay_ms(850); //delay for the length of the note
    noTone(); //delay no tone for 100 milliseconds
    delay_ms(100); //delay on no music for 100 ms

    //Ab
    noteCounts = frequencyConvert(415);
    Tone(noteCounts);
    delay_ms(1500); //delay for the length of the note
    noTone(); //delay no tone for 100 milliseconds
    delay_ms(1000); //delay on no music for 100 ms
}

/****| deathTune |**************************************
 * Brief: This function plays a sequence of notes, Bb,
 * A, Bb, Eb, and G for the bad, or death, ending
 * coming from the Piezzo buzzer
 * N/A
 * return:
 * N/A
 * ***************************************************/

void deathTune()
{
    int noteCounts;
    //Bb
    noteCounts = frequencyConvert(466);
    Tone(noteCounts);
    delay_ms(700); //delay for the length of the note
    noTone(); //delay no tone for 100 milliseconds
    delay_ms(100); //delay on no music for 100 ms

    //A
    noteCounts = frequencyConvert(440);
    Tone(noteCounts);
    delay_ms(700); //delay for the length of the note
    noTone(); //delay no tone for 100 milliseconds
    delay_ms(100); //delay on no music for 100 ms

    //Ab
    noteCounts = frequencyConvert(415);
    Tone(noteCounts);
    delay_ms(700); //delay for the length of the note
    noTone(); //delay no tone for 100 milliseconds
    delay_ms(100); //delay on no music for 100 ms

    //G
    noteCounts = frequencyConvert(196);
    Tone(noteCounts);
    delay_ms(2000); //delay for the length of the note
    noTone(); //delay no tone for 100 milliseconds
    delay_ms(100); //delay on no music for 100 ms
}


