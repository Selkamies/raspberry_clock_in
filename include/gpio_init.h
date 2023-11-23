/**
 * @file gpio_init.h
 * @author Selkamies
 * 
 * @brief Manages the pigpio library initialization. pigpio handles the GPIO pins of Raspberry Pi.
 * 
 * @date Created 2023-11-13
 * @date Modified 2023-11-23
 * 
 * @copyright Copyright (c) 2023
 */

#ifndef GPIO_INIT_H
#define GPIO_INIT_H



#include <signal.h>     // sig_atomic_t, signal() and SIGINT used by pigpio.
#include <stdbool.h>



/**
 * @brief Pigpio uses this to keep track if the pins are running?
 */
extern volatile sig_atomic_t signal_received;

/**
 * @brief 
 * 
 * @param signo 
 */
void signalHandler(int signo);

/**
 * @brief Initializes the pigpio connection, so that we can use the GPIO pins.
 * 
 * @return true If pigpio was initialized successfully.
 * @return false If pigpio was not initialized.
 */
bool initializeGPIOLibrary();

/**
 * @brief Uses pigpio's time_sleep() to sleep.
 * 
 * @param seconds Duration in seconds, how long to sleep.
 */
void sleepGPIOLibrary(double seconds);

/**
 * @brief Terminates pigpio gracefully.
 */
void cleanupGPIOLibrary();


#endif // GPIO_INIT_H