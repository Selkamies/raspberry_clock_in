/**
 * @file config_handler.c
 * @author Selkamies
 * 
 * @brief Reads key-value pairs from config.ini and passes relevant values to other files.
 * 
 * @date Created 2023-11-14
 * @date Modified 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: Look for config.ini in a few places. First in the same folder, then under config folder?
 */



#include <stdio.h>      // printf(), snprintf().
#include <stdlib.h>     // atoi(), strtod().
#include <string.h>     // strcmp(), strstr(), sscanf().
#include <stdbool.h>

#include "config_handler.h"
//#include "config_data.h"



#pragma region Globals

/** @brief Maximum length of the key like MAX_PIN_LENGTH. */
#define MAX_KEY_LENGTH 50
/** @brief Maximum length of the value like "10". */
#define MAX_VALUE_LENGTH 50
/** @brief Maximum length of the line with key and value like MAX_PIN_LENGTH = 4 */
#define MAX_LINE_LENGTH 110

#define SECTION_KEYPAD "KEYPAD"
#define SECTION_KEYPAD_KEYS "KEYPAD_KEYS"
#define SECTION_KEYPAD_GPIO "KEYPAD_GPIO_PIN_NUMBERS"
#define SECTION_LED "LED"
#define SECTION_LED_GPIO "LED_GPIO_PIN_NUMBERS"
#define SECTION_SOUNDS "SOUNDS"

#define KEY_MAX_PIN_LENGTH "MAX_PIN_LENGTH"
#define KEY_KEYPRESS_TIMEOUT "KEYPRESS_TIMEOUT"
#define KEY_KEYPAD_ROWS "KEYPAD_ROWS"
#define KEY_KEYPAD_COLUMNS "KEYPAD_COLUMNS"
#define KEY_KEYPAD_UPDATE_INVERVAL "KEYPAD_UPDATE_INTERVAL_SECONDS"

#define KEY_PREFIX_KEY_KEYPAD_ROW_D_COLUMN_D "KEY_KEYPAD_ROW_"
#define KEY_KEYPAD_KEY_ROW_D_COLUMN_D "KEY_KEYPAD_ROW_%d_COLUMN_%d"
#define KEY_PREFIX_KEYPAD_ROW_D "KEYPAD_ROW_"
#define KEY_KEYPAD_ROW_D "KEYPAD_ROW_%d"
#define KEY_PREFIX_KEYPAD_COLUMN_D "KEYPAD_COLUMN_"
#define KEY_KEYPAD_COLUMN_D "KEYPAD_COLUMN_%d"

#define KEY_LED_STAYS_ON_FOR "LED_STAYS_ON_FOR"
#define KEY_LED_RED_GPIO "LED_RED"
#define KEY_LED_GREEN_GPIO "LED_GREEN"
#define KEY_LED_BLUE_GPIO "LED_BLUE"

#define KEY_AUDIO_DEVICE_ID "AUDIO_DEVICE_ID"



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



void readConfigFile(struct ConfigData *configData)
{
    configData->keypadConfig.keypadState.keys = NULL;

    printf("Reading config.ini.\n");

    FILE *file = fopen(fileName, "r");
    if (!file) 
    {
        fprintf(stderr, "Error opening file: %s\n", fileName);
        return;
    }

    readLines(file, configData);

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

    // Loop through the lines in config.ini.
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

        // sscanf parses data from a string (line), here it reads KEY and VALUE from KEY = VALUE.
        if (sscanf(line, keyValueFormatString, key, value) == 2) 
        {
            printf("Section: '%s', Key: '%s', Value: '%s'\n", currentSection, key, value);
            
            // Save the value to config struct.
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
        return true; 
    }

    return false;
}



static void setConfigValue(struct ConfigData *configData, const char *section, const char *key, const char *value)
{
    if (strcmp(section, SECTION_KEYPAD) == 0 ||
        strcmp(section, SECTION_KEYPAD_KEYS) == 0 ||
        strcmp(section, SECTION_KEYPAD_GPIO) == 0)
    {
        readKeypadData(configData, key, value);
    }

    else if (strcmp(section, SECTION_LED) == 0 || 
             strcmp(section, SECTION_LED_GPIO) == 0)
    {
        readLEDData(configData, key, value);
    }

    else if (strcmp(section, SECTION_SOUNDS) == 0)
    {
        readSoundData(configData, key, value);
    }
}

