/**
 * @file config_handler.c
 * @author Selkamies
 * 
 * @brief Reads key-value pairs from config.ini and passes relevant values to other files.
 * 
 * @date Created 2023-11-14
 * @date Modified 2023-12-04
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: Look for config.ini in a few places. First in the same folder, then under config folder?
 */



#include <stdio.h>      // printf(), snprintf().
#include <stdlib.h>     // atoi(), strtod().
#include <string.h>     // strcmp(), strstr(), sscanf().

#include "config_handler.h"

// These are the files that we pass the values read from config.ini to.
#include "keypad.h"
#include "leds.h"
#include "sounds.h"



/** @brief Maximum length of the key like MAX_PIN_LENGTH. */
#define MAX_KEY_LENGTH 50
/** @brief Maximum length of the value like "10". */
#define MAX_VALUE_LENGTH 50
/** @brief Maximum length of the line with key and value like MAX_PIN_LENGTH = 4 */
#define MAX_LINE_LENGTH 150



#pragma region Globals

/**
 * @brief Struct holding all values read from config.ini. Relevant values are passed to files
 * that need them, and this struct is discarded after we have done passing them.
 * 
 */
struct ConfigData
{
    //////////////
    // keypad.c //
    //////////////

    struct KeypadConfig keypadConfig;
    struct KeypadGPIOPins keypadPins;
    // TODO: Two-dimensional malloc required.
    //struct Keypad keypadState;
    char **keypadKeys;
    
    ////////////
    // leds.c //
    ////////////
    
    struct LedGPIOPins ledPins;
    int ledStaysOnFor;

    //////////////
    // sounds.c //
    //////////////

    int audioDeviceID;
};

/**
 * @brief Stores a key-value pair like MAX_PIN_LENGTH and "4". Read from config.ini.
 */
//struct KeyValuePair
//{
    /** @brief Key name of the key-value pair. Example: MAX_PIN_LENGTH */
    //char key[MAX_KEY_LENGTH];
    /** @brief Value for the key as a string. Example: "4" */
    //char value[MAX_VALUE_LENGTH];
//};

const char *fileName = "../config/config.ini";

#pragma endregion



#pragma region FunctionDeclarations

/**
 * @brief Loops through all the lines in the file (config.ini), reads the key-value pairs
 * and saves the values to a configData struct.
 * 
 * @param file File that we're reading. config.ini.
 * @param configData Struct we're temporarily saving the values we're reading to.
 */
static void readLines(FILE *file, struct ConfigData *configData);

/**
 * @brief Checks if the line is either empty or a comment line.
 * 
 * @param line Raw line from config.ini.
 * @return true If the line is skippable.
 * @return false If the line is either section name line or key-value line.
 */
static bool isSkippableLine(const char *line);

/**
 * @brief Checks if the line is a section name line, and reads the name of the new section.
 * 
 * @param line Raw line from config.ini.
 * @param sectionFormatString Formatting string used by sscanf() to parse section name from between brackets in a line.
 * @param currentSection Name of the current section. Gets changed to the name of the new section.
 * @return true If the line is a section change line.
 * @return false If the line is not a section change line.
 */
static bool isSectionChangeLine(const char *line, const char *sectionFormatString, char *currentSection);

/**
 * @brief Reads the value read from config.ini to the correct place in configData struct. 
 * Forwards the relevat portions of configData to other files.
 * 
 * @param configData Struct holding all the config values that are read from config.ini.
 * @param section Name of the section the key-value pair is under.
 * @param key Key name of the key-value pair. Example: MAX_PIN_LENGTH
 * @param value Value for the key as a string. Example: "4"
 */
static void setConfigValue(struct ConfigData *configData, const char *section, const char *key, const char *value);

/**
 * @brief Reads the keypad config values read from config.ini to configData struct.
 * 
 * @param configData Struct holding all the config values that are read from config.ini.
 * @param key Key name of the key-value pair. Example: MAX_PIN_LENGTH
 * @param value Value for the key as a string. Example: "4"
 */
static void readKeypadData(struct ConfigData *configData, const char *key, const char *value);

