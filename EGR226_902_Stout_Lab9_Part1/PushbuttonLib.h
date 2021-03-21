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
int DebouncePress1(void);
int DebounceRelease1(void);
int DebouncePress2(void);
int DebounceRelease2(void);
int DebouncePress3(void);
int DebounceRelease3(void);


#endif /* PUSHBUTTONLIB_H_ */
