//inclusion gaurds to prevent the header file from being included multiple times
#ifndef STRFUNCTIONS_H
#define STRFUNCTIONS_H

extern int BinaryStringToInt(char *string, int start, int length);

extern int strfind(char *searchfor, char *in, int searchfor_size, int in_size);

extern int HexStringToInt(char *string, int start, int length);

extern int IntMin(int arg1, int arg2);

extern char *IntToBinaryString(char *str, int input, int padding);

extern int strInsertChar(char *str, int index, char value, int size);

extern int charfind(char searchfor, char *in, int in_size);

#endif