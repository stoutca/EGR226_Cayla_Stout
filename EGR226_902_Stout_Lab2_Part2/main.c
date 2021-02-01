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
void parse_file(char, char);
void print_book(book);
void search_title(char, int, char);
void search_author(char, int, char);
void search_ISBN(char, int, char);

int main()
{
    //array of book-structures
    book book_array[360];

    //call parse_file() and send the filename and array of book structures
    parse_file(char filename, book_array);

    //prompt user to search for books by title, author, or ISBN
    printf("What would you like to search by? Enter [0] for Title, [1] for Author, or [2] for ISBN: ");

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
