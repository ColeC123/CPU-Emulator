#include "stdio.h"
#include "windows.h"
#include "string.h"

DWORD WINAPI myThread(LPVOID lpParamter) {
    int* test = (int*)lpParamter;
    int count = 0;
    int index = 0;
    DWORD start_time = 0;
    DWORD end_time = 0;
    while (1) {
        //timeGetTime requires linking with winmm.dll: gcc thread.c -I"C:\Windows\System32" -lwinmm -o thread
        end_time = timeGetTime();
        if (end_time - start_time >= 5) {
            start_time = end_time;
            count %= 1000;
            index %= 100;

            test[index] = count;

            count += (end_time % 300) + 200;
            index++;
        }
    }
    return 0;
}

int main(void) {
    int test[100];
    memset(test, 0, 100 * sizeof(int));

    DWORD myThreadID = 0;
    HANDLE myHandle = CreateThread(0, 0, myThread, test, 0, &myThreadID);

    system("cls");

    // hide cursor
    printf("\x1b[?25l");

    while (1) {
        // if user presses esc, then the program quits
        if (GetAsyncKeyState(0x1B) & -1) {
            break;
        }

        for (int i = 0; i < 100; i++) {
            if (i % 10 == 0) {
                printf("\n\n");
            }
            printf("%4d ", test[i]);
        }

        // ANSI escape sequence to move cursor to origin
        printf("\x1b[0;0H");
    }

    // Make Cursor visible again
    printf("\x1b[?25h]");
    system("cls");

    DWORD thread_exit_code = 0;
    GetExitCodeThread(myHandle, &thread_exit_code);

    TerminateThread(myHandle, thread_exit_code);
    CloseHandle(myHandle);

    return 0;
}