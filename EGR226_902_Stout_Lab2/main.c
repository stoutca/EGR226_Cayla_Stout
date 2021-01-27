/**************************************************************************************************
* Author: Cayla Stout
* Course: EGR 226 - 902
* Date: 01/27/2021
* Project: Lab 02 Part 1 - Resistor Analysis
* File: ResistorAnalysis.c
* Description: Application that prints Color-Code Scheme for 4-band resistors, accepts and error checks
user-input resistance, calculates the color codes needed for a 4-band resistor based on the user-input
resistance, and loops again if the user wants to input a new resistance.
***************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> //ctype.h library needed for error checking the user-defined resistance value
#include <math.h>  //the math.h library is needed for performing the pow() function in the calcResistorColors() function

//function prototypes for four custom functions in program
void prompt(void);
void calcResistorColors(int);

void getColorBands(char*, char*, char*, char*);
void calcResistance(char, char, char, char);

int main()
{
    int repeat_flag = 0; //flag to track if the user wants to loop through the program with a new resistance. While it is 0, the program will continue to loop
    int Resistance; //integer value to store the user resistance after it is error checked
    do
    {
    int acceptable_input = 1; //flag to track if the user has input valid input
    char userInput [100]; //string to store the user input initially
    char userResponse; //char to hold the user response for whether or not they want to enter a new resistance
    prompt(); //call to the function prompt(void);

    do
    {
    acceptable_input = 1; //the user flag starts off as 1, where 1 = acceptable input and 0 = unacceptable input

    fgets(userInput, 100, stdin); //command to accept get user input, up to 100 characters

    for(int i = 0; userInput[i] != NULL; i++) //a for loop to go through the user string to check for characters in the input
    {
      if((isalpha(userInput[i])) || (ispunct(userInput[i]))) //if the user input contains a character or punctuation mark, the flag is set to 0 for unacceptable input
        acceptable_input = 0;
    }
    if(acceptable_input == 1) //if none of the conditional statements were tripped above, meaning the input is an acceptable number
    {
        Resistance = atoi(userInput); //the userinput string is converted to an integer and assigned to the Resistance integer variable
        if((Resistance < 1 ) || (Resistance > 99000000)) //if the new integer value is less than 1 or greater than 99,000,000
        {
            acceptable_input = 0; //the flag is set to 0 for unacceptable input
        }
    }
    if(acceptable_input == 0) //if, after all conditionals, the input is found to be unacceptable, the following command prints to the screen and the user has to enter a new value
    {
        printf("Please input a valid Resistance between 1 and 99M: \n");
    }
    }while(acceptable_input == 0); //this loop will continue while the user input is unacceptable

    Resistance = atoi(userInput); //this is a check to make sure the acceptable string is converted to an integer and saved to Resistance

    calcResistorColors(Resistance); //function call to calculate and print the needed resistance colors

    printf("Would you like to try another Resistance value? Type X for yes, anything else will mean No. "); //after the color code is calculated, this prompts the user to enter a new resistance or end the program
    userResponse = getchar(); //the user input is saved to the char userResponse
    if(userResponse == 'x' || userResponse == 'X') //if the user entered x or X to enter a new resistance
    {
        repeat_flag = 0; //the flag remains unchanged
        getchar(); //this catches the enter key the user will press and prevents the error checking loop to prematurely prompt the user for acceptable input
    }
    else if(userResponse != 'x' || userResponse != 'X') //if the user enters any character besides x or X
    {
        repeat_flag = 1; //the flag is switched to 1 to end the program
    }
    }while(repeat_flag == 0); //the whole program will loop while the user opts to enter new resistance values

    printf("\nGoodbye.");
    return 0;
}

/****| prompt | **********************************************************************************
* Brief:
*   This function will print the color-code scheme for the user and prompts them to choose between
    converting a color-code to a resistance value or determining the color code for a resistance value
* param:
*   void, so no parameter inputs
* return:
*   type void, so no return value
***************************************************************************************************/
void prompt(void)
{
printf("--------------------Resistor Codes---------------------\n");
 printf("|Character| Color | Band 1 & 2 | Band 3 | Band 4 |\n");
 printf("|    K    | Black | 0 |*1         |+/- 1%%   |\n");
 printf("|    N    | Brown | 1 |*10        |+/- 2%%   |\n");
 printf("|    R    | Red   | 2 |*100       |          |\n");
 printf("|    O    | Orange| 3 |*1,000     |          |\n");
 printf("| Y | Yellow| 4 |*10,000    |          |\n");
 printf("| G | Green | 5 |*100,000   |+/- 0.5%% |\n");
 printf("| B | Blue  | 6 |*1,000,000 |+/- 0.25%%|\n");
 printf("| V | Violet| 7 |*10,000,000|+/- 0.1%% |\n");
 printf("| E | Grey  | 8 |           |+/- 0.05%%|\n");
 printf("| W | White | 9 |           |          |\n");
 printf("| D | Gold  |   |           |+/- 5%%   |\n");
 printf("| S | Silver|   |           |+/- 10%%  |\n");
 printf("-------------------------------------------------------\n\n");

    printf("Would you like to convert a color-code to a resistance value from the colors listed above(option 1) or determine the color-code of a resistance value between 1 and 99M ohms (option 2)? Enter 1 for option 1 or 2 for option 2:")

    printf("What resistance would you like color-coded? Enter a number between 1 and 99M ohms: "); //prompts user for a resistance value
}

