/**
 * @file leds_config.h
 * @author Selkamies
 * 
 * @brief 
 * 
 * @date Created 2023-12-05
 * @date Modified 2023-12-05
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef LEDS_CONFIG_H
#define LEDS_CONFIG_H



struct LEDStatus
{
    // Whether any led is on.
    bool LEDIsOn;
    // When led was turned on.
    double LEDStartTime;
    // How many seconds the led stays on for.
    int LEDStaysOnFor;
};

struct LEDGPIOPins 
{
    int LED_RED;
    int LED_GREEN;
    int LED_BLUE;
};

// TODO: Use this instead of global structs.
struct LEDConfig
{
    struct LEDGPIOPins pins;
    struct LEDStatus LEDCurrentStatus;
};



#endif // LEDS_CONFIG_H
