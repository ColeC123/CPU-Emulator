#include "stdio.h"
#include "windows.h"

#define INT_LM_BIT 2147483648

int main(void) {
    printf("Hello World!\n");
    while (!(GetAsyncKeyState(0x1B) & INT_LM_BIT)) {
    }
    ExitProcess(0);
}