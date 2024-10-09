#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "strfunctions.h"
#include "string.h"
#include "windows.h"

// short for convert file condition
int convFileCondition(char** searchthrough, int argsize);

// short for modify disk condition (disk refers to the disk that the emulator uses for storing and loading values)
int modiskCondition(char** searchthrough, int argsize);

int getFileExtension(char* searchthrough, char* file_extension, int max_file_extension_size);

int intExp(int base, int exp);

int main(int argc, char** argv) {
    //<=1 since the name of the executable is included in the command line arguments
    if (argc <= 1) {
        printf("\n\x1b[31mError\x1b[0m: Not Enough arguments\n");
        return 0;
    }

    bool successfull_command_parse = false;
    for (int i = 0; i < 1; i++) {
        // the command convfile will be used to convert a specified file into another type of file
        if (strncmp(argv[1], "convfile", 8) == 0) {
            if (convFileCondition(argv, argc) == 1) {
                printf("\n\x1b[31mError\x1b[0m: Command Execution Failed\n");
                return 1;
            } else {
                successfull_command_parse = true;
                // break out of the loop since I only have this setup right now to handle one command at a time
                break;
            }
        } else if (strncmp(argv[1], "modisk", 6) == 0) {
            if (modiskCondition(argv, argc) == 1) {
                printf("\n\x1b[31mError\x1b[0m: Command Execution Failed\n");
                return 1;
            } else {
                successfull_command_parse = true;
                break;
            }
        }
    }

    if (successfull_command_parse == true) {
        printf("\n\x1b[32mSuccess\x1b[0m: Successfully parsed and executed command\n");
    } else {
        printf("\n\x1b[31mError\x1b[0m: Failed to parse command\n");
    }

    return 0;
}

