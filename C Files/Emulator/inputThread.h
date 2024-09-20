#ifndef INPUTTHREAD_H
#define INPUTTHREAD_H

#include "windows.h"
#include "stdbool.h"

// minimum value for a signed integer, this will have the leftmost bit set only
#define INT_LM_BIT 2147483648

typedef struct inputParams {
    //if key_interrupt is 0, then no key press was reported
    int key_interrupt;
    bool exit;
} inputParams;

// Spawns a thread to handle input separately for best speed
// ideally Windows will move this thread onto a separate core or hyperthread (hardware thread not software thread)
// where the emulator and input detector can run as fast as possible on each of their respecitve cores or hyperthreads
// however, it is up to the OS to decide how to handle and where to create the thread
DWORD WINAPI inputThread(LPVOID lpParam);

#endif