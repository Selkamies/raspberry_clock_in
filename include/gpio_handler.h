#ifndef GPIO_HANDLER_H
#define GPIO_HANDLER_H



#include <signal.h>     // sig_atomic_t



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
 * @brief Initializes the pigpio connection.
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