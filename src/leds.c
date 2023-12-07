/**
 * @file leds.c
 * @author Selkamies
 * 
 * @brief Handles the RGB led attached to the Raspberry Pi 4.
 * 
 * @date Created 2023-11-16
 * @date Modified 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 */



#include <stdbool.h>

#include "leds.h"
#include "leds_config.h"        // struct LEDConfig.

#include "gpio_functions.h"     // turnGPIOPinOn(), turnGPIOPinOff().
#include "timer.h"              // getCurrentTimeInSeconds()




void updateLED(struct LEDConfig *LEDConfigData)
{
    if (LEDConfigData->LEDCurrentStatus.LEDIsOn)
    {
        double LEDHasBeenOnFor = getCurrentTimeInSeconds() - LEDConfigData->LEDCurrentStatus.LEDStartTime;

        if (LEDHasBeenOnFor >= LEDConfigData->LEDCurrentStatus.LEDStaysOnFor)
        {
            turnLEDsOff(LEDConfigData);
        }
    }
}

void turnLEDOn(struct LEDConfig *LEDConfigData, const bool red, const bool green, const bool blue)
{
    turnLEDsOff(LEDConfigData);

    if (red)
    {
        turnGPIOPinOn(LEDConfigData->pins.LED_RED);
    }

    if (green)
    {
        turnGPIOPinOn(LEDConfigData->pins.LED_GREEN);
    }

    if (blue)
    {
        turnGPIOPinOn(LEDConfigData->pins.LED_BLUE);
    }

    if (red || green ||blue)
    {
        LEDConfigData->LEDCurrentStatus.LEDIsOn = true;
        LEDConfigData->LEDCurrentStatus.LEDStartTime = getCurrentTimeInSeconds();
    }
}

void turnLEDsOff(struct LEDConfig *LEDConfigData)
{
    if (LEDConfigData->LEDCurrentStatus.LEDIsOn)
    {
        turnGPIOPinOff(LEDConfigData->pins.LED_RED);
        turnGPIOPinOff(LEDConfigData->pins.LED_GREEN);
        turnGPIOPinOff(LEDConfigData->pins.LED_BLUE);

        LEDConfigData->LEDCurrentStatus.LEDIsOn = false;
        LEDConfigData->LEDCurrentStatus.LEDStartTime = 0;
    }
}



void initializeLeds(struct LEDConfig *LEDConfigData)
{
    LEDConfigData->LEDCurrentStatus.LEDIsOn = false;
    LEDConfigData->LEDCurrentStatus.LEDStartTime = 0;
}

void cleanupLEDs(struct LEDConfig *LEDConfigData)
{
    turnLEDsOff(LEDConfigData);
}