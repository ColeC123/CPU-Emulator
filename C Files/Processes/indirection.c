#include "stdio.h"
#include "windows.h"

int main(void) {
    // This is the executable that the CreateProcess function will create a process of
    char executablePath[260];
    printf("\nEnter the full path to gcc.exe (no quotes around path): ");

    //Get the path of to the compiler from standard input
    fgets(executablePath, 260, stdin);

    // fgets includes a newline character, so I have to remove it from the string
    for (int i = 0; executablePath[i] != '\0'; i++) {
        if (executablePath[i] == '\n') {
            executablePath[i] = '\0';
        }
    }

    BOOL should_run = TRUE;

    while (should_run) {
        char command_text[] = "Enter the command: ";
        printf("\n%s", command_text);

        // I decided to put an arbitrary limit of 600 character for the command line inputs passed into the executable
        char args[600];
        fgets(args, 600, stdin);
        for (int i = 0; args[i] != '\0'; i++) {
            if (args[i] == '\n') {
                args[i] = '\0';
            }
        }

        // This enables the handles to be inherited
        SECURITY_ATTRIBUTES securityAttribs = {
            .bInheritHandle = TRUE,                  // allows for handles to be inherited by the child process
            .nLength = sizeof(SECURITY_ATTRIBUTES),  // the size of this struct
            .lpSecurityDescriptor = NULL,            // default security
        };

        STARTUPINFO stinfo = {
            .cb = sizeof(STARTUPINFO),        // Lets windows know the size of this struct under the hood
            .lpReserved = NULL,               // Must always be NULL according to docs
            .lpDesktop = NULL,                // I am using the default Desktop settings
            .lpTitle = "IndirectGCC",         // The title of the process (this will show in the top part of the window)
            .cbReserved2 = 0,                 // Must always be 0 according to docs
            .lpReserved2 = NULL,              // Must always be NULL according to docs
            .dwFlags = STARTF_USESTDHANDLES,  // lets the window creation know to use standard handles
            // Specify the standard handles for creation
            .hStdError = GetStdHandle(STD_ERROR_HANDLE),
            .hStdInput = GetStdHandle(STD_INPUT_HANDLE),
            .hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE),
        };

        // This will store information about the created process
        PROCESS_INFORMATION proc_info;

        // Use this to create an instance of gcc.exe (a process)
        // Pass command line arguments to compile the specified directory
        CreateProcessA(
            executablePath,    // Name of the executable to create a process (instance of a program) of
            args,              // Command line arguments. This is what I am passing to GCC
            &securityAttribs,  // Process's Secutiry attributes.
            &securityAttribs,  // Thread attributes.
            TRUE,              // Whether or not to inherit handles.
            0,                 // The creation flags.
            NULL,              // The environement variables of the new process. I am just using the default
            NULL,              // This specifies the directory this process should have access to, I am using the default (NULL)
            &stinfo,           // This will use the information specified in the startup info struct for creating the process
            &proc_info);       // This will populate the process information struct with handles and process ids

        WaitForSingleObject(proc_info.hProcess, INFINITE);  // Wait for gcc to finish compiling
        DWORD exit_code = -1;                               // initialize it to -1 to see if GetExitCode returns something else
        GetExitCodeProcess(proc_info.hProcess, &exit_code); //Get the exit code of the process
        printf("\nGCC exitcode: %d\n", exit_code);

        CloseHandle(proc_info.hProcess);
        CloseHandle(proc_info.hThread);

        char exit_input[10];
        printf("\nIf you want to continue, type y, else type anything else: ");
        
        //Get a string from standard input and check to see if the first character is y or Y and exit the program if that is ture
        fgets(exit_input, 10, stdin);
        if (exit_input[0] != 'y' && exit_input[0] != 'Y') {
            should_run = FALSE;
        }
    }

    return 0;
}