// Proper order for convfile command: cli convfile FilePath WhatToConvertFileTo OutputFile
/*
Note: for file paths if a space is contained in the file path, then it must be surrounded by quotes
the parser that passes these commands into this process will automatically remove the quotes
*/
int convFileCondition(char** searchthrough, int argsize) {
    if (argsize < 5) {
        printf("\n\x1b[31mError\x1b[0m: convfile command requires the following format for command line arguments\n");
        printf("cli convfile ReadFilePath WhatToConvertFileTo Output/WriteFilePath\n");
        printf("WhatToConvertFileTo options:\n");
        printf("bt2b -- binary text (.txt) to binary file (.bin)\n");
        printf("b2bt -- binary file (.bin) to binary text (.txt)\n");
        return 1;
    }

    char file_type[10];
    if (getFileExtension(searchthrough[2], file_type, 10) == 1) {
        return 1;
    }

    if (strncmp(file_type, "txt", 3) != 0 && strncmp(file_type, "bin", 3) != 0) {
        printf("\n\x1b[31mError\x1b[0m: The file extensions .%s is not supported\n", file_type);
        return 1;
    }

    // This is used for debugging
    // printf("\nFile Type: %s\n", file_type);

    // The files that will be opened and written to respectively.
    FILE* given_file;
    FILE* output_file;

    // bt2b stands for binary text in a .txt file (as in 001000) to binary file (.bin)
    if (strncmp(searchthrough[3], "bt2b", 4) == 0 && strncmp(file_type, "txt", 3) == 0) {
        given_file = fopen(searchthrough[2], "rt");

        if (given_file == NULL) {
            printf("\n\x1b[31mError\x1b[0m: Failed to open file\n");
            // This will indicate in the main function to exit since the file couldn't be opened
            return 1;
        }

        output_file = fopen(searchthrough[4], "wb");

        // This is used to get the size of te file to know how much to allocate with malloc
        fseek(given_file, 0L, SEEK_END);
        int file_string_size = ftell(given_file);
        char* file_string = (char*)malloc((file_string_size + 1) * sizeof(char));

        int file_string_logical_size = 0;
        rewind(given_file);

        bool semicolon_present = false;

        while (!feof(given_file)) {
            char temp_char;
            fread(&temp_char, sizeof(char), 1, given_file);

            if (temp_char == ';') {
                semicolon_present = true;
            } else if (temp_char == '\n' || temp_char == '\0') {
                semicolon_present = false;
            }

            if (semicolon_present == false && temp_char != ' ') {
                file_string[file_string_logical_size] = temp_char;
                file_string_logical_size++;
            }
        }
        file_string[file_string_logical_size] = '\0';

        /*
        printf("\nTranslated Text File\n%s\n", file_string);

        printf("\nFile String logical size: %d\n", file_string_logical_size);
        printf("File String actual size: %d\n\n", file_string_size);

        printf("\nBinary to Int Conversion\n");
        */

        int last_line_start_index = 0;

        // file_string_logical_size + 1 accounts for the fact that there is a null terminator at that index
        for (int i = 0; i < file_string_logical_size + 1; i++) {
            if (file_string[i] == '\n' || file_string[i] == '\0') {
                // this accounts for the case where the only character on a line is a newline character in teh text file
                if (i - last_line_start_index > 0) {
                    int temp_int = BinaryStringToInt(file_string, last_line_start_index, i - last_line_start_index);
                    fwrite(&temp_int, sizeof(int), 1, output_file);

                    /*
                    Used for print debugging:
                    char* bin_str = NULL;
                    bin_str = IntToBinaryString(bin_str, temp_int, 32);
                    printf("index: %d | Number: %u | Binary rep: %s\n", i, (unsigned int)temp_int, bin_str);
                    free(bin_str);
                    */
                } else {
                    /*
                    For the case where the line contains only a newline character (i - last_line_start_index = 0)
                    that spot in the file will be initialized to 0
                    */

                    int temp_int = 0;
                    fwrite(&temp_int, sizeof(int), 1, output_file);

                    /*
                    Used for print debugging:
                    char* bin_str = NULL;
                    bin_str = IntToBinaryString(bin_str, temp_int, 32);
                    printf("index: %d | Number: %u | Binary rep: %s\n", i, (unsigned int)temp_int, bin_str);
                    free(bin_str);
                    */
                }

                // account for fact that next line start will occur after \n, or at i + 1 index
                last_line_start_index = i + 1;
            }
        }

        free(file_string);

        fclose(given_file);
        fclose(output_file);

        return 0;
    }

    // b2bt stands for binary file (.bin) to binary textfile (.txt)
    if (strncmp(searchthrough[3], "b2bt", 4) == 0 && strncmp(file_type, "bin", 3) == 0) {
        given_file = fopen(searchthrough[2], "rb");

        if (given_file == NULL) {
            printf("\n\x1b[31mError\x1b[0m: Failed to open file\n");
            // lets the main function to exit since it failed to open the given file
            return 1;
        }

        output_file = fopen(searchthrough[4], "wt");

        // Get the size of the file to know how much memory to allocate
        int file_int_size = 0;
        fseek(given_file, 0L, SEEK_END);

        file_int_size = (int)ftell(given_file);
        rewind(given_file);

        int* file_int = (int*)malloc(file_int_size * sizeof(int));
        int file_int_logical_size = 0;

        // loop through the file until the end of the file and read the contents of the file as an int to a buffer
        while (!feof(given_file)) {
            fread(file_int + file_int_logical_size, sizeof(int), 1, given_file);
            file_int_logical_size++;
        }

        // Write the binary of the file as a character string of 1s and 0s
        char* binary_string = NULL;
        for (int i = 0; i < file_int_logical_size; i++) {
            binary_string = IntToBinaryString(binary_string, file_int[i], 32);
            fprintf(output_file, "%s", binary_string);
            fprintf(output_file, "\n");
        }

        free(file_int);
        fclose(given_file);
        fclose(output_file);

        return 0;
    }

    // The above if conditionals make up the actual content of this function, and will return 0 if they succeed.
    // Therefore, if this return is reached, that means they all failed, and therefore something went wrong in processing the command
    return 1;
}