static void readKeypadData(struct ConfigData *configData, const char *key, const char *value)
{
    //////////////
    // [KEYPAD] //
    //////////////

    if (strcmp(key, KEY_MAX_PIN_LENGTH) == 0)
    {
        configData->keypadConfig.MAX_PIN_LENGTH = atoi(value);
    }

    else if (strcmp(key, KEY_KEYPRESS_TIMEOUT) == 0)
    {
        configData->keypadConfig.KEYPRESS_TIMEOUT = atoi(value);
    }

    else if (strcmp(key, KEY_KEYPAD_ROWS) == 0)
    {
        configData->keypadConfig.KEYPAD_ROWS = atoi(value);
        configData->keypadConfig.pins.keypad_rows = calloc(configData->keypadConfig.KEYPAD_ROWS, sizeof(int));
    }

    else if (strcmp(key, KEY_KEYPAD_COLUMNS) == 0)
    {
        configData->keypadConfig.KEYPAD_COLUMNS = atoi(value);
        configData->keypadConfig.pins.keypad_columns = calloc(configData->keypadConfig.KEYPAD_COLUMNS, sizeof(int));
    }

    else if (strcmp(key, KEY_KEYPAD_UPDATE_INVERVAL) == 0)
    {
        configData->keypadConfig.UPDATE_INTERVAL_SECONDS = strtod(value, NULL);
    }

    ///////////////////
    // [KEYPAD_KEYS] //
    ///////////////////
    
    // strstr(key, string) looks for key in the string. 
    // if (strstr(key, string)) == key returns true if the key is in the beginning of the string.
    // if (strstr(key, string)) returns true if the key is anywhere in the string.
    else if (strstr(key, KEY_PREFIX_KEY_KEYPAD_ROW_D_COLUMN_D) == key)
    {
        int rowIndex, columnIndex;

        // Gets the row and column indexes from the key.
        if (sscanf(key, KEY_KEYPAD_KEY_ROW_D_COLUMN_D, &rowIndex, &columnIndex) == 2)
        {
            // Dynamically allocate memory for keys array if not already allocated.
            if (configData->keypadConfig.keypadState.keys == NULL)
            {
                configData->keypadConfig.keypadState.keys = malloc(configData->keypadConfig.KEYPAD_ROWS * sizeof(char *));

                for (int row = 0; row < configData->keypadConfig.KEYPAD_ROWS; row++)
                {
                    configData->keypadConfig.keypadState.keys[row] = malloc(configData->keypadConfig.KEYPAD_COLUMNS * sizeof(char));
                }
            }

            // Store the key in the dynamically allocated keys array.
            configData->keypadConfig.keypadState.keys[rowIndex][columnIndex] = value[0];
        }
    }

    ///////////////////////////////
    // [KEYPAD_GPIO_PIN_NUMBERS] //
    ///////////////////////////////

    else if (strstr(key, KEY_PREFIX_KEYPAD_ROW_D) == key)
    {
        int rowIndex;

        // Gets the row index from the key.
        if (sscanf(key, KEY_KEYPAD_ROW_D, &rowIndex) == 1)
        {
            configData->keypadConfig.pins.keypad_rows[rowIndex] = atoi(value);
        }
    }

    else if (strstr(key, KEY_PREFIX_KEYPAD_COLUMN_D) == key)
    {
        int columnIndex;

        // Gets the column index from the key.
        if (sscanf(key, KEY_KEYPAD_COLUMN_D, &columnIndex) == 1)
        {
            configData->keypadConfig.pins.keypad_columns[columnIndex] = atoi(value);
        }
        
    }
}

static void readLEDData(struct ConfigData *configData, const char *key, const char *value)
{
    ///////////
    // [LED] //
    ///////////

    if (strcmp(key, KEY_LED_STAYS_ON_FOR) == 0)
    {
        //configData->LEDStaysOnFor = atoi(value);
        configData->LEDConfigData.LEDCurrentStatus.LEDStaysOnFor = atoi(value);
    }

    ////////////////////////////
    // [LED_GPIO_PIN_NUMBERS] //
    ////////////////////////////

    else if (strcmp(key, KEY_LED_RED_GPIO) == 0)
    {
        configData->LEDConfigData.pins.LED_RED = atoi(value);
    }

    else if (strcmp(key, KEY_LED_GREEN_GPIO) == 0)
    {
        configData->LEDConfigData.pins.LED_GREEN = atoi(value);
    }

    else if (strcmp(key, KEY_LED_BLUE_GPIO) == 0)
    {
        configData->LEDConfigData.pins.LED_BLUE = atoi(value);
    }
}

static void readSoundData(struct ConfigData *configData, const char *key, const char *value)
{
    if (strcmp(key, KEY_AUDIO_DEVICE_ID) == 0)
    {
        configData->soundsConfig.manualAudioDeviceID = atoi(value);
    }
}
