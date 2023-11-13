
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

bool isKeypadColumnOn(int keypadColumnPin)
{
    if (gpioRead(keypadColumnPin))
    {
        return true;
    }

    else
    {
        return false;
    }
}