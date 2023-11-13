#ifndef KEYPAD_H
#define KEYPAD_H



#include <stdbool.h>
#include <time.h>



#define MAX_PIN_LENGTH 4
#define KEYPRESS_TIMEOUT 10

#define KEYPAD_ROWS 4
#define KEYPAD_COLUMNS 4

#define EMPTY_KEY '\0'
#define EMPTY_TIMESTAMP 0



struct CurrentPinCode
{
    int nextPressIndex;
    time_t lastKeyPressTime;
    char keyPresses[MAX_PIN_LENGTH];
};

struct KeyPad
{
    char keys[KEYPAD_ROWS][KEYPAD_COLUMNS];
    bool keysPressedPreviously[KEYPAD_ROWS][KEYPAD_COLUMNS];
};



#endif // KEYPAD_H