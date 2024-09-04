#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
// 23 bits for address value allows for 2^23 memory slots, I'm only going for 4096 (2^12) right now
static const int RAM_SIZE = 4096;
int* RAM;

// Arbitrary file size limit, could be much larger
static const int max_file_string_length = 4000;

// See Architecture Notes for more details about instruction set

// For now all of the registers will just be initialized to 0 at the start of the program
static int registers[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#define MAX_FILE_LINE_READ 70

enum register_names {
    rax = 0b0000,
    rbx = 0b0001,
    rcx = 0b0010,
    rdx = 0b0011,
    rsi = 0b0100,
    rdi = 0b0101,
    rbp = 0b0110,
    rsp = 0b0111,
    r1 = 0b1000,
    r2 = 0b1001,
    r3 = 0b1010,
    r4 = 0b1011,
    r5 = 0b1100,
    r6 = 0b1101,
    r7 = 0b1110,
    r8 = 0b1111,
};

int BinaryStringToInt(char* string, int start, int length);

void ArgumentInputs(void);

int strfind(char* searchfor, char* in, int searchfor_size, int in_size);

int HexStringToInt(char* string, int start, int length);

int IntMin(int arg1, int arg2);

char* IntToBinaryString(char* str, int input);

int main(void) {
    RAM = (int*)malloc(RAM_SIZE * sizeof(int));

    char* binarynum = NULL;
    char* binarynum2 = NULL;

    ArgumentInputs();

    bool running = true;

    // unsigned int makes for more consistent bit shifting behavior
    unsigned int ram_value = 0;
    while (running) {
        ram_value = RAM[registers[rcx]];

        binarynum = IntToBinaryString(binarynum, ram_value >> 27);
        printf("opcode: %s\n", binarynum);

        switch (ram_value >> 27) {
            case 0b00000:
                printf("HLT Instruction");
                running = false;
                break;

            case 0b00001:
                printf("MOV Instruction");
                registers[(ram_value << 5) >> 28] = RAM[(ram_value << 9) >> 9];
                break;

            case 0b00010:
                printf("ADD Instruction");
                registers[(ram_value << 5) >> 28] += registers[(ram_value << 9) >> 28];
                break;

            case 0b00011:
                printf("CMP Instruction");
                break;

            case 0b00100:
                printf("JMP Instruction");
                // Gets rid of top 5 bits (opcode), and then shifts to the left 9 bits
                // in order to get the correct address size
                //-1 accounts for the fact that rcx automatically increments by one
                registers[rcx] = ((RAM[registers[rcx]] << 5) >> 9) - 1;
                break;

            case 0b00101:
                printf("SUB Instruction");
                registers[(ram_value << 5) >> 28] -= registers[(ram_value << 9) >> 28];
                break;

            case 0b00110:
                printf("LEA Instruction");
                break;

            case 0b00111:
                printf("STORE Instruction");
                break;

            case 0b01000:
                printf("BITSHFTR Instruction");
                break;

            case 0b01001:
                printf("BITSHFTL Instruction");
                break;

            case 0b01010:
                printf("AND Instruction");
                break;

            case 0b01011:
                printf("XOR Instruction");
                break;

            case 0b01100:
                printf("OR Instruction");
                break;

            case 0b01101:
                printf("NOT Instruction");
                break;

            default:
                printf("Opcode Identification Failure");
                break;
        }

        printf("\n\n");
        registers[rcx]++;
    }

    free(RAM);
    free(binarynum);
    free(binarynum2);

    printf("\n\nrax: %d  |  rbx: %d\n", registers[rax], registers[rbx]);
    printf("rcx: %d  |  rdx: %d\n", registers[rcx], registers[rdx]);
    printf("rsi: %d  |  rdi: %d\n", registers[rsi], registers[rdi]);
    printf("rbp: %d  |  rsp: %d\n", registers[rbp], registers[rsp]);
    printf("r1:  %d  |  r2:  %d\n", registers[r1], registers[r2]);
    printf("r3:  %d  |  r4:  %d\n", registers[r3], registers[r4]);
    printf("r5:  %d  |  r6:  %d\n", registers[r5], registers[r6]);
    printf("r7:  %d  |  r8:  %d\n", registers[r7], registers[r8]);

    return 0;
}

int BinaryStringToInt(char* string, int start, int length) {
    int retval = 0;
    for (int i = start; i < start + length; i++) {
        retval += ((int)string[i] - 48) << (length - (i - start) - 1);
    }
    return retval;
}

void ArgumentInputs(void) {
    char* file_string = (char*)malloc(max_file_string_length * sizeof(char));
    int file_string_logical_size = 0;

    // Right now max argument size is 140 characters, maybe change this later
    char args[140];
    printf("Args> ");
    fgets(args, 140, stdin);
    int arg_size = strlen(args);

    // ----- looks for file to be scanned and loaded into memory -----
    char* load_file = "load:\"";
    int load_file_str_size = strlen(load_file);
    int file_path_index = strfind(load_file, args, load_file_str_size, arg_size) + load_file_str_size;

    char PATH[257];
    int PATH_size = 0;
    for (int i = file_path_index; args[i] != '"' && (i - file_path_index) < 256 && i < arg_size; i++) {
        PATH[i - file_path_index] = args[i];
        PATH_size++;
    }
    PATH[PATH_size] = '\0';
    // ----- Note: the text file path must be in quotes after load: -----

    // ----- looks for readas to determine how to interpret contents of text file -----
    char* readas = "readas:\"";
    int readas_size = strlen(readas);
    int readas_index = strfind(readas, args, readas_size, arg_size) + readas_size;

    char readas_type[16];  // size is 16 to allow for wiggle room later on when naming more readas types
    int readas_type_size = 0;
    for (int i = readas_index; args[i] != '"' && (i - readas_index) < 16 && i < arg_size; i++) {
        readas_type[i - readas_index] = args[i];
        readas_type_size++;
    }
    readas_type[readas_type_size] = '\0';

    int (*interpret_file_as)(char*, int, int);

    if (strncmp(readas_type, "binary", IntMin(readas_type_size, 6)) == 0) {
        interpret_file_as = BinaryStringToInt;
    } else if (strncmp(readas_type, "hexadecimal", IntMin(readas_type_size, 11)) == 0) {
        interpret_file_as = HexStringToInt;
    } else {
        interpret_file_as = BinaryStringToInt;
    }
    // ----- Options right now are binary and hexadecimal ------

    FILE* fptr;
    fptr = fopen(PATH, "r");

    char mystring[MAX_FILE_LINE_READ];
    bool semicolon_present = false;

    while (fgets(mystring, MAX_FILE_LINE_READ, fptr)) {
        int count = 0;
        while (mystring[count] != '\0' && file_string_logical_size < max_file_string_length) {
            if (mystring[count] == ';') {
                semicolon_present = true;
            } else if (mystring[count] == '\n') {
                semicolon_present = false;
            }

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

    int curr_file_search_count = 0;
    int past_file_search_count = 0;
    int ram_position_count = 0;
    //<= is in order to make sure that the null terminating character of the file is included in the search
    while (curr_file_search_count <= file_string_logical_size) {
        if (file_string[curr_file_search_count] == '\n' || file_string[curr_file_search_count] == '\0') {
            // fancy function pointers to make code cleaner and faciliate addition of more file parsing options
            RAM[ram_position_count] = (*interpret_file_as)(file_string, past_file_search_count, curr_file_search_count - past_file_search_count);
            // add one to account for the fact that curr_file_search_count will be increased by one right after this
            past_file_search_count = curr_file_search_count + 1;
            ram_position_count++;
        }
        curr_file_search_count++;
    }

    free(file_string);
}

int strfind(char* searchfor, char* in, int searchfor_size, int in_size) {
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
}

// For letter values, this function only excepts capitals (A-F)
int HexStringToInt(char* string, int start, int length) {
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
char* IntToBinaryString(char* str, int input) {
    // input is converted to unsigned int since my logic relies on the fact that bit shifting to the left
    // fills in the new binary spaces with 0s instead of 1s as happens for negative signed integers
    unsigned int conversion = input;

    unsigned int temp = conversion;
    int binary_string_size = 0;

    binary_string_size++;
    temp = temp >> 1;

    while (temp > 0) {
        binary_string_size++;
        temp = temp >> 1;
    }

    str = (char*)realloc(str, (binary_string_size + 1) * sizeof(char));

    if (str != NULL) {
        for (int i = binary_string_size - 1; i >= 0; i--) {
            str[i] = (char)(((conversion % 2) + 48));
            conversion = conversion >> 1;
        }
        str[binary_string_size] = '\0';

        return str;
    } else {
        exit(1);
    }
}