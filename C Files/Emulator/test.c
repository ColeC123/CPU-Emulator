#include "cli.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "windows.h"

// minimum value for a signed integer, this will have the leftmost bit set only
#define INT_LM_BIT 2147483648
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

static int current_key_interrupt = 0;

// Spawns a thread to handle input separately for best speed
// ideally Windows will move this thread onto a separate core or hyperthread (hardware thread not software thread)
// where the emulator and input detector can run as fast as possible on each of their respecitve cores or hyperthreads
// however, it is up to the OS to decide how to handle and where to create the thread
DWORD WINAPI inputThread(LPVOID lpParam) {
    int *key_interrupt = (int *)lpParam;

    static int vk_check_size = 72;

    // This is ordered based on how the keys appear on the keyboard
    // Note: I left some keys out which aren't commonly used because this alone took a long time
    static int vk_check[] = {
        VK_DELETE,
        VK_INSERT,
        VK_SNAPSHOT,
        VK_OEM_3,
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        '0',
        VK_OEM_MINUS,
        VK_OEM_PLUS,
        VK_BACK,
        VK_TAB,
        'Q',
        'W',
        'E',
        'R',
        'T',
        'Y',
        'U',
        'I',
        'O',
        'P',
        VK_OEM_4,
        VK_OEM_6,
        VK_OEM_5,
        VK_CAPITAL,
        'A',
        'S',
        'D',
        'F',
        'G',
        'H',
        'J',
        'K',
        'L',
        VK_OEM_1,
        VK_OEM_7,
        VK_RETURN,
        VK_LSHIFT,
        'Z',
        'X',
        'C',
        'V',
        'B',
        'N',
        'M',
        VK_OEM_COMMA,
        VK_OEM_PERIOD,
        VK_OEM_2,
        VK_RSHIFT,
        VK_LCONTROL,
        VK_LWIN,
        VK_LMENU,
        VK_SPACE,
        VK_RMENU,
        VK_RCONTROL,
        VK_LEFT,
        VK_DOWN,
        VK_UP,
        VK_RIGHT,
        VK_SHIFT,
        VK_CONTROL,
        VK_MENU,
        VK_LBUTTON,
        VK_RBUTTON,
        VK_MBUTTON,
    };

    // creates an array where the index of a virtual key corresponds to an int value of your choosing (like a hash map)
    int conversion_array[256];

    // In a real computer key presses send a scan code that is different from an ascii character, but to simplify the
    // processing of key presses and to reduce the compute time for the emulator, I am converting the virtual keys
    // to ascii characters when possible

    // the ascii value for 0 - 9 are the same as the virtual key
    conversion_array[(int)'0'] = (int)'0';
    conversion_array[(int)'1'] = (int)'1';
    conversion_array[(int)'2'] = (int)'2';
    conversion_array[(int)'3'] = (int)'3';
    conversion_array[(int)'4'] = (int)'4';
    conversion_array[(int)'5'] = (int)'5';
    conversion_array[(int)'6'] = (int)'6';
    conversion_array[(int)'7'] = (int)'7';
    conversion_array[(int)'8'] = (int)'8';
    conversion_array[(int)'9'] = (int)'9';

    // the ascii values for captial A - captial Z are the same as the virtual key. In order to make input easier
    // to handle in the emulator, I have decided to map them to the lowercase versions
    conversion_array[(int)'A'] = (int)'a';
    conversion_array[(int)'B'] = (int)'b';
    conversion_array[(int)'C'] = (int)'c';
    conversion_array[(int)'D'] = (int)'d';
    conversion_array[(int)'E'] = (int)'e';
    conversion_array[(int)'F'] = (int)'f';
    conversion_array[(int)'G'] = (int)'g';
    conversion_array[(int)'H'] = (int)'h';
    conversion_array[(int)'I'] = (int)'i';
    conversion_array[(int)'J'] = (int)'j';
    conversion_array[(int)'K'] = (int)'k';
    conversion_array[(int)'L'] = (int)'l';
    conversion_array[(int)'M'] = (int)'m';
    conversion_array[(int)'N'] = (int)'n';
    conversion_array[(int)'O'] = (int)'o';
    conversion_array[(int)'P'] = (int)'p';
    conversion_array[(int)'Q'] = (int)'q';
    conversion_array[(int)'R'] = (int)'r';
    conversion_array[(int)'S'] = (int)'s';
    conversion_array[(int)'T'] = (int)'t';
    conversion_array[(int)'U'] = (int)'u';
    conversion_array[(int)'V'] = (int)'v';
    conversion_array[(int)'W'] = (int)'w';
    conversion_array[(int)'X'] = (int)'x';
    conversion_array[(int)'Y'] = (int)'y';
    conversion_array[(int)'Z'] = (int)'z';

    // Next up are the rest of the keys in vk_check
    conversion_array[VK_LBUTTON] = (int)VK_LBUTTON;    // left mouse button will be same as its virtual key
    conversion_array[VK_RBUTTON] = (int)VK_RBUTTON;    // right mouse button will be same as its virtual key
    conversion_array[VK_MBUTTON] = (int)VK_MBUTTON;    // middle mouse button will be same as its virtual key
    conversion_array[VK_BACK] = (int)'\b';             // backspace maps to ascii character for backspace (ascii backspace only shifts cursor back by one for each backsapce character, nothing else)
    conversion_array[VK_TAB] = (int)'\t';              // tab corresponds to the tab ascii character
    conversion_array[VK_RETURN] = (int)'\n';           // Enter key will correspond to newline
    conversion_array[VK_SHIFT] = (int)VK_SHIFT;        // there is no nice ascii equivalent for the shift key
    conversion_array[VK_CONTROL] = (int)VK_CONTROL;    // there is no nice ascii equivalenet for the control key
    conversion_array[VK_MENU] = (int)VK_MENU;          // there is no nice ascii equivalent for the alt key
    conversion_array[VK_CAPITAL] = (int)VK_CAPITAL;    // there is no nice ascii equivalent for the CAPSLOCK key
    conversion_array[VK_SPACE] = (int)' ';             // spacebar will correspond to the ascii character for a space
    conversion_array[VK_LEFT] = (int)VK_LEFT;          // there is no nice ascii equivalent for the arrow keys
    conversion_array[VK_UP] = (int)VK_UP;              // there is no nice ascii equivalent for the arrow keys
    conversion_array[VK_RIGHT] = (int)VK_RIGHT;        // there is no nice ascii equivalent for the arrow keys
    conversion_array[VK_DOWN] = (int)VK_DOWN;          // there is no nice ascii equivalent for the arrow keys
    conversion_array[VK_SNAPSHOT] = (int)VK_SNAPSHOT;  // there is no nice ascii equivalent for the PRTSC key
    conversion_array[VK_INSERT] = (int)VK_INSERT;      // there is no nice ascii equivalent for the INSERT key
    conversion_array[VK_DELETE] = (int)'\x7F';         // the delete key will correspond to the delete ascii character
    conversion_array[VK_LWIN] = (int)VK_LWIN;          // there is no ascii equivalent for the left windows key
    conversion_array[VK_LSHIFT] = (int)VK_LSHIFT;      // there is no ascii equivalent for left shift
    conversion_array[VK_RSHIFT] = (int)VK_RSHIFT;      // there is no ascii equivalent for right shift
    conversion_array[VK_LCONTROL] = (int)VK_LCONTROL;  // there is no ascii equivalent for left control
    conversion_array[VK_RCONTROL] = (int)VK_RCONTROL;  // there is no ascii equivalent for right control
    conversion_array[VK_RMENU] = (int)VK_RMENU;        // there is no ascii equivalent for right alt
    conversion_array[VK_LMENU] = (int)VK_LMENU;        // there is no ascii equivalent for left alt
    conversion_array[VK_OEM_1] = (int)';';             // VK_OEM_1 key corresponds to ;
    conversion_array[VK_OEM_2] = (int)'/';             // VK_OEM_2 key corresponds to /
    conversion_array[VK_OEM_3] = (int)'`';             // VK_OEM_3 key corresponds to `
    conversion_array[VK_OEM_4] = (int)'[';             // VK_OEM_4 key corresponds to [
    conversion_array[VK_OEM_5] = (int)'\\';            /*VK_OEM_5 key correpsonds to \ */
    conversion_array[VK_OEM_6] = (int)']';             // VK_OEM_6 key corresponds to ]
    conversion_array[VK_OEM_7] = (int)'\'';            // VK_OEM_7 key corresponds to '
    conversion_array[VK_OEM_PLUS] = (int)'=';          // the + key happens to be where = is also, but = is what shows up when shift is not pressed
    conversion_array[VK_OEM_COMMA] = (int)',';         // the , key corresponds to ,
    conversion_array[VK_OEM_MINUS] = (int)'-';         // the - key corresponds to -
    conversion_array[VK_OEM_PERIOD] = (int)'.';        // the . key corresponds to .

    // key tracker is used to specify whether or not the key has been pressed since the last check
    bool key_tracker[256];
    // initialize everything to false
    memset(key_tracker, false, 256 * sizeof(bool));

    while (1) {
        for (int i = 0; i < vk_check_size; i++) {
            // INT_LM_BIT has its l
            // most significant bit set means key is down in GetAsyncKeyState
            if (key_tracker[i] == false && GetAsyncKeyState(vk_check[i]) & (int)INT_LM_BIT) {
                // note that keypress has been recorded
                key_tracker[i] == true;

                // the bitwise or with INT_LM_BIT is to set the left bit equal to 1 to indicate the key is down
                *key_interrupt = conversion_array[vk_check[i]] | INT_LM_BIT;
            } else if (key_tracker[i] == true && !(GetAsyncKeyState(vk_check[i]) & (int)INT_LM_BIT)) {
                // note that key-up has been recorded
                key_tracker[i] == false;

                // No bitwise or is needed here since a 0 in the leftmost bit would be a negative value
                *key_interrupt = conversion_array[vk_check[i]];
            }
        }
    }
    return 0;
}

int main(void) {
    RAM = (int *)malloc(RAM_SIZE * sizeof(int));

    // if this is 0 that means no key interrupt was reported
    int key_interrupt;

    DWORD input_thread_id = 0;
    // Create the input thread immediately, allocate a stack size of 600 integer values, and pass in a pointer to the key_interrupt value
    HANDLE input_handle = CreateThread(0, 600 * sizeof(int), inputThread, &key_interrupt, 0, &input_thread_id);

    char *binarynum = NULL;
    char *binarynum2 = NULL;

    bool emulation_active = true;

    while (emulation_active) {
        bool running = true;

        ArgumentInputs((ArgList){.RAM = RAM, .emulation_active = &emulation_active});

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
    }

    // Terminate thread
    DWORD input_thread_exit_code = 0;
    GetExitCodeThread(input_handle, &input_thread_exit_code);
    TerminateThread(input_handle, input_thread_exit_code);
    CloseHandle(input_handle);

    // free heap allocated resources
    free(RAM);
    free(binarynum);
    free(binarynum2);

    printf("\nSuccessful Exit\n");

    return 0;
}