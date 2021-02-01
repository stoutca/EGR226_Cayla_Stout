#include <stdio.h>
#include <stdlib.h>
#include <string.h> //include the string library to tokenize arrays

//defining the book structure
typedef struct{
    char title[255];
    char author_name[50];
    int pages;
    int year_published;
}book;

//custom function prototypes
int parse_file(char, char);
void print_book(book);
void search_title(char, int, char);
void search_author(char, int, char);
void search_ISBN(char, int, char);

int main()
{
    //array of book-structures
    book book_array[360];
    int acceptable_input = 1; //flag for user input
    int userChoice; //variable to hold user choice for searching through book database
    int applicationOn = 1; //flag to let program loop through book database endlessly inside of while loop
    int numBooks; //int variable to store the number of books from the file

    char filename[] = "BookList.txt";

    char Title[225]; //string for user input titles
    char author[50]; //string for user input authors
    char ISBNnum[50]; //string for user input of ISBN numbers

    //call parse_file() and send the filename and array of book structures
    parse_file(filename, book_array);
    for(int numBooks = 0; book_array[numBooks].title != NULL; numBooks++); //loop through array and get the number of books

    while(applicationOn == 1)
    {

    //prompt user to search for books by title, author, or ISBN
    printf("What would you like to search by? Enter [0] for Title, [1] for Author, or [2] for ISBN: ");
     do //do-while loop to check for user input
        {
            scanf("%c",&userChoice); //read in the user choice and check if it is acceptable
            getchar(); //wait for user input
            if((isdigit(userChoice)) && ((userChoice == 0) || (userChoice == '1') || (userChoice == '2'))) //if the input is a digit and is 1 or 2
            {
                acceptable_input = 1;
            }
            else
            {
                acceptable_input = 0;
                printf("\nPlease enter a 0, 1, or 2 to search the book database: ");
            }
        }while(acceptable_input == 0);


        if(userChoice == 0) //if the user chose to search by title
        {
            printf("\nPlease enter a title or key words to search the book titles by: ");
            fgets(Title, 225, stdin); //read in all of user input
            search_title(book_array, numBooks, Title); //call function to search the book database using the user title
        }

        else if(userChoice == 1) //if the user chose to search by author
        {
            printf("\nPlease enter an author name or letters of a name to search the books by: "); //prompt user to enter an author's name or letters to a name
            fgets(author, 50, stdin); //get user input from console
            search_author(book_array, numBooks, author); //call function to search the data base by the author
        }

        else if(userChoice == 2) //if the user chose to search by ISBN number
        {
            printf("\nPlease enter an ISBN number to search the books by: ");
            fgets(ISBNnum, 50, stdin);
            search_ISBN(book_array, numBooks, ISBNnum);
        }
    }

    return 0;
}

/*-----------------------------------------------------------
* Function: parse_file
* Description: Sift through a file of books to populate
* an empty book array
* Inputs:
* (char*) filename: file to open and parse
* (book*) book_array: array of book structures to
* populate
* Outputs:
* (int) parse status, 1 if successful, 0 otherwise.
*---------------------------------------------------------*/
int parse_file(char filename[], book book_array[]) {
 FILE* infile = fopen(filename, "r"); // Attempt to open file
 if (infile == NULL) // Return 0 (failure) if file could not open
 return 0;

 char buffer[512]; // Create temporary string buffer variable
 int i = 0; // Indexer for book array

 while (fgets(buffer, 512, infile)) { // Loop collecting each line from the file
 char * ptr = strtok(buffer,","); // Parse string by commas and newline
 if(strcmp(ptr,"N/A")) // Validate string
 strcpy(book_array[i].title,ptr);// First parse is title

 ptr = strtok(NULL,",\n");
 // Todo: Validate string
 // Todo: Second parse is author
 // Todo: Handle null value

 ptr = strtok(NULL,",\n");
 // Todo: Validate string
 // Todo: Third parse is ISBN
 // Todo: Handle null value

 ptr = strtok(NULL,",\n");
 // Todo: Validate string
 // Todo: Fourth parse is page count
 // Todo: Handle null value

 ptr = strtok(NULL,",\n");
 // Todo: Validate string
 // Todo: Fifth parse is year published
 // Todo: Handle null value
 i++;
 }

 return 1;
}

/*-----------------------------------------------------------
* Function: print_book
* Description: Sift through a file of books to populate
* an empty book array
* Inputs:
* (char*) filename: file to open and parse
* (book*) book_array: array of book structures to
* populate
* Outputs:
* (int) parse status, 1 if successful, 0 otherwise.
*---------------------------------------------------------*/

void print_book(book)
{


}

/*-----------------------------------------------------------
* Function: search_title
* Description: Sift through a file of books to populate
* an empty book array
* Inputs:
* (char*) filename: file to open and parse
* (book*) book_array: array of book structures to
* populate
* Outputs:
* (int) parse status, 1 if successful, 0 otherwise.
*---------------------------------------------------------*/

void search_title(char, int, char)
{

}

/*-----------------------------------------------------------
* Function: search_author
* Description: Sift through a file of books to populate
* an empty book array
* Inputs:
* (char*) filename: file to open and parse
* (book*) book_array: array of book structures to
* populate
* Outputs:
* (int) parse status, 1 if successful, 0 otherwise.
*---------------------------------------------------------*/

void search_author(char, int, char)
{

}

/*-----------------------------------------------------------
* Function: search_ISBN
* Description: Sift through a file of books to populate
* an empty book array
* Inputs:
* (char*) filename: file to open and parse
* (book*) book_array: array of book structures to
* populate
* Outputs:
* (int) parse status, 1 if successful, 0 otherwise.
*---------------------------------------------------------*/

void search_ISBN(char, int, char)
{

}
