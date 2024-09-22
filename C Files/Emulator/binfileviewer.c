#include "stdio.h"
#include "stdlib.h"
#include "strfunctions.h"

int main(int argc, char** argv) {
    FILE* fptr;
    fptr = fopen(argv[1], "rb");

    char* bin_string = NULL;

    while (!feof(fptr)) {
        int temp_int;
        fread(&temp_int, sizeof(int), 1, fptr);

        bin_string = IntToBinaryString(bin_string, temp_int, 32);

        printf("%s\n", bin_string);
    }

    free(bin_string);
    return 0;
}