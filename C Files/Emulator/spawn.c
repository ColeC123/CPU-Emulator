#include "stdio.h"
#include "windows.h"

int main(void) {
    STARTUPINFO stinfo = {
        .cb = sizeof(STARTUPINFO), //Lets windows know the size of this struct under the hood
        .lpReserved = NULL, //Must always be NULL according to docs
        .lpDesktop = NULL, //I am using the default Desktop settings
        .lpTitle = "TestProcess", //The title of the process (this will show in the top part of the window)
        .cbReserved2 = 0, //Must always be 0 according to docs
        .lpReserved2 = NULL, //Must always be NULL according to docs
    };

    //This will store information about the created process
    PROCESS_INFORMATION proc_info;

    CreateProcessA(
        "hello.exe", //Name of the executable to create a process (instance of a program) of
        NULL, //Command line arguments. I have none, so this is NULL
        NULL, //Secutiry attributes. I am using the default, so this is NULL
        NULL, //Thread attributes. I am using the default, so this is NULL
        FALSE, //Whether or not to inherit handles. I want this process to be entirely separate, so this is set to false.
        CREATE_NEW_CONSOLE, //The creation flags. I want to create a new console for this process, so it is set to CREATE_NEW_CONSOLE
        NULL, //The environement variables of the new process. I am just using the default
        NULL, //This specifies the directory this process should have access to, I am using the default (NULL)
        &stinfo, //This will use the information specified in the startup info struct for creating the process
        &proc_info); //This will populate the process information struct with handles and process ids

    CloseHandle(proc_info.hProcess); //Don't need any handles to the spawned process in this function
    CloseHandle(proc_info.hThread); //Don't need any handles to the spawned process's thread in this function
    return 0;
}