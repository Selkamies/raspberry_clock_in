/**
 * @file pins.h
 * @author Selkamies
 * 
 * @brief Handles the mapping of GPIO pins to pin numbers, and has functions for 
 * initializing and resetting their status.
 * 
 * @date Created 2023-11-13
 * @date Modified 2023-11-15
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef PINS_H
#define PINS_H



/**
 * @brief Struct holding the pin numbers for all Raspberry Pi 4 GPIO pins used by the program.
 * TODO: Read the pin numbers from file, so we don't have to recompile if we change pins.
 */
struct GPIOPins
{
    /** @brief GPIO pin numbers of the keypad rows. */
    int *keypad_rows;
    /** @brief GPIO pin numbers of the keypad columns. */
    int *keypad_columns;
};



/**
 * @brief Creates a struct with the GPIO pins used in the program and initialized their states.
 * 
 * @return struct GPIOPins 
 */
struct GPIOPins createGPIOPins();

/**
 * @brief Read the pin numbers for GPIO pins from a human-readable file.
 * TODO: Actually read the pin numbers from file.
 * 
 * @param gpio_pins Struct with the GPIO pin numbers.
 * @param filename
 * @return struct GPIOPins 
 */
struct GPIOPins readGPIOPinsFromFile(const char *filename);

/**
 * @brief Sets the initial status of the GPIO pins.
 * 
 * @param gpio_pins Struct with the GPIO pin numbers.
 */
void initializeGPIOPins(struct GPIOPins *gpio_pins);

/**
 * @brief Sets the GPIO pins back to default status.
 * 
 * @param gpio_pins Struct with the GPIO pin numbers.
 */
void cleanupGPIOPins(struct GPIOPins *gpio_pins) ;



/**
 * @brief Prints the status of GPIO pins.
 * 
 * @param gpio_pins Struct with the GPIO pin numbers.
 */
void printGPIOPinStatus(struct GPIOPins *gpio_pins);



#endif // PINS_H