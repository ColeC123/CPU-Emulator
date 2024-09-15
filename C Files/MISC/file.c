#include "stdio.h" 
#include "string.h"

int main(void) {
    char buffer[100];
    FILE* fptr = fopen("binary.bin", "wb");

    int int_data[] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\0'};
    size_t amount_written = fwrite(int_data, sizeof(int), 13, fptr);

    fclose(fptr);

    fptr = fopen("binary.bin", "rb");

    int buffer_size = 0;

    int temp[100];
    memset(temp, -1, sizeof(int));
    //continue while the end of the file has not been reached
    while (!feof(fptr)) {
        fread(temp + buffer_size, sizeof(int), 1, fptr);
        printf("%d ", temp[buffer_size]);
        buffer_size++;
    }
    fclose(fptr);

    for (int i = 0; i < buffer_size; i++) {
        buffer[i] = (char)temp[i];
    }
    buffer[buffer_size] = '\0';
    printf("\n\n%s\n", buffer);

    return 0;
}