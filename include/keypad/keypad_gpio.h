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
 * @brief Checks if the keypad column is off, meaning that a key is pressed in the column 
 * if the corresponding keypad row is off.
 * TODO: The logic seems reversed to me, change name or fix otherwise?
 * 
 * @param keypadColumnPin GPIO pin number representing the keypad column.
 * @return true If gpioRead(keypadColumnPin) = 0.
 * @return false If gpioRead(keypadColumnPin) = 1.
 */
bool isKeypadColumnOff(int keypadColumnPin);



#endif // KEYPAD_GPIO_H