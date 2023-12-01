/**
 * @file config_handler.c
 * @author Selkamies
 * 
 * @brief Reads values from config.ini and sets them in a config struct in config.h.
 * 
 * @date Created 2023-11-14
 * @date Modified 2023-12-01
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: Read keypad keys from file.
 * TODO: Any way to reduce hardcoded strings? 
 *       " %100[^=]=%100[^\n]" should replace 100 with MAX_LINE_LENGTH.
 *       setConfigValue uses hardcoded comparisons. Any way to just compare the key and struct members?
 * TODO: Look for config.ini in a few places. First in the same folder, then under config folder?
 */



#include <stdio.h>
#include <ctype.h>      // isspace().
#include <stdlib.h>     // atoi() for converting string to int.
#include <string.h>     // strcmp(), strstr(), sscanf().

#include "config_handler.h"

#include "sounds.h"



const char *fileName = "../config/config.ini";



void readConfigFile()
{
    printf("Loading config.ini.\n");
    
    //char *fileName = "config.ini";
    char *fileName = "../config/config.ini";

    FILE *file = fopen(fileName, "r");
    if (!file) 
    {
        fprintf(stderr, "Error opening file: %s\n", fileName);
        return;
    }

    struct ConfigData configData;
    configData.keypadKeys = NULL;
    //configData.keypadState.keysPressedPreviously = NULL;

    char line[MAX_LINE_LENGTH];
    //char firstChar = line[0];
    char currentSection[MAX_KEY_LENGTH];

    while (fgets(line, sizeof(line), file)) 
    {
        // Skip comments and empty lines
        if (line[0] == ';' || line[0] == '#' || line[0] == '\n' || line[0] == '\r') 
        {
            continue;
        }

        // Check for section change. sscanf sets the currentSection.
        if (line[0] == '[' && sscanf(line, "[%99[^]]]", currentSection) == 1) 
        {
            continue;
        }

        // Key name of the key-value pair. Example: MAX_PIN_LENGTH
        char key[MAX_KEY_LENGTH];
        // Value for the key as a string. Example: "4"
        char value[MAX_VALUE_LENGTH];

        // sscanf parses data from a string. 
        // It tries to find max length 100 of key part, =, and then max 100 length value part and a linebreak.
        // Then it assings the values to the keyValuePair.
        if (sscanf(line, " %100[^=]=%100[^\n]", key, value) == 2) 
        {
            //sscanf(value, " %100[^\n]", value);

            // TODO: Stripping leading whitespace doesn't work without strcpy for some reason.
            strcpy(key, stripString(key));
            strcpy(value, stripString(value));
            //printf("After: '%s'\n", value);
            
            // Save the values to config struct.
            setConfigValue(&configData, currentSection, key, value);
        }
    }

    // Pass read variables to relevant files.
    //setKeypadValues(&configData.keypadConfig, &configData.keypadPins, &configData.keypadState);
    setKeypadValues(&configData.keypadConfig, &configData.keypadPins, configData.keypadKeys);
    setLedVariables(&configData.ledPins, configData.ledStaysOnFor);
    setSoundsConfig(configData.audioDeviceID);

    // TODO: We cannot free these here.
    //free(configData.keypadPins.keypad_rows);
    //free(configData.keypadPins.keypad_columns);

    fclose(file);
}



void setConfigValue(struct ConfigData *configData, char *section, char *key, char *value)
{
    //printf("Reading value from section '%s'.\n", section);

    if (strcmp(section, "KEYPAD_GPIO_PIN_NUMBERS") == 0 || 
        strcmp(section, "KEYPAD") == 0 ||
        strcmp(section, "KEYPAD_KEYS") == 0)
    {
        readKeypadData(configData, key, value);
    }

    else if (strcmp(section, "LED_GPIO_PIN_NUMBERS") == 0 ||
             strcmp(section, "LED") == 0)
    {
        readLedData(configData, key, value);
    }

    else if (strcmp(section, "SOUNDS") == 0)
    {
        readSoundData(configData, key, value);
    }
}

void readKeypadData(struct ConfigData *configData, char *key, char *value)
{
    //////////////
    // [KEYPAD] //
    //////////////

    if (strcmp(key, "MAX_PIN_LENGTH") == 0)
    {
        //config.MAX_PIN_LENGTH = atoi(value);
        configData->keypadConfig.MAX_PIN_LENGTH = atoi(value);
    }

    else if (strcmp(key, "KEYPRESS_TIMEOUT") == 0)
    {
        //config.KEYPRESS_TIMEOUT = atoi(value);
        configData->keypadConfig.KEYPRESS_TIMEOUT = atoi(value);
    }

    ///////////////////
    // [KEYPAD_KEYS] //
    ///////////////////

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

void readLedData(struct ConfigData *configData, char *key, char *value)
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

void readSoundData(struct ConfigData *configData, char *key, char *value)
{
    if (strcmp(key, "AUDIO_DEVICE_ID") == 0)
    {
        configData->audioDeviceID = atoi(value);
    }
}



char *stripStringLeading(char *string)
{
    // Move the starting point forward until we find a non-whitespace character.
    while(isspace(*string))
    {
        string++;
    }

    // If the string is now empty, return the original string.
    if (*string == 0)
    {
        return string;
    }
    
    return string;
}

char *stripStringTrailing(char *string)
{
    // Get a pointer to the last character of the string.
    char *back = string + strlen(string);

    // Loop backwards from the last character until we find a non-whitespace character.
    while(isspace(*--back));

    // Terminate the string at the new end.
    *(back + 1) = '\0';

    return string;
}

char *stripString(char *string)
{
    return stripStringTrailing(stripStringLeading(string));
}

