
#include <stdbool.h>
#include <pigpio.h>

#include "keypad_gpio.h"



void turnKeypadRowOn(int keypadRowPin)
{
    gpioWrite(keypadRowPin, 1);
}

void turnKeypadRowOff(int keypadRowPin)
{
    gpioWrite(keypadRowPin, 0);
}

bool isKeypadColumnOff(int keypadColumnPin)
{
    if (gpioRead(keypadColumnPin))
    {
        return false;
    }

    else
    {
        return true;
    }
}