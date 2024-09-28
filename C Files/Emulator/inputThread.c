#include "inputThread.h"

// minimum value for a signed integer, this will have the leftmost bit set only
#define INT_LM_BIT 2147483648

// Spawns a thread to handle input separately for best speed
// ideally Windows will move this thread onto a separate core or hyperthread (hardware thread not software thread)
// where the emulator and input detector can run as fast as possible on each of their respecitve cores or hyperthreads
// however, it is up to the OS to decide how to handle and where to create the thread
DWORD WINAPI inputThread(LPVOID lpParam) {
    inputParams* inputs = (inputParams*)lpParam;

    static int vk_check_size = 72;

    // This is ordered based on how the keys appear on the keyboard
    // Note: I left some keys out which aren't commonly used because this alone took a long time
    static int vk_check[] = {
        VK_DELETE,
        VK_INSERT,
        VK_SNAPSHOT,
        VK_OEM_3,
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        '0',
        VK_OEM_MINUS,
        VK_OEM_PLUS,
        VK_BACK,
        VK_TAB,
        'Q',
        'W',
        'E',
        'R',
        'T',
        'Y',
        'U',
        'I',
        'O',
        'P',
        VK_OEM_4,
        VK_OEM_6,
        VK_OEM_5,
        VK_CAPITAL,
        'A',
        'S',
        'D',
        'F',
        'G',
        'H',
        'J',
        'K',
        'L',
        VK_OEM_1,
        VK_OEM_7,
        VK_RETURN,
        VK_LSHIFT,
        'Z',
        'X',
        'C',
        'V',
        'B',
        'N',
        'M',
        VK_OEM_COMMA,
        VK_OEM_PERIOD,
        VK_OEM_2,
        VK_RSHIFT,
        VK_LCONTROL,
        VK_LWIN,
        VK_LMENU,
        VK_SPACE,
        VK_RMENU,
        VK_RCONTROL,
        VK_LEFT,
        VK_DOWN,
        VK_UP,
        VK_RIGHT,
        VK_SHIFT,
        VK_CONTROL,
        VK_MENU,
        VK_LBUTTON,
        VK_RBUTTON,
        VK_MBUTTON,
    };

    // creates an array where the index of a virtual key corresponds to an int value of your choosing (like a hash map)
    int conversion_array[256];

    // In a real computer key presses send a scan code that is different from an ascii character, but to simplify the
    // processing of key presses and to reduce the compute time for the emulator, I am converting the virtual keys
    // to ascii characters when possible

    // the ascii value for 0 - 9 are the same as the virtual key
    conversion_array[(int)'0'] = (int)'0';
    conversion_array[(int)'1'] = (int)'1';
    conversion_array[(int)'2'] = (int)'2';
    conversion_array[(int)'3'] = (int)'3';
    conversion_array[(int)'4'] = (int)'4';
    conversion_array[(int)'5'] = (int)'5';
    conversion_array[(int)'6'] = (int)'6';
    conversion_array[(int)'7'] = (int)'7';
    conversion_array[(int)'8'] = (int)'8';
    conversion_array[(int)'9'] = (int)'9';

    // the ascii values for captial A - captial Z are the same as the virtual key. In order to make input easier
    // to handle in the emulator, I have decided to map them to the lowercase versions
    conversion_array[(int)'A'] = (int)'a';
    conversion_array[(int)'B'] = (int)'b';
    conversion_array[(int)'C'] = (int)'c';
    conversion_array[(int)'D'] = (int)'d';
    conversion_array[(int)'E'] = (int)'e';
    conversion_array[(int)'F'] = (int)'f';
    conversion_array[(int)'G'] = (int)'g';
    conversion_array[(int)'H'] = (int)'h';
    conversion_array[(int)'I'] = (int)'i';
    conversion_array[(int)'J'] = (int)'j';
    conversion_array[(int)'K'] = (int)'k';
    conversion_array[(int)'L'] = (int)'l';
    conversion_array[(int)'M'] = (int)'m';
    conversion_array[(int)'N'] = (int)'n';
    conversion_array[(int)'O'] = (int)'o';
    conversion_array[(int)'P'] = (int)'p';
    conversion_array[(int)'Q'] = (int)'q';
    conversion_array[(int)'R'] = (int)'r';
    conversion_array[(int)'S'] = (int)'s';
    conversion_array[(int)'T'] = (int)'t';
    conversion_array[(int)'U'] = (int)'u';
    conversion_array[(int)'V'] = (int)'v';
    conversion_array[(int)'W'] = (int)'w';
    conversion_array[(int)'X'] = (int)'x';
    conversion_array[(int)'Y'] = (int)'y';
    conversion_array[(int)'Z'] = (int)'z';

    // Next up are the rest of the keys in vk_check
    conversion_array[VK_LBUTTON] = (int)VK_LBUTTON;    // left mouse button will be same as its virtual key
    conversion_array[VK_RBUTTON] = (int)VK_RBUTTON;    // right mouse button will be same as its virtual key
    conversion_array[VK_MBUTTON] = (int)VK_MBUTTON;    // middle mouse button will be same as its virtual key
    conversion_array[VK_BACK] = (int)'\b';             // backspace maps to ascii character for backspace (ascii backspace only shifts cursor back by one for each backsapce character, nothing else)
    conversion_array[VK_TAB] = (int)'\t';              // tab corresponds to the tab ascii character
    conversion_array[VK_RETURN] = (int)'\n';           // Enter key will correspond to newline
    conversion_array[VK_SHIFT] = (int)VK_SHIFT;        // there is no nice ascii equivalent for the shift key
    conversion_array[VK_CONTROL] = (int)VK_CONTROL;    // there is no nice ascii equivalenet for the control key
    conversion_array[VK_MENU] = (int)VK_MENU;          // there is no nice ascii equivalent for the alt key
    conversion_array[VK_CAPITAL] = (int)VK_CAPITAL;    // there is no nice ascii equivalent for the CAPSLOCK key
    conversion_array[VK_SPACE] = (int)' ';             // spacebar will correspond to the ascii character for a space
    conversion_array[VK_LEFT] = (int)VK_LEFT;          // there is no nice ascii equivalent for the arrow keys
    conversion_array[VK_UP] = (int)VK_UP;              // there is no nice ascii equivalent for the arrow keys
    conversion_array[VK_RIGHT] = (int)VK_RIGHT;        // there is no nice ascii equivalent for the arrow keys
    conversion_array[VK_DOWN] = (int)VK_DOWN;          // there is no nice ascii equivalent for the arrow keys
    conversion_array[VK_SNAPSHOT] = (int)VK_SNAPSHOT;  // there is no nice ascii equivalent for the PRTSC key
    conversion_array[VK_INSERT] = (int)VK_INSERT;      // there is no nice ascii equivalent for the INSERT key
    conversion_array[VK_DELETE] = (int)'\x7F';         // the delete key will correspond to the delete ascii character
    conversion_array[VK_LWIN] = (int)VK_LWIN;          // there is no ascii equivalent for the left windows key
    conversion_array[VK_LSHIFT] = (int)VK_LSHIFT;      // there is no ascii equivalent for left shift
    conversion_array[VK_RSHIFT] = (int)VK_RSHIFT;      // there is no ascii equivalent for right shift
    conversion_array[VK_LCONTROL] = (int)VK_LCONTROL;  // there is no ascii equivalent for left control
    conversion_array[VK_RCONTROL] = (int)VK_RCONTROL;  // there is no ascii equivalent for right control
    conversion_array[VK_RMENU] = (int)VK_RMENU;        // there is no ascii equivalent for right alt
    conversion_array[VK_LMENU] = (int)VK_LMENU;        // there is no ascii equivalent for left alt
    conversion_array[VK_OEM_1] = (int)';';             // VK_OEM_1 key corresponds to ;
    conversion_array[VK_OEM_2] = (int)'/';             // VK_OEM_2 key corresponds to /
    conversion_array[VK_OEM_3] = (int)'`';             // VK_OEM_3 key corresponds to `
    conversion_array[VK_OEM_4] = (int)'[';             // VK_OEM_4 key corresponds to [
    conversion_array[VK_OEM_5] = (int)'\\';            /*VK_OEM_5 key correpsonds to \ */
    conversion_array[VK_OEM_6] = (int)']';             // VK_OEM_6 key corresponds to ]
    conversion_array[VK_OEM_7] = (int)'\'';            // VK_OEM_7 key corresponds to '
    conversion_array[VK_OEM_PLUS] = (int)'=';          // the + key happens to be where = is also, but = is what shows up when shift is not pressed
    conversion_array[VK_OEM_COMMA] = (int)',';         // the , key corresponds to ,
    conversion_array[VK_OEM_MINUS] = (int)'-';         // the - key corresponds to -
    conversion_array[VK_OEM_PERIOD] = (int)'.';        // the . key corresponds to .

    // key tracker is used to specify whether or not the key has been pressed since the last check
    bool key_tracker[256];
    // initialize everything to false
    memset(key_tracker, false, 256 * sizeof(bool));

    while (inputs->exit == false) {
        for (int i = 0; i < vk_check_size; i++) {
            // INT_LM_BIT has its l
            // most significant bit set means key is down in GetAsyncKeyState
            if (key_tracker[i] == false && GetAsyncKeyState(vk_check[i]) & (int)INT_LM_BIT) {
                // note that keypress has been recorded
                key_tracker[i] = true;

                // the bitwise or with INT_LM_BIT is to set the left bit equal to 1 to indicate the key is down
                inputs->key_interrupt = conversion_array[vk_check[i]] | INT_LM_BIT;
            } else if (key_tracker[i] == true && !(GetAsyncKeyState(vk_check[i]) & (int)INT_LM_BIT)) {
                // note that key-up has been recorded
                key_tracker[i] = false;

                // No bitwise or is needed here since a 0 in the leftmost bit would be a negative value
                inputs->key_interrupt = conversion_array[vk_check[i]];
            }
        }
    }

    return 0;
}