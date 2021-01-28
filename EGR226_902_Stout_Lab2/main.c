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
#include <string.h> //include string library
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
    int colorCode = 0; //flag to determine if the user chose to color code a resistance value. 1 if true, 0 if false.
    int colorDecode = 0; //flag to determine if the user chose to decode a color combination to find a resistance value. 1 if true, 0 if false.
    do
    {
        int acceptable_input = 1; //flag to track if the user has input valid input
        char userInputResistance [100]; //string to store the user input initially
        char color1, color2, color3, color4; //variables to store the user's choice of color characters, should they opt to decode a color combination
        char userResponse; //char to hold the user response for whether or not they want to enter a new resistance
        prompt(); //call to the function prompt(void);
        int decodeChoice; //character variable to store the user's choice to decode a color combination or color code a resistor.

        do
        {
            scanf("%c",&decodeChoice); //read in the user choice and check if it is acceptable
            getchar(); //wait for user input
            if((isdigit(decodeChoice)) && ((decodeChoice == '1') || (decodeChoice == '2'))) //if the input is a digit and is 1 or 2
            {
                acceptable_input = 1;
                if(decodeChoice == '2')
                {
                    printf("\nPlease enter a resistance to be converted to colors that is between 1 and 99,000,000(inclusive): ");
                    do
                    {
                        acceptable_input = 1; //the user flag starts off as 1, where 1 = acceptable input and 0 = unacceptable input
                        fgets(userInputResistance, 100, stdin); //command to accept get user input, up to 100 characters

                        for(int i = 0; userInputResistance[i] != NULL; i++) //a for loop to go through the user string to check for characters in the input
                        {
                            if((isalpha(userInputResistance[i])) || (ispunct(userInputResistance[i]))) //if the user input contains a character or punctuation mark, the flag is set to 0 for unacceptable input
                                acceptable_input = 0;
                        }
                        if(acceptable_input == 1) //if none of the conditional statements were tripped above, meaning the input is an acceptable number
                        {
                            Resistance = atoi(userInputResistance); //the userinputResistance string is converted to an integer and assigned to the Resistance integer variable
                            if((Resistance < 1 ) || (Resistance > 99000000)) //if the new integer value is less than 1 or greater than 99,000,000
                            {
                                acceptable_input = 0; //the flag is set to 0 for unacceptable input
                            }
                        }
                        if(acceptable_input == 0) //if, after all conditionals, the input is found to be unacceptable, the following command prints to the screen and the user has to enter a new value
                        {
                            printf("Please input a valid Resistance between 1 and 99M: \n");
                        }
                    }while(acceptable_input == 0);  //this loop will continue while the user input is unacceptable

                    Resistance = atoi(userInputResistance); //this is a check to make sure the acceptable string is converted to an integer and saved to Resistance

                    calcResistorColors(Resistance); //function call to calculate and print the needed resistance colors

                }

                else if(decodeChoice == '1')
                {
                    printf("\nPlease enter a color combination using color characters from the above table in the form XXXX, where X is an UPPERCASE color character: ");
                    getColorBands(&color1, &color2, &color3, &color4);
                    calcResistance(color1, color2, color3, color4);
                    getchar();
                }
            }
            else //if the user initial choice was not a 1 or 2, prompt them to try again
                {
                    acceptable_input = 0;
                    printf("\nPlease enter a 1 or 2 to choose option 1 or 2: ");
                }

        }while(acceptable_input == 0);

        printf("Would you like to try another Resistance value or color combination? Type X for yes, anything else will mean No. "); //after the color code is calculated, this prompts the user to enter a new resistance or end the program
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
    }
    while(repeat_flag == 0);  //the whole program will loop while the user opts to enter new resistance values

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
    printf("|    K    | Black |      0     |*1         |+/- 1%%   |\n");
    printf("|    N    | Brown |      1     |*10        |+/- 2%%   |\n");
    printf("|    R    | Red   |      2     |*100       |          |\n");
    printf("|    O    | Orange|      3     |*1,000     |          |\n");
    printf("|    Y    | Yellow|      4     |*10,000    |          |\n");
    printf("|    G    | Green |      5     |*100,000   |+/- 0.5%% |\n");
    printf("|    B    | Blue  |      6     |*1,000,000 |+/- 0.25%%|\n");
    printf("|    V    | Violet|      7     |*10,000,000|+/- 0.1%% |\n");
    printf("|    E    | Grey  |      8     |           |+/- 0.05%%|\n");
    printf("|    W    | White |      9     |           |          |\n");
    printf("|    D    | Gold  |            |           |+/- 5%%   |\n");
    printf("|    S    | Silver|            |           |+/- 10%%  |\n");
    printf("-------------------------------------------------------\n\n");

    printf("Would you like to convert a color-code to a resistance value from the colors listed above (option 1) or determine the color-code of a resistance value (option 2)? Enter 1 for option 1 or 2 for option 2: ");

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

/****| getColorBands | **********************************************************************************
* Brief:
*   This function will collect user input of 4 characters, aligning with the character-to-color scheme printed
*   in prompt().
* param:
*   4 pointers to characters
* return:
*   the parameters were references, so this function will "return" all 4 values of user input
***************************************************************************************************/

