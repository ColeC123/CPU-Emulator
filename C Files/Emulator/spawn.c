#include "stdio.h"
#include "windows.h"

int main(void) {
    SECURITY_ATTRIBUTES file_attribs = {
        .nLength = sizeof(SECURITY_ATTRIBUTES),
        .lpSecurityDescriptor = NULL,
        .bInheritHandle = TRUE,
    };

    HANDLE fhandle = CreateFileA(
        "IPCFIlE",                                          // Name of the file
        GENERIC_READ | GENERIC_WRITE,                       // The desired access
        0,                                                  // the sharing mode
        &file_attribs,                                      // the security attributes
        CREATE_NEW,                                         // the creation info
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE,  // the flags and attributes of the file
        NULL                                                // An optional input for a template file handle
    );

    if (fhandle == INVALID_HANDLE_VALUE) {
        printf("\nCreateFileA failed\n");
    }

    SECURITY_ATTRIBUTES fmap_attribs = {
        .nLength = sizeof(SECURITY_ATTRIBUTES),
        .lpSecurityDescriptor = NULL,
        .bInheritHandle = TRUE,
    };

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    printf("\nAllocation Granularity: %d\n", sysinfo.dwAllocationGranularity);

    DWORD bytes_written;
    for (int i = 0; i < (int)sysinfo.dwAllocationGranularity; i++) {
        WriteFile(fhandle, &i, sizeof(i), &bytes_written, NULL);
    }

    DWORD file_size = GetFileSize(fhandle, NULL);
    printf("\nFile Size: %d\n", (int)file_size);

    HANDLE fmap = CreateFileMappingA(
        fhandle,                   // handle to the file to be mapped
        &fmap_attribs,             // the security attributes of teh file map
        PAGE_READWRITE,            // the access rights
        0,                         // The high order dword size is 0
        0,                         // the low order dword size is 300
        "process_file_map_object"  // the name of the file mapping object
    );

    if (fmap == NULL) {
        printf("\nFile Mapping Failed\n");
    }

    LPVOID fview = MapViewOfFile(
        fmap,                 // The file mapping to be viewed
        FILE_MAP_ALL_ACCESS,  // The access writes for this File Mapping
        0,                    // The high order DWORD offset
        0,                    // The low order DWORD offset
        0                     // The size of the file memory to be mapped
    );

    if (fview == NULL) {
        printf("\nMapViewOfFile failed\n");
    }

    int* intview = (int*)fview;
    printf("\nFirst value in filemap: %d\n", intview[0]);
    printf("\nFirst value in filemap: %d\n", intview[1]);
    printf("\nFirst value in filemap: %d\n", intview[2]);

    STARTUPINFO stinfo = {
        .cb = sizeof(STARTUPINFO),  // Lets windows know the size of this struct under the hood
        .lpReserved = NULL,         // Must always be NULL according to docs
        .lpDesktop = NULL,          // I am using the default Desktop settings
        .lpTitle = "TestProcess",   // The title of the process (this will show in the top part of the window)
        .cbReserved2 = 0,           // Must always be 0 according to docs
        .lpReserved2 = NULL,        // Must always be NULL according to docs
    };

    // This will store information about the created process
    PROCESS_INFORMATION proc_info;

    CreateProcessA(
        "hello.exe",         // Name of the executable to create a process (instance of a program) of
        NULL,                // Command line arguments. I have none, so this is NULL
        NULL,                // Secutiry attributes. I am using the default, so this is NULL
        NULL,                // Thread attributes. I am using the default, so this is NULL
        FALSE,               // Whether or not to inherit handles. I want this process to be entirely separate, so this is set to false.
        CREATE_NEW_CONSOLE,  // The creation flags. I want to create a new console for this process, so it is set to CREATE_NEW_CONSOLE
        NULL,                // The environement variables of the new process. I am just using the default
        NULL,                // This specifies the directory this process should have access to, I am using the default (NULL)
        &stinfo,             // This will use the information specified in the startup info struct for creating the process
        &proc_info);         // This will populate the process information struct with handles and process ids

    UnmapViewOfFile(fview);  // Unmap the file view from memory

    CloseHandle(proc_info.hProcess);  // Don't need any handles to the spawned process in this function
    CloseHandle(proc_info.hThread);   // Don't need any handles to the spawned process's thread in this function

    CloseHandle(fhandle);  // Make sure to close these handles
    CloseHandle(fmap);     // Make sure to close these handles
    return 0;
}