/****| calcResistorColors | **********************************************************************************
* Brief:
*   This function will convert the user resistance into a color-code for the needed resistor.
* param:
*   (int)   n: 4 bytes worth of data input with the assigned name Resistance
* return:
*   type void, so no value is returned
***************************************************************************************************************/

void calcResistorColors(int Resistance)
{
    int multiplier = 0;
    int secondBandNumber;
    int firstBandNumber;
    int multiplierNumber;
    char firstBandColor[50];
    char secondBandColor[50];
    char multiplierColor[50];

    while(Resistance > 99) //while the Resistance is 100 or greater
    {
        Resistance = Resistance/10; //the resistance is divided by 10
        multiplier = multiplier + 1; //the number of times the resistance can be divided by 10 before becoming 2 digits reveals the multiplier (x10) the resistance has
    }

    multiplierNumber = pow(10,multiplier); //raises 10 to the power of the multiplier

    secondBandNumber = Resistance % 10; //to find the number for the second band, the resistance is divided by 10 and the remainder is assigned to secondbandnumber. This works because Resistance is now a 2-digit number
    firstBandNumber = (Resistance/10) % 10; //to find the firstBandNumber, the resistance is divided by 10 one more time to get a single digit number and the modulus of that is taken from 10

    //finding first band color. Depending on what the values of the bands are, the color will change.
    if(firstBandNumber == 0)
        strcpy(firstBandColor, "Black"); //i.e. if the firstBandNumber was found to be zero, the first digit in the number is 0 and the color of the band is black
    else if(firstBandNumber == 1)
        strcpy(firstBandColor, "Brown");
    else if(firstBandNumber == 2)
        strcpy(firstBandColor, "Red");
    else if(firstBandNumber == 3)
        strcpy(firstBandColor, "Orange");
    else if(firstBandNumber == 4)
        strcpy(firstBandColor, "Yellow");
    else if(firstBandNumber == 5)
        strcpy(firstBandColor, "Green");
    else if(firstBandNumber == 6)
        strcpy(firstBandColor, "Blue");
    else if(firstBandNumber == 7)
        strcpy(firstBandColor, "Violet");
    else if(firstBandNumber == 8)
        strcpy(firstBandColor, "Grey");
    else if(firstBandNumber == 9)
        strcpy(firstBandColor, "White");

    //finding second band color
    if(secondBandNumber == 0)
        strcpy(secondBandColor, "Black");
    else if(secondBandNumber == 1)
        strcpy(secondBandColor, "Brown");
    else if(secondBandNumber == 2)
        strcpy(secondBandColor, "Red");
    else if(secondBandNumber == 3)
        strcpy(secondBandColor, "Orange");
    else if(secondBandNumber == 4)
        strcpy(secondBandColor, "Yellow");
    else if(secondBandNumber == 5)
        strcpy(secondBandColor, "Green");
    else if(secondBandNumber == 6)
        strcpy(secondBandColor, "Blue");
    else if(secondBandNumber == 7)
        strcpy(secondBandColor, "Violet");
    else if(secondBandNumber == 8)
        strcpy(secondBandColor, "Grey");
    else if(secondBandNumber == 9)
        strcpy(secondBandColor, "White");

    //finding multiplier color
    if(multiplierNumber == 1)
        strcpy(multiplierColor, "Black");
    else if(multiplierNumber == 10)
        strcpy(multiplierColor, "Brown");
    else if(multiplierNumber == 100)
        strcpy(multiplierColor, "Red");
    else if(multiplierNumber == 1000)
        strcpy(multiplierColor, "Orange");
    else if(multiplierNumber == 10000)
        strcpy(multiplierColor, "Yellow");
    else if(multiplierNumber == 100000)
        strcpy(multiplierColor, "Green");
    else if(multiplierNumber == 1000000)
        strcpy(multiplierColor, "Blue");
    else if(multiplierNumber == 10000000)
        strcpy(multiplierColor, "Violet");

    //print
    printf("%s-%s-%s\n", firstBandColor, secondBandColor, multiplierColor); //prints the strings (colors) of the bands in order to the console

    return;
}
