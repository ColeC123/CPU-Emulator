#include "stdio.h"
#include "windows.h"

static int vk_check_size = 73;
static int vk_check[] = {
    VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_BACK, VK_TAB, VK_RETURN, VK_SHIFT,
    VK_CONTROL, VK_MENU, VK_CAPITAL, VK_SPACE, VK_HOME,
    VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN, VK_SNAPSHOT, VK_INSERT, VK_DELETE,
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', VK_LWIN, VK_OEM_1, VK_OEM_PLUS, VK_OEM_COMMA, VK_OEM_MINUS,
    VK_OEM_PERIOD, VK_OEM_2, VK_OEM_3, VK_OEM_4, VK_OEM_5, VK_OEM_6, VK_OEM_7,
    VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL, VK_RMENU, VK_LMENU,
    };

static char conversion_array[256][20];

char* vkToString(int vk);

void genVkConversionArray(void);

int main(void) {
    HANDLE handleout = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE handlein = GetStdHandle(STD_INPUT_HANDLE);
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    LARGE_INTEGER freq;

    // Enable Console to use virtual terminal sequences
    // I found that the sequences can still work without it, but I believe it may be required for certain instances
    DWORD dwmode = 0;
    GetConsoleMode(handleout, &dwmode);
    dwmode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(handleout, dwmode);

    system("cls");

    genVkConversionArray();

    // hide cursor
    printf("\x1b[?25l");
    while (1) {
        // virtual terminal sequence for moving cursor. Supported on Windows, MacOS, and Linux
        printf("\x1b[0;0H");

        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&start);

        //-1 will always have the leftmost bit set in twos complement
        if (GetAsyncKeyState(0x1B) & -1) {
            break;
        }

        for (int i = 0; i < vk_check_size; i++) {
            if (i % 12 == 0) {
                printf("\n");
            }

            //-1 means leftmost is set, and binary and will return 0 if lefmost bit is not set, and greater than 0 if otherwise
            if (GetAsyncKeyState(vk_check[i]) & -1) {
                printf("\x1b[35m\x1b[106m%s\x1b[0m ", conversion_array[vk_check[i]]);
            } else {
                printf("\x1b[0m%s ", conversion_array[vk_check[i]]);
            }
        }

        QueryPerformanceCounter(&end);
        double time = (double)(((double)end.QuadPart - (double)start.QuadPart) / (double)freq.QuadPart);
        printf("\n\x1b[0m\nTime: %f miliseconds\n\n", time * 1000);
        printf("Frequency: %f program cycles per second\n", 1 / time);

        FlushConsoleInputBuffer(handlein);
    }
    // Set text color to default and show cursor again
    printf("\x1b[0m");
    // Make Cursor visible again
    printf("\x1b[?25h]");

    system("cls");
    return 0;
}

//Generates a 2 dimensional array where the index of the virtual keycode contains a string of the ascii characters that you
//want to be associated with that virtual keycode
void genVkConversionArray(void) {

    for (int i = (int)'0'; i <= (int)'9'; i++) {
        conversion_array[i][0] = (char)i;
        conversion_array[i][1] = '\0';
    }

    for (int i = (int)'A'; i <= (int)'Z'; i++) {
        conversion_array[i][0] = (char)i;
        conversion_array[i][1] = '\0';
    }

    strncpy(conversion_array[VK_LBUTTON], "LEFTMOUSEBUTTON", 20);
    strncpy(conversion_array[VK_RBUTTON], "RIGHTMOUSEBUTTON", 20);
    strncpy(conversion_array[VK_MBUTTON], "MIDDLEMOUSEBUTTON", 20);
    strncpy(conversion_array[VK_BACK], "BACKSPACE", 20);
    strncpy(conversion_array[VK_TAB], "TAB", 20);
    strncpy(conversion_array[VK_RETURN], "ENTER", 20);
    strncpy(conversion_array[VK_SHIFT], "SHIFT", 20);
    strncpy(conversion_array[VK_CONTROL], "CTRL", 20);
    strncpy(conversion_array[VK_MENU], "ALT", 20);
    strncpy(conversion_array[VK_CAPITAL], "CAPSLOCK", 20);
    strncpy(conversion_array[VK_SPACE], "SPACEBAR", 20);
    strncpy(conversion_array[VK_LEFT], "LEFTARROW", 20);
    strncpy(conversion_array[VK_UP], "UPARROW", 20);
    strncpy(conversion_array[VK_RIGHT], "RIGHTARROW", 20);
    strncpy(conversion_array[VK_DOWN], "DOWNARROW", 20);
    strncpy(conversion_array[VK_SNAPSHOT], "PRINTSCREEN", 20);
    strncpy(conversion_array[VK_INSERT], "INSERT", 20);
    strncpy(conversion_array[VK_DELETE], "DELETE", 20);
    strncpy(conversion_array[VK_LWIN], "LEFTWINDOWS", 20);
    strncpy(conversion_array[VK_LSHIFT], "LEFTSHIFT", 20);
    strncpy(conversion_array[VK_RSHIFT], "RIGHTSHIFT", 20);
    strncpy(conversion_array[VK_LCONTROL], "LEFTCONTROL", 20);
    strncpy(conversion_array[VK_RCONTROL], "RIGHTCONTROL", 20);
    strncpy(conversion_array[VK_RMENU], "RIGHTALT", 20);
    strncpy(conversion_array[VK_LMENU], "LEFTALT", 20);
    strncpy(conversion_array[VK_OEM_1], ";" , 20);
    strncpy(conversion_array[VK_OEM_2], "/", 20);
    strncpy(conversion_array[VK_OEM_3], "`", 20);
    strncpy(conversion_array[VK_OEM_4], "[", 20);
    strncpy(conversion_array[VK_OEM_5], "\\", 20);
    strncpy(conversion_array[VK_OEM_6], "]", 20);
    strncpy(conversion_array[VK_OEM_7], "'", 20);
    strncpy(conversion_array[VK_OEM_PLUS], "+", 20);
    strncpy(conversion_array[VK_OEM_COMMA], ",", 20);
    strncpy(conversion_array[VK_OEM_MINUS], "-", 20);
    strncpy(conversion_array[VK_OEM_PERIOD], ".", 20);
}