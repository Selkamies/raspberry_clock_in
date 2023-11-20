/**
 * @file leds.c
 * @author Selkamies
 * 
 * @brief Handles the RGB led attached to the Raspberry Pi 4.
 * 
 * @date Created 2023-11-16
 * @date Modified 2023-11-20
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: Move all pipgio commands to separate file?
 * 
 */



#include <stdbool.h>
#include <time.h>           // time_t and time().
#include <pigpio.h>

#include "leds.h"



struct LedGPIOPins ledPins;

struct LedStatus ledStatus;



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

void setLedVariables(struct LedGPIOPins *pins, int ledStaysOnFor)
{
    ledPins = *pins;
    ledStatus.ledStaysOnFor = ledStaysOnFor;
}

void initializeLeds()
{
    ledStatus.ledIsOn = false;
    ledStatus.ledStartTime = 0;
}