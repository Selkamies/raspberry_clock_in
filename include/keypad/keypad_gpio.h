#ifndef KEYPAD_GPIO_H
#define KEYPAD_GPIO_H



/**
 * @brief Turns the GPIO pin for the keypad row on.
 * 
 * @param keypadRowPin The pin number for the GPIO pin.
 */
void turnKeypadRowOn(int keypadRowPin);

/**
 * @brief Turns the GPIO pin for the keypad row off.
 * 
 * @param keypadRowPin The pin number for the GPIO pin.
 */
void turnKeypadRowOff(int keypadRowPin);

/**
 * @brief Checks if the GPIO pin for the keypad column is on.
 * 
 * @param keypadColumnPin The pin number for the GPIO pin.
 * @return true If the keypad column is on.
 * @return false If the keypad column is off.
 */
bool isKeypadColumnOn(int keypadColumnPin);



#endif // KEYPAD_GPIO_H