// Command should look like the following format: cli modisk pathToDisk disk_const_offset pathToBinFileToBeInserted insertFileRange
// Example: cli modisk CPUEmulatorDisk.bin 0 ROM.bin 0-e (start of insert file to end of insert file)
// The above command would insert the contents of ROM.bin into CPUEmulatorDisk.bin starting at index 0
// The constant offset can be thought of as the starting index in the disk file where the information in the specified file is placed
// Note: the disk file for the emulator will always be named CPUEmulatorDisk.bin, but the path may vary
int modiskCondition(char** searchthrough, int argsize) {
    if (argsize < 6) {
        printf("\n\x1b[31mError\x1b[0m: modisk command requires the following format for command line arguments\n");
        printf("Should follow this format: cli modisk pathToDisk disk_const_offset pathToBinFileToBeInserted insert_file_range\n");
        printf("Example: cli modisk disk.bin 0 (start of disk.bin) test.bin 0-e (beginning to end of file, or a specific number)\n");
        return 1;
    }

    char file_type[10];
    if (getFileExtension(searchthrough[2], file_type, 10) == 1) {
        return 1;
    }

    if (strncmp(file_type, "bin", 3) != 0) {
        printf("\x1b[31mError\x1b[0m: modisk command requires the input file to be of type .bin\n");
        return 1;
    }

    wchar_t wide_path[260];
    int wide_path_size = 0;

    // The following path converts the disk file path, a char string, to a wchar_t string
    // so that it will work with CreateFileW
    for (int i = 0; searchthrough[2][i] != '\0' && i < 260; i++) {
        wide_path[i] = (wchar_t)searchthrough[2][i];
        wide_path_size++;
    }
    wide_path[wide_path_size] = (wchar_t)'\0';

    // Windows api must be used to open the disk file so that writing to it does not overwrite what is already there
    HANDLE disk = CreateFileW(
        wide_path,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_WRITE | FILE_SHARE_DELETE | FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (disk == INVALID_HANDLE_VALUE) {
        printf("\x1b[31mError\x1b[0m: Disk file does not exist or was unable to open it\n");
        return 1;
    }

    wchar_t read_file_path[260];
    int read_file_size = 0;

    // The following loop converts readfile path, a char string, to a wchar_t string so it will work
    // with the CreateFileW function
    for (int i = 0; searchthrough[4][i] != '\0' && i < 260; i++) {
        read_file_path[i] = (wchar_t)searchthrough[4][i];
        read_file_size++;
    }
    read_file_path[read_file_size] = (wchar_t)'\0';

    HANDLE read_file = CreateFileW(
        read_file_path,                                          // Name of the file
        GENERIC_READ | GENERIC_READ,                             // Access rights
        FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,  // Sharing rights
        NULL,                                                    // Security attributes, I am using the default
        OPEN_EXISTING,                                           // CreationDisposition, I want to always open an existing file
        FILE_ATTRIBUTE_NORMAL,                                   // Default attributes for this file
        NULL                                                     // I don't want a template file, so this is NULL
    );

    // Verify that the opening of the file succeeded
    if (read_file == INVALID_HANDLE_VALUE) {
        CloseHandle(disk);
        printf("\x1b[31mError\x1b[0m: Read file does not exist or was unable to open it\n");
        return 1;
    }

    // This searches through offset number provided and makes sure that the only characters here are
    // numbers, since atoi will return 0 if the given string is not a number, and 0 is actually a
    // valid disk offset (that would mean just insert everything at the beginning of the file)
    for (int i = 0; searchthrough[3][i] != '\0'; i++) {
        if (!((int)searchthrough[3][i] - 48 <= 57)) {
            printf("\x1b[31mError\x1b[0m: The disk file offset entered is not a number\n");
            CloseHandle(disk);
            CloseHandle(read_file);
            return 1;
        }
    }

    // This is the file offset for the disk
    int file_offset = atoi(searchthrough[3]);

    // Find the location of the dash to know how to find the two numbers in the range
    int dash_location = -1;
    for (int i = 0; searchthrough[5][i] != '\0'; i++) {
        if (searchthrough[5][i] == '-') {
            dash_location = i;
        }
    }

    // dash_location equaling -1 signal that no dash was found, meaning there is a formatting error
    if (dash_location == -1) {
        printf("\x1b[31mError\x1b[0m: The insert file offset entered is needs to be a range\n");
        printf("Example: 0-e (beginning to end of file) or 30-400 (30th byte position to 400th byte position)\n");
        CloseHandle(disk);
        CloseHandle(read_file);
        return 1;
    }

    int first_half_range = 0;
    // Make sure that the first part of the range contains only numbers
    for (int i = 0; i < dash_location; i++) {
        if (!((int)searchthrough[5][i] - 48 <= 57)) {
            printf("\x1b[31mError\x1b[0m: The first half of the insert file range entered is not a number\n");
            CloseHandle(disk);
            CloseHandle(read_file);
            return 1;
        }

        // This calculates the first half of the range while moving through the string
        // Have to use dash_location - 1 - i index of the string since the end of the string contains
        // the smaller values numerically (think place values in 999 for instance)
        first_half_range += ((int)searchthrough[5][dash_location - 1 - i] - 48) * intExp(10, i);
    }

    int second_half_range = 0;

    //Get the size of the string in the 5th command line argument (Windows should automatically null terminate this string)
    int arg5_size = strlen(searchthrough[5]);

    // This would be if after the dash there is an e, meaning end of file
    if (searchthrough[5][dash_location + 1] == 'e' && searchthrough[5][dash_location + 2] == '\0') {
        // This gets the size of the file in bytes
        second_half_range = GetFileSize(read_file, NULL);
    } else {
        for (int i = dash_location + 1; i < arg5_size; i++) {
            if (!((int)searchthrough[5][i] - 48 <= 57)) {
                printf("\x1b[31mError\x1b[0m: The first half of the insert file range entered is not a number\n");
                CloseHandle(disk);
                CloseHandle(read_file);
                return 1;
            }

            // This calculates the first half of the range while moving through the string
            // Have to use dash_location - 1 - i index of the string since the end of the string contains
            // the smaller values numerically (think place values in 999 for instance)
            // arg5_size - 1 - (i - dash_location -1) starts at the final index of searchthrough[5], and decrements
            // down by 1 as i increases by one
            second_half_range += ((int)searchthrough[5][arg5_size - 1 - (i - dash_location - 1)] - 48) * intExp(10, i - dash_location - 1);
        }
    }

    // Create an array of BYTES to be filled by ReadFile function
    BYTE* read_file_buffer = (BYTE*)malloc((second_half_range - first_half_range) * sizeof(BYTE));

    if (SetFilePointer(read_file, first_half_range, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
        printf("\x1b[31mError\x1b[0m: Failed to set insert file offset\n");
        CloseHandle(disk);
        CloseHandle(read_file);
        free(read_file_buffer);
        return 1;
    }

    // Check to make sure that the ReadFile function succeeds
    if (ReadFile(read_file, read_file_buffer, (second_half_range - first_half_range) * sizeof(BYTE), NULL, NULL) == FALSE) {
        printf("\x1b[31mError\x1b[0m: Failed to read from the read file\n");
        CloseHandle(disk);
        CloseHandle(read_file);
        free(read_file_buffer);
        return 1;
    }

    // Set the file pointer to the correct offset in the disk file, and check to see if it fails
    if (SetFilePointer(disk, file_offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
        printf("\x1b[31mError\x1b[0m: Failed to set disk file offset\n");
        CloseHandle(disk);
        CloseHandle(read_file);
        free(read_file_buffer);
        return 1;
    }

    // Write to the file and retreive information to determine success
    BOOL write_completed = WriteFile(disk, read_file_buffer, (second_half_range - first_half_range) * sizeof(BYTE), NULL, NULL);
    int write_error_code = GetLastError();

    // Check if WriteFile function failed or succeeded. ERROR_IO_PENDING is not actually a failure, it just means
    // that the write has not ocurred yet, but will soon
    if (write_completed == FALSE && write_error_code != ERROR_IO_PENDING) {
        printf("\x1b[31mError\x1b[0m: Failed to write to given disk file\n");
        CloseHandle(disk);
        CloseHandle(read_file);
        free(read_file_buffer);
        return 1;
    }

    CloseHandle(disk);
    CloseHandle(read_file);
    free(read_file_buffer);

    return 0;
}

// searches through a given string for its file extension, and will put the file extension into a provided string
int getFileExtension(char* searchthrough, char* file_extension, int max_file_extension_size) {
    int file_extension_count = 0;
    bool period_found = false;

    // This gets the last index of a period in the file path.
    // This is done to ensure that any periods earlier on in the path are not misinterpreted as the file type
    int period_last_index = -1;
    for (int i = 0; searchthrough[i] != '\0'; i++) {
        if (searchthrough[i] == '.') {
            period_last_index = i;
        }
    }

    if (period_last_index == -1) {
        printf("\n\x1b[31mError\x1b[0m: File extension not found. Please include a file extension when reading files.\n");
        return 1;
    }

    // given the last index of a period, add 1 to get the start of the file type and record the file type to file_type string
    // Even though file_type_count can store 7 character, it is less than 6 to account for the extra null terminator character required
    for (int i = period_last_index + 1; searchthrough[i] != '\0' && file_extension_count < max_file_extension_size - 1; i++) {
        file_extension[file_extension_count] = searchthrough[i];
        file_extension_count++;
    }
    file_extension[file_extension_count] = '\0';

    return 0;
}

int intExp(int base, int exp) {
    int result = 1;

    for (int i = 0; i < exp; i++) {
        result *= base;
    }

    return result;
}
