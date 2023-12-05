/**
 * @file leds.c
 * @author Selkamies
 * 
 * @brief Handles the RGB led attached to the Raspberry Pi 4.
 * 
 * @date Created 2023-11-16
 * @date Modified 2023-12-05
 * 
 * @copyright Copyright (c) 2023
 */



#include <stdbool.h>

#include "leds.h"

#include "gpio_functions.h"     // turnGPIOPinOn(), turnGPIOPinOff().
#include "timer.h"              // getCurrentTimeInSeconds()



struct LEDStatus
{
    // Whether any led is on.
    bool LEDIsOn;
    // When led was turned on.
    double LEDStartTime;
    // How many seconds the led stays on for.
    int LEDStaysOnFor;
};

struct LEDGPIOPins LEDPins;
struct LEDStatus LEDCurrentStatus;



void updateLED()
{
    if (LEDCurrentStatus.LEDIsOn)
    {
        double LEDHasBeenOnFor = getCurrentTimeInSeconds() - LEDCurrentStatus.LEDStartTime;

        if (LEDHasBeenOnFor >= LEDCurrentStatus.LEDStaysOnFor)
        {
            turnLEDsOff();
        }
    }
}

void turnLEDOn(const bool red, const bool green, const bool blue)
{
    turnLEDsOff();

    if (red)
    {
        turnGPIOPinOn(LEDPins.LED_RED);
    }

    if (green)
    {
        turnGPIOPinOn(LEDPins.LED_GREEN);
    }

    if (blue)
    {
        turnGPIOPinOn(LEDPins.LED_BLUE);
    }

    if (red || green ||blue)
    {
        LEDCurrentStatus.LEDIsOn = true;
        LEDCurrentStatus.LEDStartTime = getCurrentTimeInSeconds();
    }
}

void turnLEDsOff()
{
    if (LEDCurrentStatus.LEDIsOn)
    {
        turnGPIOPinOff(LEDPins.LED_RED);
        turnGPIOPinOff(LEDPins.LED_GREEN);
        turnGPIOPinOff(LEDPins.LED_BLUE);
        LEDCurrentStatus.LEDIsOn = false;
        LEDCurrentStatus.LEDStartTime = 0;
    }
}

void setLEDVariables(const struct LEDGPIOPins *pins, const int LEDStaysOnFor)
{
    LEDPins = *pins;
    LEDCurrentStatus.LEDStaysOnFor = LEDStaysOnFor;
}

void initializeLeds()
{
    LEDCurrentStatus.LEDIsOn = false;
    LEDCurrentStatus.LEDStartTime = 0;
}