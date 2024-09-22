#include "stdio.h"
#include "windows.h"

#define INT_LM_BIT 2147483648

int main(int argc, char** argv) {
    
    printf("%s\n", argv[0]);
    HANDLE fmap;

    //Using OpenFileMapping to establish inter process communication between child and parent processes
    fmap = OpenFileMappingA(
        FILE_MAP_ALL_ACCESS,  // want to have full access to the file
        FALSE,                // do not want handles to be inherited
        argv[0]               // the name of the file mapping object is passed in as a command line argument at the creation of this process
    );

    if (fmap == NULL) {
        printf("File Mapping Failed\n");
    }

    int* intview;

    intview = (int*)MapViewOfFile(
        fmap, //the handle to the file mapping object that a view is being created of
        FILE_MAP_ALL_ACCESS, //establishing full access to the file
        0, //high order dword offset
        0, //low order dword offset
        0 //number of bytes to map,
    );

    BOOL should_run = TRUE;
    while (should_run) {
        if (GetAsyncKeyState(0x1b) & 1) {
            should_run = FALSE;
        }

        printf("\x1b[0;0H");  // move cursor to the top left of the console
        for (int i = 0; i < 10; i++) {
            printf("%3d ", intview[i]);
        }
    }

    UnmapViewOfFile(intview);
    CloseHandle(fmap);

    return 0;
}