/**
 * @brief Reads the LED config values read from config.ini to configData struct.
 * 
 * @param configData Struct holding all the config values that are read from config.ini.
 * @param key Key name of the key-value pair. Example: MAX_PIN_LENGTH
 * @param value Value for the key as a string. Example: "4"
 */
static void readLEDData(struct ConfigData *configData, const char *key, const char *value);

/**
 * @brief Reads the sound config values read from config.ini to configData struct.
 * 
 * @param configData Struct holding all the config values that are read from config.ini.
 * @param key Key name of the key-value pair. Example: MAX_PIN_LENGTH
 * @param value Value for the key as a string. Example: "4"
 */
static void readSoundData(struct ConfigData *configData, const char *key, const char *value);

#pragma endregion



void readConfigFile()
{
    printf("Reading config.ini.\n");

    FILE *file = fopen(fileName, "r");
    if (!file) 
    {
        fprintf(stderr, "Error opening file: %s\n", fileName);
        return;
    }

    // Struct for temporarily holding data read from config.ini. Destroyed after file reading ends.
    struct ConfigData configData;

    readLines(file, &configData);

    // Pass read variables to relevant files.
    setKeypadValues(&configData.keypadConfig, &configData.keypadPins, configData.keypadKeys);
    setLedVariables(&configData.ledPins, configData.ledStaysOnFor);
    setSoundsConfig(configData.audioDeviceID);

    fclose(file);
}

static void readLines(FILE *file, struct ConfigData *configData)
{
    // Raw line read from config.ini.
    char line[MAX_LINE_LENGTH];
    // Name of the current section in config.ini.
    char currentSection[MAX_KEY_LENGTH];
    // Formatting string used by sscanf() to parse section name from between brackets in a line.
    char sectionFormatString[MAX_LINE_LENGTH];
    snprintf(sectionFormatString, sizeof(sectionFormatString), " [%%%d[^]]]", MAX_KEY_LENGTH - 1);
    // Formatting string used by sscanf() to parse keys and values from a line.
    char keyValueFormatString[MAX_LINE_LENGTH];
    // Reads key until first whitespace or equals sign, then reads the value from after equals sign adn whitespaces.
    snprintf(keyValueFormatString, sizeof(keyValueFormatString), " %%%d[^= ] = %%%d[^\n]", MAX_KEY_LENGTH - 1, MAX_VALUE_LENGTH - 1);

    while (fgets(line, sizeof(line), file)) 
    {
        if (isSkippableLine(line))
        {
            continue;
        }

        if (isSectionChangeLine(line, sectionFormatString, currentSection))
        {
            continue;
        }

        // Key name of the key-value pair. Example: MAX_PIN_LENGTH
        char key[MAX_KEY_LENGTH];
        // Value for the key as a string. Example: "4"
        char value[MAX_VALUE_LENGTH];

        // sscanf parses data from a string, here it reads KEY and VALUE from KEY = VALUE.
        // Then it assings the values to the keyValuePair.
        if (sscanf(line, keyValueFormatString, key, value) == 2) 
        {
            printf("Key '%s', Value: '%s'\n", key, value);
            
            // Save the values to config struct.
            setConfigValue(configData, currentSection, key, value);
        }
    }
}

static bool isSkippableLine(const char *line) 
{
    if (line[0] == ';' || line[0] == '#' || line[0] == '\n' || line[0] == '\r') 
    {
        return true; 
    }

    return false; 
}

static bool isSectionChangeLine(const char *line, const char *sectionFormatString, char *currentSection) 
{
    if (line[0] == '[' && sscanf(line, sectionFormatString, currentSection) == 1) 
    {
        printf("Section: '%s'\n", currentSection);
        return true; 
    }

    return false;
}



static void setConfigValue(struct ConfigData *configData, const char *section, const char *key, const char *value)
{
    if (strcmp(section, "KEYPAD_GPIO_PIN_NUMBERS") == 0 || 
        strcmp(section, "KEYPAD") == 0 ||
        strcmp(section, "KEYPAD_KEYS") == 0)
    {
        readKeypadData(configData, key, value);
    }

    else if (strcmp(section, "LED_GPIO_PIN_NUMBERS") == 0 ||
             strcmp(section, "LED") == 0)
    {
        readLEDData(configData, key, value);
    }

    else if (strcmp(section, "SOUNDS") == 0)
    {
        readSoundData(configData, key, value);
    }
}

