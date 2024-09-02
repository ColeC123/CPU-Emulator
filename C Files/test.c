#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "math.h"

// 23 bits for address value allows for 2^23 memory slots, I'm only going for 4096 (2^12) right now
static const int RAM_SIZE = 4096;
int* RAM;

// Arbitrary file size limit, could be much larger
static const int max_file_string_length = 4000;

// See Architecture Notes for more details about instruction set

static int rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r1, r2, r3, r4, r5, r6, r7, r8;

int BinaryStringToInt(char* string, int start, int length);

void ArgumentInputs(void);

int strfind(char* searchfor, char* in, int searchfor_size, int in_size);

int HexStringToInt(char* string, int start, int length);

int IntMin(int arg1, int arg2);

int main(int argc, char** argv) {
    RAM = (int*)malloc(RAM_SIZE * sizeof(int));

    ArgumentInputs();

    for (int i = 0; i < 14; i++) {
        switch (RAM[i] >> 27) {
            case 0b00000:
                printf("HLT Instruction");
                break;

            case 0b00001:
                printf("MOV Instruction");
                break;

            case 0b00010:
                printf("ADD Instruction");
                break;

            case 0b00011:
                printf("CMP Instruction");
                break;

            case 0b00100:
                printf("JMP Instruction");
                break;

            case 0b00101:
                printf("SUB Instruction");
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
    }

    free(RAM);

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

    char readas_type[16]; //size is 16 to allow for wiggle room later on when naming more readas types
    int readas_type_size = 0;
    for (int i = readas_index; args[i] != '"' && (i - readas_index) < 16 && i < arg_size; i++) {
        readas_type[i - readas_index] = args[i];
        readas_type_size++;
    }
    readas_type[readas_type_size] = '\0';

    int (*interpret_file_as) (char*, int, int);

    if (strncmp(readas_type, "binary", IntMin(readas_type_size, 6)) == 0) {
        interpret_file_as = BinaryStringToInt;
        printf("\nbinary conversion\n");
    } else if (strncmp(readas_type, "hexadecimal", IntMin(readas_type_size, 11)) == 0) {
        interpret_file_as = HexStringToInt;
        printf("\nhexadecimal conversion\n");
    } else {
        interpret_file_as = BinaryStringToInt;
        printf("\ndefault conversion\n");
    }
    // ----- Options right now are binary and hexadecimal ------

    FILE* fptr;
    fptr = fopen(PATH, "r");
    char mystring[60];
    while (fgets(mystring, 60, fptr)) {
        int count = 0;
        while (mystring[count] != '\0' && file_string_logical_size < max_file_string_length) {
            if (mystring[count] != ' ') {
                file_string[file_string_logical_size] = mystring[count];
                file_string_logical_size++;
            }
            count++;
        }
    }
    //null terminator used to mark the ending point of the file
    file_string[file_string_logical_size] = '\0';

    int curr_file_search_count = 0;
    int past_file_search_count = 0;
    int ram_position_count = 0;
    //<= is in order to make sure that the null terminating character of the file is included in the search
    while (curr_file_search_count <= file_string_logical_size) {
        if (file_string[curr_file_search_count] == '\n' || file_string[curr_file_search_count] == '\0') {
            //fancy function pointers to make code cleaner and faciliate addition of more file parsing options
            RAM[ram_position_count] = (*interpret_file_as)(file_string, past_file_search_count, curr_file_search_count - past_file_search_count);
            //add one to account for the fact that curr_file_search_count will be increased by one right after this
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