#include "stdio.h" 
#include "string.h"
#include "windows.h"
#include "stdlib.h"

//Note: In order to enable link time optimizations with clang, you have to include the command -fuse-ld=lld (this sets the linker to lld.exe, the clang linker)
int main(void) {
    HANDLE fhandle = CreateFileA(
        "binary.bin",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (fhandle == NULL) {
        printf("Error opening file\n");
    }

    HANDLE fmap = CreateFileMappingA(
        fhandle,
        NULL,
        PAGE_READWRITE,
        0,
        2000,
        "filemappingobject2"
    );

    if (fmap == NULL) {
        printf("Error creating file mapping object\n");
    }

    char* file_view = (char*) MapViewOfFile(
        fmap,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        2000
    );

    if (file_view == NULL) {
        printf("Error creating file view\n");
    }

    printf("Enter some text to be put into the file: ");
    fgets(file_view, 1000, stdin);

    UnmapViewOfFile(file_view);
    CloseHandle(fmap);
    CloseHandle(fhandle);
    return 0;
}