void getColorBands(char *color1, char *color2, char *color3, char *color4)
{
    int acceptable_input = 1;
    do
        {
            acceptable_input = 1;//the user flag starts off as 1, where 1 = acceptable input and 0 = unacceptable input
            scanf("%c%c%c%c", color1, color2, color3, color4); //command to accept get user input, up to 100 characters

            if((isalpha(*color1)) && (isalpha(*color2)) && (isalpha(*color3)) && (isalpha(*color4))) //if all of the user inputs are characters
                    {
                        if((*color1 != 'K') && (*color1 != 'N') && (*color1 != 'R') && (*color1 != 'O') && (*color1 != 'Y') && (*color1 != 'G') && (*color1 != 'B') && (*color1 != 'V') && (*color1 != 'E') && (*color1 != 'W')) //error check for all values of band 1
                        {
                            acceptable_input = 0;
                        }
                        if((*color2 != 'K') && (*color2 != 'N') && (*color2 != 'R') && (*color2 != 'O') && (*color2 != 'Y') && (*color2 != 'G') && (*color2 != 'B') && (*color2 != 'V') && (*color2 != 'E') && (*color2 != 'W')) //error checking to see if color 2's input was not acceptable
                        {
                            acceptable_input = 0;
                        }
                        if((*color3 != 'K') && (*color3 != 'N') && (*color3 != 'R') && (*color3 != 'O') && (*color3 != 'Y') && (*color3 != 'G') && (*color3 != 'B') && (*color3 != 'V')) //error checking all input for band 3
                        {
                            acceptable_input = 0;
                        }
                        if((*color4 != 'K') && (*color4 != 'N') && (*color4 != 'G') && (*color4 != 'B') && (*color4 != 'V') && (*color4 != 'E') && (*color4 != 'D') && (*color4 != 'S')) //error checking all input for band 4
                        {
                            acceptable_input = 0;
                        }
                    }
            else
                acceptable_input = 0; //if the user input contains numbers or punctuation, it is not acceptable input

            if(acceptable_input == 0) //if, after all conditionals, the input is found to be unacceptable, the following command prints to the screen and the user has to enter a new value
                {
                    printf("Please input a valid color combination in the form XXXX, where X is an uppercase character from the color table: ");
                    getchar();
                }
        } while(acceptable_input == 0); //this loop will continue while the user input is unacceptable

        return; //return to the main function

}

/****| calcResistorColors | **********************************************************************************
* Brief:
*   This function will take 4 characters and decode them to find the resistance of the given color code.
* param:
*   4 values of type (char) that contain the 4 color characters
* return:
*   type void, so no value is returned
***************************************************************************************************************/


void calcResistance(char c1, char c2, char c3, char c4)
{
    float digit1;
    float digit2;
    float multiplier;
    char tolerance[10]; //array to hold the string for the tolerance
    float Resistance; //final calculated value of the Resistance
    float firstHalf; //variable to hold the first two numbers put together

    switch(c1) //switch case to cycle through all the possible values of c1 (color for band 1) to determine what the first number is
    {
    case 'K':
        digit1 = 0;
        break;

    case 'N':
        digit1 = 1;
        break;

    case 'R':
        digit1 = 2;
        break;

    case 'O':
        digit1 = 3;
        break;

    case 'Y':
        digit1 = 4;
        break;

    case 'G':
        digit1 = 5;
        break;

    case 'B':
        digit1 = 6;
        break;

    case 'V':
        digit1 = 7;
        break;

    case 'E':
        digit1 = 8;
        break;

    case 'W':
        digit1 = 9;
        break;
    }

    switch(c2) //look through all of the possibilities for colors of band 2 and print the corresponding digit
    {
        case 'K':
        digit2 = 0;
        break;

    case 'N':
        digit2 = 1;
        break;

    case 'R':
        digit2 = 2;
        break;

    case 'O':
        digit2 = 3;
        break;

    case 'Y':
        digit2 = 4;
        break;

    case 'G':
        digit2 = 5;
        break;

    case 'B':
        digit2 = 6;
        break;

    case 'V':
        digit2 = 7;
        break;

    case 'E':
        digit2 = 8;
        break;

    case 'W':
        digit2 = 9;
        break;
    }

    switch(c3) //switch case function to cycle through all the possible colors of the third band and find the multiplier
    {
        case 'K':
        multiplier = 1;
        break;

    case 'N':
        multiplier = 10;
        break;

    case 'R':
        multiplier = 100;
        break;

    case 'O':
        multiplier = 1000;
        break;

    case 'Y':
        multiplier = 10000;
        break;

    case 'G':
        multiplier = 100000;
        break;

    case 'B':
        multiplier = 1000000;
        break;

    case 'V':
        multiplier = 10000000;
        break;

    case 'D':
        multiplier = 0.1;
        break;

    case 'S':
        multiplier = 0.01;
        break;

    }

    switch(c4) //switch case to cycle through all the possible colors of the 4th band and assign the right string, the tolerance, to the string "tolerance"
    {
    case 'K':
        strcpy(tolerance, "+/- 1%");
        break;

    case 'N':
        strcpy(tolerance, "+/- 2%");
        break;

    case 'G':
        strcpy(tolerance, "+/- 0.5%");
        break;

    case 'B':
        strcpy(tolerance, "+/- 0.25%");
        break;

    case 'V':
        strcpy(tolerance, "+/- 0.1%");
        break;

    case 'E':
        strcpy(tolerance, "+/- 0.05%");
        break;

    case 'D':
        strcpy(tolerance, "+/- 5%");
        break;

    case 'S':
        strcpy(tolerance, "+/- 10%");
        break;
    }

    firstHalf = ((float)(digit1 * 10)) + ((float)digit2); //calculate what the first two digits are in the resistance. The first digit will always be a multiple of 10 higher than the second

    Resistance = firstHalf * multiplier; //multiply the first two digits in the resistance by the multiplier to get the final resistance

    printf("\nThe decoded resistance is: %g %s\n", Resistance, tolerance); //print the final decoded resistance and associated tolerance

    return;
}

