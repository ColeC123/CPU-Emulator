#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "strfunctions.h"
#include "string.h"

// Arbitrary file size limit, could be much larger
#define max_file_string_length 5000

#define MAX_FILE_LINE_READ 70

int main(int argc, char** argv) {
    //<=1 since the name of the executable is included in the command line arguments
    if (argc <= 1) {
        return 0;
    }

    char *file_string = (char *)malloc(max_file_string_length * sizeof(char));
    int file_string_logical_size = 0;

    FILE *fptr;
    fptr = fopen(PATH, "r");
    if (fptr == NULL) {
        printf("\n\nFile Opening error\n\n");
    }

    char mystring[MAX_FILE_LINE_READ];
    bool semicolon_present = false;

    while (fgets(mystring, MAX_FILE_LINE_READ, fptr)) {
        int count = 0;
        while (mystring[count] != '\0' && file_string_logical_size < max_file_string_length) {
            // The logic for semicolon comments
            if (mystring[count] == ';') {
                semicolon_present = true;
            } else if (mystring[count] == '\n') {
                semicolon_present = false;
            }

            // Spaces are removed while moving through the file because it is more efficient than copying the entire file and then removing them
            // semicolon bool is needed because fgets will move through the entire file no matter how small MAX_FILE_LINE_READ is (within reason)
            // fgets sort of reads through the file in chunks
            if (mystring[count] != ' ' && semicolon_present == false) {
                file_string[file_string_logical_size] = mystring[count];
                file_string_logical_size++;
            }

            count++;
        }
    }
    fclose(fptr);

    // null terminator used to mark the ending point of the file
    file_string[file_string_logical_size] = '\0';

    // The rest of this function loads the textfile into memory
    int curr_file_search_count = 0;
    int past_file_search_count = 0;
    int ram_position_count = 0;

    //<= is in order to make sure that the null terminating character of the file is included in the search
    /*
    while (curr_file_search_count <= file_string_logical_size) {
        if (file_string[curr_file_search_count] == '\n' || file_string[curr_file_search_count] == '\0') {
            // fancy function pointers to make code cleaner and faciliate addition of more file parsing options
            Emargs.RAM[ram_position_count] = (*interpret_file_as)(file_string, past_file_search_count, curr_file_search_count - past_file_search_count);
            // add one to account for the fact that curr_file_search_count will be increased by one right after this
            past_file_search_count = curr_file_search_count + 1;
            ram_position_count++;
        }
        curr_file_search_count++;
    }
    */
    free(file_string);
    return 0;
}