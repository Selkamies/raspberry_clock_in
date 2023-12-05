/**
 * @file timer.c
 * @author Selkamies
 * 
 * @brief Handles getting the current time from system.
 * 
 * @date Created  2023-12-05
 * @date Modified 2023-12-05
 * 
 * @copyright Copyright (c) 2023
 */



#include <time.h>               // clock_gettime(), CLOCK_REALTIME.



double getCurrentTimeInSeconds()
{
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);

    // Adds up the amount of seconds (whole number) and amount of nanoseconds converted to seconds (double).
    return currentTime.tv_sec + (currentTime.tv_nsec / 1e9);
}