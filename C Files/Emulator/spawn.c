#include "stdio.h"
#include "windows.h"

int main(void) {
    STARTUPINFO stinfo = {
        .cb = sizeof(STARTUPINFO),
        .lpReserved = NULL,
        .lpDesktop = NULL,
        .lpTitle = "TestProcess",
        .cbReserved2 = 0,
        .lpReserved2 = NULL,
    };

    PROCESS_INFORMATION proc_info;

    CreateProcessA(
        "C:\\Users\\colec\\C Programs\\CPU Emulator\\C Files\\Emulator\\hello.exe",
        NULL,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &stinfo,
        &proc_info
    );

    WaitForSingleObject(proc_info.hProcess, 2000);
    DWORD proc_exit_code;
    GetExitCodeProcess(proc_info.hProcess, &proc_exit_code);
    printf("\nProcess Exit Code: %d\n", (int)proc_exit_code);
    CloseHandle(proc_info.hProcess);
    CloseHandle(proc_info.hThread);
    return 0;
}