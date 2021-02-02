#include<stdio.h>
#include<stdlib.h>
#include<string.h> //include the string library to tokenize arrays

//defining the book structure
typedef struct{
    char title[255];
    char author_name[50];
    char ISBN[10];
    int pages;
    int year_published;
}book;

//custom function prototypes
int parse_file(char*, book*);
void print_book(book);
void search_title(book*, int, char*);
void search_author(book*, int, char*);
void search_ISBN(book*, int, char*);

int main()
{
    //array of book-structures
    book book_array[360];
    int acceptable_input = 1; //flag for user input
    char userChoice[100]; //variable to hold user choice for searching through book database
    int userNum;
    int applicationOn = 1; //flag to let program loop through book database endlessly inside of while loop
    int numBooks; //int variable to store the number of books from the file

    char filename[] = "BookList.csv";

    char Title[225]; //string for user input titles
    char author[50]; //string for user input authors
    char ISBNnum[50]; //string for user input of ISBN numbers

    //call parse_file() and send the filename and array of book structures
    parse_file(filename, book_array);
    numBooks = 360; //set the number of books
    while(applicationOn == 1)
    {

    //prompt user to search for books by title, author, or ISBN
    printf("What would you like to search by? Enter [0] for Title, [1] for Author, or [2] for ISBN: ");
     do //do-while loop to check for user input
        {
            acceptable_input = 1; //reset the flag to acceptable input at every loop. Conditionals will change the flag if necessary
            fgets(userChoice, 100, stdin); //read in the user choice and check if it is acceptable
        for(int i = 0; userChoice[i] != NULL; i++) //a for loop to go through the user string to check for characters in the input
            {
                if((isalpha(userChoice[i])) || (ispunct(userChoice[i]))) //if the user input contains a character or punctuation mark, the flag is set to 0 for unacceptable input
                    acceptable_input = 0;
            }
            userNum = atoi(userChoice); //convert the input to an integer
            if(((userNum == 0) || (userNum == 1) || (userNum == 2)) && (acceptable_input == 1)) //if the input is a digit and is 0, 1, or 2
            {
                acceptable_input = 1;
            }
            if((userNum < 0) || (userNum > 2)) //if the number is less than 0 or greater than 2
            {
                acceptable_input = 0; //non acceptable input
            }
            if(acceptable_input == 0)
                {
                    printf("\nPlease enter a 0, 1, or 2 to search the book database: "); //prompt the user to enter a new value
                    getchar();
                }
        }while(acceptable_input == 0); //loop while the input is unacceptable

        if(userNum == 0) //if the user chose to search by title
        {
            printf("\nPlease enter a title or key words to search the book titles by: ");
               acceptable_input = 1;
               fgets(Title, 255, stdin); //read in all of user input
               Title[strlen(Title) - 1] = '\0'; //fgets reads in the newline character, which strstr() can't understand, so the newline character has to be rest to the null character
             search_title(book_array, numBooks, Title); //call function to search the book database using the user title
        }

        else if(userNum == 1) //if the user chose to search by author
        {
            printf("\nPlease enter an author name or letters of a name to search the books by: "); //prompt user to enter an author's name or letters to a name
            fgets(author, 50, stdin); //read in all of user input
            author[strlen(author) - 1] = '\0';
            search_author(book_array, numBooks, author); //call function to search the data base by the author
        }

        else if(userNum == 2) //if the user chose to search by ISBN number
        {
            printf("\nPlease enter an ISBN number to search the books by: ");
            fgets(ISBNnum, 10, stdin);
            ISBNnum[strlen(ISBNnum) - 1] = '\0';
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
 else
    strcpy(book_array[i].title,ptr);

 ptr = strtok(NULL,",\n");
 //Validate string
 if(strcmp(ptr,"N/A"))
 {
 //Second parse is author
     strcpy(book_array[i].author_name,ptr);
//Handle null value
 }
  else
    strcpy(book_array[i].author_name,ptr);

 ptr = strtok(NULL,",\n");
 //Validate string
 if(strcmp(ptr, "N/A"))
 {
 // Third parse is ISBN
    strcpy(book_array[i].ISBN, ptr);
 }
 //Handle null value
  else
    strcpy(book_array[i].ISBN,ptr);

 ptr = strtok(NULL,",\n");
 //Validate string
 if(strcmp(ptr, "N/A"))
 {
 //Fourth parse is page count
    book_array[i].pages = atoi(ptr);
 }
 //Handle null value
 else
    book_array[i].pages = atoi(ptr);

 ptr = strtok(NULL,",\n");
 //Validate string
 if(strcmp(ptr, "N/A"))
 {
 //Fifth parse is year published
    book_array[i].year_published = atoi(ptr);
 }
 //Handle null value
 else
    book_array[i].year_published = atoi(ptr);

 i++;
 }

 return 1;
}

/*-----------------------------------------------------------
* Function: print_book
* Description: Prints the contents of the structured variable
*  book that is passed to it.
* Inputs:
* (book*) book_array: array of book structures to print
* Outputs:
*  Type void, so no outputs
*---------------------------------------------------------*/

void print_book(book Book)
{
    //print the contents
    printf("\nTitle: %s\nAuthor: %s\nISBN: %s\nPage count: %d\nYear Published: %d\n", Book.title, Book.author_name, Book.ISBN, Book.pages, Book.year_published);

}

/*-----------------------------------------------------------
* Function: search_title
* Description: Sift through n array of books to find
* a book title that matches the user input string
* Inputs:
* (book*) book_array: array of book structures to read through
* (int) numBooks: number of books in the array
* Outputs:
* Type void, so no outputs
*---------------------------------------------------------*/

void search_title(book book_array[], int numBooks, char userTitle[])
{
    int i;
    char *p;
    for(i = 0; i < numBooks; i++) //loop to go through book array
    {
       p = strstr(book_array[i].title, userTitle);
       //printf("%s, %s\n", book_array[i].title, userTitle);
       if(p)
        {
            print_book(book_array[i]); //if the user title matches a title in the book array call the print_book function
        }
    }
}

/*-----------------------------------------------------------
* Function: search_author
* Description: Sift through an array of books to find the author
*  name that matches the string the user passed.
* Inputs:
* (book*) book_array: array of book structures to read through
* (int) numBooks: number of books in the array
* Outputs:
* Type void, so no outputs
*---------------------------------------------------------*/

void search_author(book book_array[], int Books, char author[])
{
    int i;
    char *p;
  for(i = 0; i < Books; i++) //loop to go through book array
    {
       p = strstr(book_array[i].author_name, author);
       //printf("%s, %s\n", book_array[i].title, userTitle);
       if(p)
        {
            print_book(book_array[i]); //if the user title matches a title in the book array call the print_book function
        }
    }
}

/*-----------------------------------------------------------
* Function: search_ISBN
* Description: Sift through an array of books to find an ISBN
*  number that matches the ISBN string the user passed.
* Inputs:
* (book*) book_array: array of book structures to read through
* (int) numBooks: number of books in the array
* Outputs:
* Type void, so no outputs
*---------------------------------------------------------*/

void search_ISBN(book book_array[], int numBook, char ISBN[])
{
    int i;
    char *p;
  for(i = 0; i < numBook; i++) //loop to go through book array
    {
       p = strstr(book_array[i].ISBN, ISBN);
       //printf("%s, %s\n", book_array[i].title, userTitle);
       if(p)
        {
            print_book(book_array[i]); //if the user title matches a title in the book array, call the print_book function
        }
    }
}
