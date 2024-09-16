#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

// Arbitrary file size limit, could be much larger
#define max_file_string_length 5000

#define MAX_FILE_LINE_READ 70

// Using a struct for the input into ArgumentInputs makes it much easier to add functionality later on
typedef struct ArgList {
    int *RAM;
    bool *emulation_active;
} ArgList;

int BinaryStringToInt(char *string, int start, int length);

int ArgumentInputs(ArgList Emargs);

int strfind(char *searchfor, char *in, int searchfor_size, int in_size);

int HexStringToInt(char *string, int start, int length);

int IntMin(int arg1, int arg2);

char *IntToBinaryString(char *str, int input, int padding);

int strInsertChar(char *str, int index, char value, int size);

int charfind(char searchfor, char *in, int in_size);

int BinaryStringToInt(char *string, int start, int length) {
    unsigned int retval = 0;
    for (int i = start; i < start + length; i++) {
        retval += (unsigned int)((unsigned int)string[i] - (unsigned int)48) << (length - (i - start) - 1);
    }
    return (int)retval;
}

int ArgumentInputs(ArgList Emargs) {
    char *file_string = (char *)malloc(max_file_string_length * sizeof(char));
    int file_string_logical_size = 0;

    // Right now max argument size is 140 characters, maybe change this later
    char args[140];
    printf("Args> ");
    fgets(args, 140, stdin);
    int arg_size = strlen(args);

    char *look_for_exit = "exit";
    int exit_index = strfind(look_for_exit, args, 4, arg_size);
    if (exit_index != -1) {
        // This lets the main program know to exit the loop and perform the proper cleanup
        *(Emargs.emulation_active) = false;
        free(file_string);
        return 0;
    }

    // ----- looks for file to be scanned and loaded into memory -----
    char *load_file = "load:\"";
    int load_file_str_size = strlen(load_file);
    int file_path_index = strfind(load_file, args, load_file_str_size, arg_size) + load_file_str_size;

    char PATH[257];
    PATH[256] = '\0';  // Initialize last byte to be null terminator just in case
    int PATH_size = 0;

    // This is the logic for getting the PATH or directory to the textfile
    for (int i = file_path_index; args[i] != '"' && (i - file_path_index) < 256 && i < arg_size; i++) {
        PATH[i - file_path_index] = args[i];
        PATH_size++;
    }
    PATH[PATH_size] = '\0';
    // ----- Note: the text file path must be in quotes after load: -----

    // ----- looks for readas to determine how to interpret contents of text file -----
    char *readas = "readas:\"";
    int readas_size = strlen(readas);
    int readas_index = strfind(readas, args, readas_size, arg_size) + readas_size;

    //Default interpretation of file will be binary string
    int (*interpret_file_as)(char *, int, int);
    interpret_file_as = BinaryStringToInt;

    // Continue if strfind didn't fail and returned something greater than -1 for the readastype
    if (readas_index != readas_size - 1) {
        char readas_type[16];  // size is 16 to allow for wiggle room later on when naming more readas types
        int readas_type_size = 0;

        // This is the logic for getting the readas_type string from the arguments
        for (int i = readas_index; args[i] != '"' && (i - readas_index) < 16 && i < arg_size; i++) {
            readas_type[i - readas_index] = args[i];
            readas_type_size++;
        }
        readas_type[readas_type_size] = '\0';

        if (strncmp(readas_type, "binary", IntMin(readas_type_size, 6)) == 0) {
            interpret_file_as = BinaryStringToInt;
        } else if (strncmp(readas_type, "hexadecimal", IntMin(readas_type_size, 11)) == 0) {
            interpret_file_as = HexStringToInt;
        }
    }
    // ----- Options right now are binary and hexadecimal ------

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
    free(file_string);

    return 0;
}

int strfind(char *searchfor, char *in, int searchfor_size, int in_size) {
    int comparison_counter = 0;

    for (int i = 0; i < in_size - searchfor_size; i++) {
        for (int j = 0; j < searchfor_size; j++) {
            if (in[i + j] == searchfor[j]) {
                comparison_counter++;
            }
        }

        if (comparison_counter == searchfor_size) {
            return i;
        }

        comparison_counter = 0;
    }

    // if no index is found, -1 is returned
    return -1;
}

int charfind(char searchfor, char *in, int in_size) {
    for (int i = 0; i < in_size; i++) {
        if (in[i] == searchfor) {
            return i;
        }
    }

    // If no index is found, it will return -1
    return -1;
}

// For letter values, this function only excepts capitals (A-F)
int HexStringToInt(char *string, int start, int length) {
    int retval = 0;
    for (int i = start; i < start + length; i++) {
        if ((int)string[i] <= (int)'9') {
            retval += (int)((int)string[i] - 48) << ((length - (i - start) - 1) << 2);
        } else {
            retval += (int)((int)string[i] - 55) << ((length - (i - start) - 1) << 2);
        }
    }
    return retval;
}

int IntMin(int arg1, int arg2) {
    if (arg1 > arg2) {
        return arg2;
    } else {
        return arg1;
    }
}

// Unitialized strings should be set to NULL when using this function
// Unkown string sizes call for heap allocated memroy, don't forget to free it
// Use function like: char* str = IntToBinaryString(str, 19);
char *IntToBinaryString(char *str, int input, int padding) {
    // input is converted to unsigned int since my logic relies on the fact that bit shifting to the left
    // fills in the new binary spaces with 0s instead of 1s as happens for negative signed integers
    unsigned int conversion = (unsigned int)input;

    str = (char *)realloc(str, (padding + 1) * sizeof(char));

    if (str != NULL) {
        for (int i = padding - 1; i > 0; i--) {
            str[i] = (char)(((conversion % 2) + 48));
            conversion = conversion >> 1;
        }
        str[0] = (char)(conversion + 48);
        str[padding] = '\0';

        return str;
    } else {
        exit(1);
    }
}

// Inserts char at specified index, pushing everything at and after that index back by one
// Make sure that the string you pass in enough space to accommadate everything being pushed left by one
int strInsertChar(char *str, int index, char value, int size) {
    char temp1 = value;
    for (int i = index; i < size + 1; i++) {
        char temp2 = str[i];
        str[i] = temp1;
        temp1 = temp2;
    }
}