/**
 * @file gpio_handler.h
 * @author Selkamies
 * 
 * @brief Manages the pigpio library initialization. pigpio handles the GPIO pins of Raspberry Pi.
 * 
 * @date Created 2023-11-13
 * @date Modified 2023-11-16
 * 
 * @copyright Copyright (c) 2023
 */

#ifndef GPIO_HANDLER_H
#define GPIO_HANDLER_H



#include <signal.h>     // sig_atomic_t used by pigpio.



/**
 * @brief Pigpio uses this to keep track if the pins are running?
 */
extern volatile sig_atomic_t signal_received;

/**
 * @brief TODO: This function does some pigpio stuff.
 * 
 * @param signo TODO: No idea.
 */
void signalHandler(int signo);

/**
 * @brief Initializes the pigpio connection, so that we can use the GPIO pins.
 */
void initializePigpio();

/**
 * @brief Uses pigpio's time_sleep() to sleep.
 * 
 * @param seconds Duration in seconds, how long to sleep.
 */
void pigpioSleep(double seconds);

/**
 * @brief Terminates pigpio gracefully.
 */
void cleanupPigpio();


#endif // GPIO_HANDLER_H