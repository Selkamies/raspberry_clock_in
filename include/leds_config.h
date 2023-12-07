/**
 * @file leds_config.h
 * @author Selkamies
 * 
 * @brief Defines LEDConfig struct, which holds basically all data used by leds.c.
 * 
 * @date Created 2023-12-05
 * @date Modified 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef LEDS_CONFIG_H
#define LEDS_CONFIG_H



#include <stdbool.h>



/**
 * @brief Struct holding data of current LED status. Whether the led is on, 
 * when did it turn on and how long should it stay on.
 */
struct LEDStatus
{
    // Whether any led is on.
    bool LEDIsOn;
    // When led was turned on.
    double LEDStartTime;
    // How many seconds the led stays on for.
    int LEDStaysOnFor;
};

/**
 * @brief Holds the GPIO pin numbers of pins used by the RGB led.
 */
struct LEDGPIOPins 
{
    int LED_RED;
    int LED_GREEN;
    int LED_BLUE;
};



/**
 * @brief Struct holding all the variables needed by leds.c.
 * 
 */
struct LEDConfig
{
    /** @brief Struct holding data of current LED status */
    struct LEDStatus LEDCurrentStatus;
    /** @brief Holds the GPIO pin numbers of pins used by the RGB led. */
    struct LEDGPIOPins pins;
};



#endif // LEDS_CONFIG_H