static void readKeypadData(struct ConfigData *configData, const char *key, const char *value)
{
    //////////////
    // [KEYPAD] //
    //////////////

    if (strcmp(key, "MAX_PIN_LENGTH") == 0)
    {
        configData->keypadConfig.MAX_PIN_LENGTH = atoi(value);
    }

    else if (strcmp(key, "KEYPRESS_TIMEOUT") == 0)
    {
        configData->keypadConfig.KEYPRESS_TIMEOUT = atoi(value);
    }

    else if (strcmp(key, "KEYPAD_ROWS") == 0)
    {
        configData->keypadConfig.KEYPAD_ROWS = atoi(value);
        configData->keypadPins.keypad_rows = calloc(configData->keypadConfig.KEYPAD_ROWS, sizeof(int));
    }

    else if (strcmp(key, "KEYPAD_COLUMNS") == 0)
    {
        configData->keypadConfig.KEYPAD_COLUMNS = atoi(value);
        configData->keypadPins.keypad_columns = calloc(configData->keypadConfig.KEYPAD_COLUMNS, sizeof(int));
    }

    else if (strcmp(key, "UPDATE_INTERVAL_SECONDS") == 0)
    {
        configData->keypadConfig.UPDATE_INTERVAL_SECONDS = strtod(value, NULL);
    }

    ///////////////////
    // [KEYPAD_KEYS] //
    ///////////////////
    
    else if (strstr(key, "KEY_KEYPAD_ROW_"))
    {
        int rowIndex, columnIndex;

        if (sscanf(key, "KEY_KEYPAD_ROW_%d_COLUMN_%d", &rowIndex, &columnIndex) == 2)
        {
            // Dynamically allocate memory for keys array if not already allocated.
            if (configData->keypadKeys == NULL)
            {
                configData->keypadKeys = malloc(configData->keypadConfig.KEYPAD_ROWS * sizeof(char *));

                for (int row = 0; row < configData->keypadConfig.KEYPAD_ROWS; row++)
                {
                    configData->keypadKeys[row] = malloc(configData->keypadConfig.KEYPAD_COLUMNS * sizeof(char));
                }
            }

            // Store the key in the dynamically allocated keys array.
            configData->keypadKeys[rowIndex][columnIndex] = value[0];
        }
    }

    ///////////////////////////////
    // [KEYPAD_GPIO_PIN_NUMBERS] //
    ///////////////////////////////

    // strstr() looks for "KEYPAD_ROW_" from "KEYPAD_ROW_0" etc.
    else if (strstr(key, "KEYPAD_ROW_") == key)
    {
        // Dynamically determine the row index from the key.
        int rowIndex = atoi(key + strlen("KEYPAD_ROW_"));
        configData->keypadPins.keypad_rows[rowIndex] = atoi(value);
    }

    else if (strstr(key, "KEYPAD_COLUMN_") == key)
    {
        // Dynamically determine the column index from the key.
        int columnIndex = atoi(key + strlen("KEYPAD_COLUMN_"));
        configData->keypadPins.keypad_columns[columnIndex] = atoi(value);
    }
}

static void readLEDData(struct ConfigData *configData, const char *key, const char *value)
{
    ////////////////////////////
    // [LED_GPIO_PIN_NUMBERS] //
    ////////////////////////////

    if (strcmp(key, "LED_RED") == 0)
    {
        configData->ledPins.LED_RED = atoi(value);
    }

    else if (strcmp(key, "LED_GREEN") == 0)
    {
        configData->ledPins.LED_GREEN = atoi(value);
    }

    else if (strcmp(key, "LED_BLUE") == 0)
    {
        configData->ledPins.LED_BLUE = atoi(value);
    }

    ///////////
    // [LED] //
    ///////////

    else if (strcmp(key, "LED_STAYS_ON_FOR") == 0)
    {
        configData->ledStaysOnFor = atoi(value);
    }
}

static void readSoundData(struct ConfigData *configData, const char *key, const char *value)
{
    if (strcmp(key, "AUDIO_DEVICE_ID") == 0)
    {
        configData->audioDeviceID = atoi(value);
    }
}
