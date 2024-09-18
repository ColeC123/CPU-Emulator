#include "stdio.h"
#include "windows.h"

#define INT_LM_BIT 2147483648

int main(void) {
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    printf("\nPage Granularity: %d | Allocation Granularity: %d\n", sysinfo.dwPageSize, sysinfo.dwAllocationGranularity);
    return 0;
}