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
#include "leds_config.h"
#include "config_data.h"



//struct LEDGPIOPins pins;
//struct LEDStatus LEDCurrentStatus;



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

/* void setLEDVariables(const struct LEDGPIOPins *LEDPins, const int LEDStaysOnFor)
{
    pins = *LEDPins;
    LEDCurrentStatus.LEDStaysOnFor = LEDStaysOnFor;
} */

void initializeLeds(struct ConfigData *tempConfigData, struct LEDConfig *LEDConfigData)
{
    LEDConfigData->pins = tempConfigData->LEDConfigData.pins;
    LEDConfigData->LEDCurrentStatus.LEDStaysOnFor = tempConfigData->LEDConfigData.LEDCurrentStatus.LEDStaysOnFor;
    
    LEDConfigData->LEDCurrentStatus.LEDIsOn = false;
    LEDConfigData->LEDCurrentStatus.LEDStartTime = 0;
}