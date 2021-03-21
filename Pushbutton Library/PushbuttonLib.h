/************************************************************
Author: Cayla Stout
Course: EGR 226 - 902
Project: Pushbutton Library
Description: This .h file prototypes the functions for initializing
and debouncing the pushbuttons used in a project's circuit
 *************************************************************/

#ifndef PUSHBUTTONLIB_H_
#define PUSHBUTTONLIB_H_


void InitializeButtons(void);
int DebouncePress(void);
int DebounceRelease(void);



#endif /* PUSHBUTTONLIB_H_ */
