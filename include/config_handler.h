/**
 * @file config_handler.h
 * @author Selkamies
 * 
 * @brief Reads values from config.ini and sets them in a config struct in config.h.
 * 
 * @date Created 2023-11-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */



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
 * @param key Key name of the key-value pair. Example: MAX_PIN_LENGTH
 * @param value Value for the key as a string. Example: "4"
 */
void setConfigValue(char *key, char *value);

/**
 * @brief Reads key-value pairs from config.ini and calls setConfigValue to set them.
 */
void readConfigFile();