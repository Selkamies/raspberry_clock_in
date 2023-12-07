/**
 * @file gpio_functions.h
 * @author Selkamies
 * 
 * @brief Handles all the GPIO pin operations required by keypad using pigpio.
 * 
 * @date Created 2023-11-13
 * @date Updated 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef GPIO_FUNCTIONS_H
#define GPIO_FUNCTIONS_H



// Forward declaration.
struct KeypadConfig;



/**
 * @brief Turns the GPIO pin on.
 * 
 * @param pinNumber The pin number for the GPIO pin.
 */
void turnGPIOPinOn(const int pinNumber);

/**
 * @brief Turns the GPIO pin off.
 * 
 * @param pinNumber The pin number for the GPIO pin.
 */
void turnGPIOPinOff(const int pinNumber);

/**
 * @brief Checks if the GPIO pin is on.
 * 
 * @param pinNumber GPIO pin number for pigpio.
 * @return true If gpioRead(pinNumber) = 0.
 * @return false If gpioRead(pinNumber) = 1.
 */
bool isGPIOPinOn(const int pinNumber);



/**
 * @brief Set the keypad GPIO pins to correct starting states.
 * 
 * @param keypadPins Struct with the GPIO pin numbers of keypad rows and columns.
 * @param config Struct holding keypad config info, like the number of rows and columns in the keypad.
 */
void initializeKeypadGPIOPins(struct KeypadConfig *config);

/**
 * @brief Sets the keypad GPIO pins back to the default states.
 * 
 * @param keypadPins Struct with the GPIO pin numbers of keypad rows and columns.
 * @param config Struct holding keypad config info, like the number of rows and columns in the keypad.
 */
void cleanupKeypadGPIOPins(struct KeypadConfig *config);




#endif // GPIO_FUNCTIONS_H