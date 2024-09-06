#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
// 23 bits for address value allows for 2^23 memory slots, I'm only going for 4096 (2^12) right now
static const int RAM_SIZE = 4096;
int* RAM;

// Arbitrary file size limit, could be much larger
#define max_file_string_length 5000

// See Architecture Notes for more details about instruction set

// For now all of the registers will just be initialized to 0 at the start of the program
static int registers[17] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static int ZF_Flag = 0; //zero result flag
static int PI_Flag = 0; //positive int result flag
static int NI_Flag = 0; //negative int result flag

// keeps track of current
static int program_counter = 0;

#define MAX_FILE_LINE_READ 70

enum register_names {
    rax = 0b00000,
    rbx = 0b00001,
    rcx = 0b00010,
    rdx = 0b00011,
    rsi = 0b00100,
    rdi = 0b00101,
    rbp = 0b00110,
    rsp = 0b00111,
    r1 = 0b01000,
    r2 = 0b01001,
    r3 = 0b01010,
    r4 = 0b01011,
    r5 = 0b01100,
    r6 = 0b01101,
    r7 = 0b01110,
    r8 = 0b01111,
    rip = 0b10000,
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
    unsigned int instruction = 0;
    while (running) {
        instruction = RAM[program_counter];
        registers[rip]++;

        binarynum = IntToBinaryString(binarynum, instruction >> 27);
        printf("opcode: %s\n", binarynum);

        switch (instruction >> 27) {
            case 0b00000:
                printf("HLT Instruction");
                running = false;
                break;

            case 0b00001:
                printf("MOV Instruction");
                registers[(instruction << 5) >> 27] = RAM[(instruction << 10) >> 10];
                break;

            case 0b00010:
                printf("ADD Instruction");
                registers[(instruction << 5) >> 27] += registers[(instruction << 10) >> 27];
                break;

            case 0b00011:
                printf("CMP Instruction");
                int comparison_result = registers[(instruction << 5) >> 27] - registers[(instruction << 10) >> 27];

                if (comparison_result == 0) {
                    ZF_Flag = 1;
                } else {
                    ZF_Flag = 0;
                }
                
                if (comparison_result > 0) {
                    PI_Flag = 1;
                } else {
                    PI_Flag = 0;
                }
                
                if (comparison_result < 0) {
                    NI_Flag = 1;
                } else {
                    NI_Flag = 0;
                }

                break;

            case 0b00100:
                printf("JMPIF Instruction");
                int jump_condition = (instruction << 5) >> 27;
                switch (jump_condition) {
                    case 0b00000:
                        if (ZF_Flag == 1) {
                            registers[rip] = (instruction << 10) >> 10;
                        }
                        break;
                    
                    case 0b00001:
                        if (ZF_Flag == 0) {
                            registers[rip] = (instruction << 10) >> 10;
                        }
                        break;
                    
                    case 0b00010:
                        if (NI_Flag == 1) {
                            registers[rip] = (instruction << 10) >> 10;
                        }
                        break;

                    case 0b00011:
                        if (PI_Flag == 1) {
                            registers[rip] = (instruction << 10) >> 10;
                        }
                        break;

                    case 0b00100:
                        if (NI_Flag == 1 || ZF_Flag == 1) {
                            registers[rip] = (instruction << 10) >> 10;
                        }
                        break;
                    
                    case 0b00101:
                        if (PI_Flag == 1 || ZF_Flag == 1) {
                            registers[rip] = (instruction << 10) >> 10;
                        }
                        break;
                }
                break;

            case 0b00101:
                printf("SUB Instruction");
                registers[(instruction << 5) >> 27] -= registers[(instruction << 10) >> 27];
                break;

            case 0b00110:
                printf("PUSH/POP");
                break;

            case 0b00111:
                printf("STORE Instruction");
                RAM[(instruction << 10) >> 10] = registers[(instruction << 5) >> 27];
                break;

            case 0b01000:
                printf("LBITSHFTR Instruction");
                // instruction is already an unsigned integer
                // This gives consistent behavior for bit shifting right (left is filled with 0s always)
                registers[(instruction << 5) >> 27] >>= (registers[(instruction << 10) >> 27]);
                break;

            case 0b01001:
                printf("BITSHFTL Instruction");
                // instruction is already an unsigned integer, giving it consistent bit shifting behavior
                registers[(instruction << 5) >> 27]  <<= (registers[(instruction << 10) >> 27]);
                break;

            case 0b01010:
                printf("AND Instruction");
                registers[(instruction << 5) >> 27] &= registers[(instruction << 10) >> 27];
                break;

            case 0b01011:
                printf("XOR Instruction");
                registers[(instruction << 5) >> 27] ^= registers[(instruction << 10) >> 27];
                break;

            case 0b01100:
                printf("OR Instruction");
                registers[(instruction << 5) >> 27] |= registers[(instruction << 10) >> 27];
                break;

            case 0b01101:
                printf("NOT Instruction");
                registers[(instruction << 5) >> 27] = ~registers[(instruction << 10) >> 27];
                break;

            case 0b01110:
                printf("EQU Instruction");
                registers[(instruction << 5) >> 27] = registers[(instruction << 10) >> 27];
                break;

            default:
                printf("Opcode Identification Failure");
                break;
        }

        printf("\n\n");
        program_counter = registers[rip];
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
    unsigned int retval = 0;
    for (int i = start; i < start + length; i++) {
        retval += ((unsigned int)string[i] - 48) << (length - (i - start) - 1);
    }
    return (int)retval;
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

    // This is the logic for getting the PATH or directory to the textfile
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

    // This is the logic for getting the readas_type string from the arguments
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
// Unkown string sizes call for heap allocated memroy, don't forget to free it
// Use function like: char* str = IntToBinaryString(str, 19);
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