/**
 * @file leds.c
 * @author Selkamies
 * 
 * @brief Handles the RGB led attached to the Raspberry Pi 4.
 * 
 * @date Created 2023-11-16
 * @date Modified 2023-11-23
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: Move all pipgio commands to separate file?
 * 
 */



#include <stdbool.h>
#include <time.h>               // time_t and time().

#include "leds.h"
#include "gpio_functions.h"



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
        turnPinOn(ledPins.LED_RED);
    }

    if (green)
    {
        turnPinOn(ledPins.LED_GREEN);
    }

    if (blue)
    {
        turnPinOn(ledPins.LED_BLUE);
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
        turnPinOff(ledPins.LED_RED);
        turnPinOff(ledPins.LED_GREEN);
        turnPinOff(ledPins.LED_BLUE);
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