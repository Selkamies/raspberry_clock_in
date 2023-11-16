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



#ifndef LEDS_H
#define LEDS_H



#include <time.h>           // time_t.

// TODO: Use the struct with all the pins in pins.h?
struct LedGPIOPins 
{
    int LED_RED;
    int LED_GREEN;
    int LED_BLUE;
};

struct LedStatus
{
    // How many seconds the led stays on for.
    int ledStaysOnFor;
    // Whether any led is on.
    bool ledIsOn;
    // When led was turned on.
    time_t ledStartTime;
};



void updateLED();

void turnLedOn(bool red, bool green, bool blue);

void turnLedsOff();



#endif // LEDS_H