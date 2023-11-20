/**
 * @file config_handler.h
 * @author Selkamies
 * 
 * @brief Reads values from config.ini and sets them in a config struct in config.h.
 * 
 * @date Created 2023-11-15
 * @date Modified 2023-11-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H


#include "keypad.h"
#include "leds.h"

// Forward declarations.
/* struct KeypadGPIOPins;
struct LedGPIOPins; */



struct ConfigData
{
    //////////////
    // keypad.c //
    //////////////

    // TODO: Use the struct fot these, it needs to be renamted and moved from config to keypad.
    int MAX_PIN_LENGTH;
    int KEYPRESS_TIMEOUT;
    int KEYPAD_ROWS;
    int KEYPAD_COLUMNS;
    struct KeypadGPIOPins keypadPins;
    // TODO: Two-dimensional malloc required.
    //int **keypad_keys;
    
    ////////////
    // leds.c //
    ////////////
    
    struct LedGPIOPins ledPins;
    int ledStaysOnFor;
};



/** @brief Maximum length of the key like MAX_PIN_LENGTH. */
#define MAX_KEY_LENGTH 50
/** @brief Maximum length of the value like "10". */
#define MAX_VALUE_LENGTH 50
/** @brief Maximum length of the line with key and value like MAX_PIN_LENGTH = 4 */
#define MAX_LINE_LENGTH 100



/**
 * @brief Stores a key-value pair like MAX_PIN_LENGTH and "4". Read from config.ini.
 */
struct KeyValuePair
{
    /** @brief Key name of the key-value pair. Example: MAX_PIN_LENGTH */
    char key[MAX_KEY_LENGTH];
    /** @brief Value for the key as a string. Example: "4" */
    char value[MAX_VALUE_LENGTH];
};



/**
 * @brief Set the value read from config.ini to the correct place in config struct.
 * 
 * @param section Name of the section the key-value pair is under.
 * @param key Key name of the key-value pair. Example: MAX_PIN_LENGTH
 * @param value Value for the key as a string. Example: "4"
 */
void setConfigValue(struct ConfigData *configData, char *section, char *key, char *value);

void readKeypadData(struct ConfigData *configData, char *key, char *value);
void readLedData(struct ConfigData *configData, char *key, char *value);

/**
 * @brief Reads key-value pairs from config.ini and calls setConfigValue to set them.
 */
void readConfigFile();

char *stripString(char *string);
char *stripStringLeading(char *string);
char *stripStringTrailing(char *string);



#endif // CONFIG_HANDLER_H