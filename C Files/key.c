#include "stdio.h"
#include "windows.h"

int main(void) {
    HANDLE handleout = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE handlein = GetStdHandle(STD_INPUT_HANDLE);
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    LARGE_INTEGER freq;

    int VK_Check[] = { 
        VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_BACK, VK_TAB, VK_RETURN, VK_SHIFT,
        VK_CONTROL, VK_MENU, VK_CAPITAL, VK_SPACE, VK_PRIOR, VK_NEXT, VK_HOME, 
        VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN, VK_SNAPSHOT, VK_INSERT, VK_DELETE,
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 
        'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 
        'W', 'X', 'Y', 'Z', 
        };

    // Enable Console to use virtual terminal sequences
    // I found that the sequences can still work without it, but I believe it may be required for certain instances
    DWORD dwmode = 0;
    GetConsoleMode(handleout, &dwmode);
    dwmode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(handleout, dwmode);

    system("cls");

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

        for (int i = 0x20; i < 0xDE; i++) {
            if (i % 10 == 0) {
                printf("\n");
            }

            //-1 means leftmost is set, and binary and will return 0 if lefmost bit is not set, and greater than 0 if otherwise
            if (GetAsyncKeyState(i) & -1) {
                printf("\x1b[35m\x1b[106m%c\x1b[0m ", (char)MapVirtualKeyA(i, MAPVK_VK_TO_CHAR));
            } else {
                printf("\x1b[0m%c ", (char)MapVirtualKeyA(i, MAPVK_VK_TO_CHAR));
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