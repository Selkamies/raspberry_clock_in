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



#ifndef LEDS_H
#define LEDS_H



#include <stdbool.h>



// Forward declaration.
struct LEDConfig;



/**
 * @brief Update led status. If led is on, check if enough time has passed to turn them off.
 * 
 * @param LEDConfigData Struct holding all the variables needed by leds.c.
 */
void updateLED(struct LEDConfig *LEDConfigData);

/**
 * @brief Turns the RGB led on, by setting the three primary colors on or off.
 * 
 * @param LEDConfigData Struct holding all the variables needed by leds.c.
 * @param red Whether to use red light or not.
 * @param green Whether to use green light or not.
 * @param blue Whether to use blue light or not.
 */
void turnLEDOn(struct LEDConfig *LEDConfigData, const bool red, const bool green, const bool blue);

/**
 * @brief Turns the RGB led off.
 * 
 * @param LEDConfigData Struct holding all the variables needed by leds.c.
 */
void turnLEDsOff(struct LEDConfig *LEDConfigData);



/**
 * @brief Initialize some variables to their default states.
 * @param LEDConfigData Struct holding all the variables needed by leds.c.
 */
void initializeLeds(struct LEDConfig *LEDConfigData);



#endif // LEDS_H