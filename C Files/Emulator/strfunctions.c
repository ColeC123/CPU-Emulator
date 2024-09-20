#include "stdio.h"
#include "stdlib.h"
#include "strfunctions.h"

int BinaryStringToInt(char *string, int start, int length) {
    unsigned int retval = 0;
    for (int i = start; i < start + length; i++) {
        retval += (unsigned int)((unsigned int)string[i] - (unsigned int)48) << (length - (i - start) - 1);
    }
    return (int)retval;
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