/**
 * @file leds.c
 * @author Selkamies
 * 
 * @brief Handles the RGB led attached to the Raspberry Pi 4.
 * 
 * @date Created 2023-11-16
 * @date Modified 2023-11-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#include <stdbool.h>
#include <time.h>           // time_t and time().
#include <pigpio.h>

#include "leds.h"



// TODO: Read these from file.
struct LedGPIOPins ledPins =
{
    .LED_RED = 15,
    .LED_GREEN = 14,
    .LED_BLUE = 18
};

struct LedStatus ledStatus =
{
    .ledIsOn = false,
    .ledStaysOnFor = 3,
    .ledStartTime = 0
};



void updateLED()
{
    if (ledStatus.ledIsOn)
    {
        time_t currentTime = time(NULL);
        double ledHasBeenOnFor = difftime(currentTime, ledStatus.ledStartTime);

        if (ledHasBeenOnFor >= ledStatus.ledStaysOnFor)
        {
            turnLedsOff();
        }
    }
}

void turnLedOn(bool red, bool green, bool blue)
{
    turnLedsOff();

    if (red)
    {
        gpioWrite(ledPins.LED_RED, 1);
    }

    if (green)
    {
        gpioWrite(ledPins.LED_GREEN, 1);
    }

    if (blue)
    {
        gpioWrite(ledPins.LED_BLUE, 1);
    }

    if (red || green ||blue)
    {
        ledStatus.ledIsOn = true;
        ledStatus.ledStartTime = time(NULL);
    }
}

void turnLedsOff()
{
    if (ledStatus.ledIsOn)
    {
        gpioWrite(ledPins.LED_RED, 0);
        gpioWrite(ledPins.LED_GREEN, 0);
        gpioWrite(ledPins.LED_BLUE, 0);
        ledStatus.ledIsOn = false;
        ledStatus.ledStartTime = 0;
    }
}