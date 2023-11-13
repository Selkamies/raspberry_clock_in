#ifndef PINS_H
#define PINS_H



/**
 * @brief Maps the Raspberry Pi 4 GPIO pins.
 * TODO: Read these from a human-readable file, so that we don't need to recompile to change them.
 */
struct GPIOPins 
{
    int keypad_row_1;
    int keypad_row_2;
    int keypad_row_3;
    int keypad_row_4;
    int keypad_column_1;
    int keypad_column_2;
    int keypad_column_3;
    int keypad_column_4;
    // TODO: RGP led pin.
    // TODO: Buzzer pin?
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
 * @brief Prints all the GPIO pin numbers.
 * 
 * @param gpio_pins Struct with the GPIO pin numbers.
 */
void printGPIOPinNumbers(struct GPIOPins *gpio_pins);



#endif // PINS_H