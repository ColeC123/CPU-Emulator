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
        printf("\n\x1b[31mError\x1b[0m: Not Enough arguments\n");
        return 0;
    }

    bool successfull_command_parse = false;
    for (int i = 0; i < 1; i++) {
        // the command convfile will be used to convert a specified file into another type of file
        if (strncmp(argv[1], "convfile", 8) == 0) {
            if (convFileCondition(argv, argc) == 1) {
                printf("\n\x1b[31mError\x1b[0m: Command Execution Failed\n");
                return 1;
            } else {
                successfull_command_parse = true;
                // break out of the loop since I only have this setup right now to handle one command at a time
                break;
            }
        }
    }

    if (successfull_command_parse == true) {
        printf("\n\x1b[32mSuccess\x1b[0m: Successfully parsed and executed command\n");
    } else {
        printf("\n\x1b[31mError\x1b[0m: Failed to parse command\n");
    }

    return 0;
}

// Proper order for convfile command: cli convfile FilePath WhatToConvertFileTo OutputFile
/*
Note: for file paths if a space is contained in the file path, then it must be surrounded by quotes
the parser that passes these commands into this process will automatically remove the quotes
*/
int convFileCondition(char** searchthrough, int argsize) {
    if (argsize < 5) {
        printf("\n\x1b[31mError\x1b[0m: convfile command requires the following format for command line arguments\n");
        printf("cli convfile ReadFilePath WhatToConvertFileTo Output/WriteFilePath\n");
        printf("WhatToConvertFileTo options:\n");
        printf("bt2b -- binary text (.txt) to binary file (.bin)\n");
        printf("b2bt -- binary file (.bin) to binary text (.txt)\n");
        return 1;
    }
    char file_type[7];
    int file_type_count = 0;
    bool period_found = false;

    // This gets the last index of a period in the file path.
    // This is done to ensure that any periods earlier on in the path are not misinterpreted as the file type
    int period_last_index = -1;
    for (int i = 0; searchthrough[2][i] != '\0'; i++) {
        if (searchthrough[2][i] == '.') {
            period_last_index = i;
        }
    }

    if (period_last_index == -1) {
        printf("\n\x1b[31mError\x1b[0m: File extension not found. Please include a file extension when reading files.\n");
        return 1;
    }

    // given the last index of a period, add 1 to get the start of the file type and record the file type to file_type string
    // Even though file_type_count can store 7 character, it is less than 6 to account for the extra null terminator character required
    for (int i = period_last_index + 1; searchthrough[2][i] != '\0' && file_type_count < 6; i++) {
        file_type[file_type_count] = searchthrough[2][i];
        file_type_count++;
    }
    file_type[file_type_count] = '\0';

    if (strncmp(file_type, "txt", 3) != 0 && strncmp(file_type, "bin", 3) != 0) {
        printf("\n\x1b[31mError\x1b[0m: The file extensions .%s is not supported\n", file_type);
        return 1;
    }

    // This is used for debugging
    // printf("\nFile Type: %s\n", file_type);

    // bt2b stands for binary text in a .txt file (as in 001000) to binary file (.bin)
    if (strncmp(searchthrough[3], "bt2b", 4) == 0 && strncmp(file_type, "txt", 3) == 0) {
        FILE* given_file = fopen(searchthrough[2], "rt");

        if (given_file == NULL) {
            printf("\n\x1b[31mError\x1b[0m: Failed to open file\n");
            // This will indicate in the main function to exit since the file couldn't be opened
            return 1;
        }

        FILE* output_file = fopen(searchthrough[4], "wb");

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

    // b2bt stands for binary file (.bin) to binary textfile (.txt)
    if (strncmp(searchthrough[3], "b2bt", 4) == 0 && strncmp(file_type, "bin", 3) == 0) {
        FILE* given_file = fopen(searchthrough[2], "rb");

        if (given_file == NULL) {
            printf("\n\x1b[31mError\x1b[0m: Failed to open file\n");
            // lets the main function to exit since it failed to open the given file
            return 1;
        }

        FILE* output_file = fopen(searchthrough[4], "wt");

        // Get the size of the file to know how much memory to allocate
        int file_int_size = 0;
        fseek(given_file, 0L, SEEK_END);

        file_int_size = (int)ftell(given_file);
        rewind(given_file);

        int* file_int = (int*)malloc(file_int_size * sizeof(int));
        int file_int_logical_size = 0;

        // loop through the file until the end of the file and read the contents of the file as an int to a buffer
        while (!feof(given_file)) {
            fread(file_int + file_int_logical_size, sizeof(int), 1, given_file);
            file_int_logical_size++;
        }

        // Write the binary of the file as a character string of 1s and 0s
        char* binary_string = NULL;
        for (int i = 0; i < file_int_logical_size; i++) {
            binary_string = IntToBinaryString(binary_string, file_int[i], 32);
            fprintf(output_file, binary_string);
            fprintf(output_file, "\n");
        }

        free(file_int);
        fclose(given_file);
        fclose(output_file);

        return 0;
    }

    return 0;
}