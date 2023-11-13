#ifndef GPIO_HANDLER_H
#define GPIO_HANDLER_H

#include <signal.h>     // sig_atomic_t

#include "pins.h"

/**
 * @brief Pigpio uses this to keep track if the pins are running?
 */
extern volatile sig_atomic_t signal_received;

/**
 * @brief This function does some pigpio stuff.
 * 
 * @param signo No idea.
 */
void signalHandler(int signo);

/**
 * @brief Initializes the pigpio connection.
 * 
 * @param gpio_pins Struct with the GPIO pin numbers.
 * @param mainLoop Function in main.c, that gets called if pigpio is initialized correctly.
 */
void initializeGPIO(struct GPIOPins *gpio_pins, void (*mainLoop)(struct GPIOPins *));



#endif // GPIO_HANDLER_H