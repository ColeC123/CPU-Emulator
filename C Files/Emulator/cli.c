#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "strfunctions.h"
#include "string.h"

// short for convert file condition
int convFileCondition(char** searchthrough, int argsize);

int main(int argc, char** argv) {
    //<=1 since the name of the executable is included in the command line arguments
    if (argc <= 1) {
        printf("\nNot Enough arguments\n");
        return 0;
    }

    for (int i = 0; i < 1; i++) {
        // the command convfile will be used to convert a specified file into another type of file
        if (strncmp(argv[1], "convfile", 8) == 0) {
            if (convFileCondition(argv, argc) == 1) {
                printf("\nFile Creation Failed\n");
                return 1;
            }
        }
    }

    /*
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
    */

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
    return 0;
}

// Proper order for convfile command: cli convfile FilePath WhatToConvertFileTo OutputFile
/*
Note: for file paths if a space is contained in the file path, then it must be surrounded by quotes
the parser that passes these commands into this process will automatically remove the quotes
*/
int convFileCondition(char** searchthrough, int argsize) {
    char file_type[7];
    int file_type_count = 0;
    bool period_found = false;

    // This code is responsible for figuring out the file type of the given file
    for (int i = 0; searchthrough[2][i] != '\0'; i++) {
        if (period_found == true && file_type_count < 7) {
            file_type[file_type_count] = searchthrough[2][i];
            file_type_count++;
        }

        if (searchthrough[2][i] == '.') {
            period_found = true;
        }
    }
    file_type[file_type_count] = '\0';

    printf("\nFile Type: %s\n", file_type);

    // bt2b stands for binary text in a .txt file (as in 001000) to binary file (.bin)
    if (strncmp(searchthrough[3], "bt2b", 4) == 0 && strncmp(file_type, "txt", 3) == 0) {
        FILE* given_file = fopen(searchthrough[2], "rt");
        FILE* output_file = fopen(searchthrough[4], "wb");

        if (given_file == NULL) {
            // This will indicate in the main function to exit since the file couldn't be opened
            return 1;
        }
        printf("\nSuccessfully Parsed command\n");

        // This is used to get the size of te file to know how much to allocate with malloc
        fseek(given_file, 0L, SEEK_END);
        int file_string_size = ftell(given_file);
        char* file_string = (char*)malloc((file_string_size + 1) * sizeof(char));

        int file_string_logical_size = 0;
        rewind(given_file);

        bool semicolon_present = false;

        while (!feof(given_file)) {
            char temp_char;
            fread(&temp_char, sizeof(char), 1, given_file);

            if (temp_char == ';') {
                semicolon_present = true;
            } else if (temp_char == '\n' || temp_char == '\0') {
                semicolon_present = false;
            }

            if (semicolon_present == false && temp_char != ' ') {
                file_string[file_string_logical_size] = temp_char;
                file_string_logical_size++;
            }
        }
        file_string[file_string_logical_size] = '\0';

        /*
        printf("\nTranslated Text File\n%s\n", file_string);

        printf("\nFile String logical size: %d\n", file_string_logical_size);
        printf("File String actual size: %d\n\n", file_string_size);

        printf("\nBinary to Int Conversion\n");
        */

        int last_line_start_index = 0;

        // file_string_logical_size + 1 accounts for the fact that there is a null terminator at that index
        for (int i = 0; i < file_string_logical_size + 1; i++) {
            if (file_string[i] == '\n' || file_string[i] == '\0') {
                // this accounts for the case where the only character on a line is a newline character in teh text file
                if (i - last_line_start_index > 0) {
                    int temp_int = BinaryStringToInt(file_string, last_line_start_index, i - last_line_start_index);
                    fwrite(&temp_int, sizeof(int), 1, output_file);

                    /*
                    Used for print debugging:
                    char* bin_str = NULL;
                    bin_str = IntToBinaryString(bin_str, temp_int, 32);
                    printf("index: %d | Number: %u | Binary rep: %s\n", i, (unsigned int)temp_int, bin_str);
                    free(bin_str);
                    */
                } else {
                    /*
                    For the case where the line contains only a newline character (i - last_line_start_index = 0)
                    that spot in the file will be initialized to 0
                    */

                    int temp_int = 0;
                    fwrite(&temp_int, sizeof(int), 1, output_file);

                    /*
                    Used for print debugging:
                    char* bin_str = NULL;
                    bin_str = IntToBinaryString(bin_str, temp_int, 32);
                    printf("index: %d | Number: %u | Binary rep: %s\n", i, (unsigned int)temp_int, bin_str);
                    free(bin_str);
                    */
                }

                // account for fact that next line start will occur after \n, or at i + 1 index
                last_line_start_index = i + 1;
            }
        }

        free(file_string);

        fclose(given_file);
        fclose(output_file);

        return 0;
    }

    // b2bt stands for binary file (.bin) to binary textfile
    if (strncmp(searchthrough[3], "b2bt", 4) == 0 && strncmp(file_type, "bin", 3) == 0) {
        // Do stuff to convert binary file to textfile filled with binary strings
    }
    return 0;
}