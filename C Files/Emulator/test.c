#include "inputThread.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "strfunctions.h"
#include "string.h"
#include "windows.h"

int *RAM;

// This will use up about 2 gigabytes of RAM
#define RAM_SIZE 536870912

// See Architecture Notes for more details about instruction set

// For now all of the registers will just be initialized to 0 at the start of the program
static int registers[17] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static int ZF_Flag = 0;  // zero result flag
static int PI_Flag = 0;  // positive int result flag
static int NI_Flag = 0;  // negative int result flag

// keeps track of current
static int program_counter = 0;

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

int main(int argc, char **argv) {
    FILE *ROM;
    // if no commands are passed to specify which binary file should be loaded into memory at startup, it will assume the name is ROM.bin
    if (argc == 1) {
        ROM = fopen("ROM.bin", "rb");

        if (ROM == NULL) {
            printf("No ROM file has been provided\n");
            return 1;
        }
    } else {
        // Open the binary file spcified by the command line arguments
        ROM = fopen(argv[1], "rb");

        if (ROM == NULL) {
            printf("Something went wrong trying to open the ROM file\n");
            return 1;
        }
    }

    RAM = (int *)malloc(RAM_SIZE * sizeof(int));

    // load the values in the ROM file directly into RAM, starting at index 0 of each
    int index_tracker = 0;
    while (!feof(ROM)) {
        fread(RAM + index_tracker, sizeof(int), 1, ROM);
        index_tracker++;
    }

    fclose(ROM);

    inputParams inputs = {.exit = false, .key_interrupt = 0};

    DWORD input_thread_id = 0;
    // Create the input thread immediately, allocate a stack size of 600 integer values, and pass in a pointer to the key_interrupt value
    HANDLE input_handle = CreateThread(0, 600 * sizeof(int), inputThread, &inputs, 0, &input_thread_id);

    char *binarynum = NULL;
    char *binarynum2 = NULL;

    bool emulation_active = true;

    bool running = true;

    // unsigned int makes for more consistent bit shifting behavior
    unsigned int instruction = 0;
    while (running) {
        instruction = RAM[program_counter];
        registers[rip]++;

        // Consider removing bitshift and replacing it with bitwise and (see example in key.c)
        binarynum = IntToBinaryString(binarynum, instruction, 32);
        printf("opcode: %s\n", binarynum);

        switch (instruction >> 27) {
            case 0b00000:
                printf("HLT Instruction");
                running = false;
                break;

            case 0b00001:
                printf("MOV Instruction");
                registers[(instruction << 5) >> 27] = RAM[registers[(instruction << 10) >> 27]];
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
                            registers[rip] = registers[(instruction << 10) >> 27];
                        }
                        break;

                    case 0b00001:
                        if (ZF_Flag == 0) {
                            registers[rip] = registers[(instruction << 10) >> 27];
                        }
                        break;

                    case 0b00010:
                        if (NI_Flag == 1) {
                            registers[rip] = registers[(instruction << 10) >> 27];
                        }
                        break;

                    case 0b00011:
                        if (PI_Flag == 1) {
                            registers[rip] = registers[(instruction << 10) >> 27];
                        }
                        break;

                    case 0b00100:
                        if (NI_Flag == 1 || ZF_Flag == 1) {
                            registers[rip] = registers[(instruction << 10) >> 27];
                        }
                        break;

                    case 0b00101:
                        if (PI_Flag == 1 || ZF_Flag == 1) {
                            registers[rip] = registers[(instruction << 10) >> 27];
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
                if ((instruction << 5) >> 31 == 0) {
                    registers[rsp]--;
                    RAM[registers[rsp]] = registers[(instruction << 6) >> 27];
                } else {
                    registers[(instruction << 6) >> 27] = RAM[registers[rsp]];
                    registers[rsp]++;
                }
                break;

            case 0b00111:
                printf("STORE Instruction");
                RAM[registers[(instruction << 5) >> 27]] = registers[(instruction << 10) >> 27];
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
                registers[(instruction << 5) >> 27] <<= (registers[(instruction << 10) >> 27]);
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

            case 0b10000:
                printf("IMUL Instruction");
                registers[(instruction << 5) >> 27] *= registers[(instruction << 10) >> 27];
                break;

            case 0b10001:
                printf("IDIV Instruction");
                registers[(instruction << 5) >> 27] /= registers[(instruction << 10) >> 27];
                break;

            case 0b10010:
                printf("CALL Instruction");
                // Push return address to stack
                registers[rsp]--;
                RAM[registers[rsp]] = registers[rip];
                // set rip to address of function
                registers[rip] = registers[(instruction << 5) >> 27];
                break;

            case 0b10011:
                printf("SETREG Instruction");
                registers[(instruction << 5) >> 27] = (instruction << 10) >> 10;
                break;

            default:
                printf("Opcode Identification Failure");
                break;
        }
        printf("\n\n");
        program_counter = registers[rip];
    }

    printf("\n\nrax: %d  |  rbx: %d\n", registers[rax], registers[rbx]);
    printf("rcx: %d  |  rdx: %d\n", registers[rcx], registers[rdx]);
    printf("rsi: %d  |  rdi: %d\n", registers[rsi], registers[rdi]);
    printf("rbp: %d  |  rsp: %d\n", registers[rbp], registers[rsp]);
    printf("r1:  %d  |  r2:  %d\n", registers[r1], registers[r2]);
    printf("r3:  %d  |  r4:  %d\n", registers[r3], registers[r4]);
    printf("r5:  %d  |  r6:  %d\n", registers[r5], registers[r6]);
    printf("r7:  %d  |  r8:  %d\n", registers[r7], registers[r8]);
    printf("rip: %d\n\n", registers[rip]);

    // The following code resets all registers, flags, and the program counter to zero
    // This allows for the code to continue to run in predictable ways when the next program is initiated
    program_counter = 0;
    for (int i = 0; i < 17; i++) {
        registers[i] = 0;
    }
    ZF_Flag = 0;
    PI_Flag = 0;
    NI_Flag = 0;

    // Exit thread
    inputs.exit = true;                       // lets thread know to end the loop and exit
    WaitForSingleObject(input_handle, 1000);  // Wait for up to one second for the thread to exit
    DWORD thread_exit_code;
    GetExitCodeThread(input_handle, &thread_exit_code);  // Get the exit code of the thread (should be 0) to make sure it returned properly
    printf("\nThread Exit Code: %d\n", (int)thread_exit_code);
    CloseHandle(input_handle);  // close the handle associated with that thread

    // free heap allocated resources
    free(RAM);
    free(binarynum);
    free(binarynum2);

    printf("\nSuccessful Exit\n");

    return 0;
}