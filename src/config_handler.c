/**
 * @file config_handler.c
 * @author Selkamies
 * 
 * @brief Reads values from config.ini and sets them in a config struct in config.h.
 * 
 * @date Created 2023-11-14
 * @date Modified 2023-11-17
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: Read keypad pin numbers from file.
 * TODO: Read keypad keys from file.
 * TODO: Any way to reduce hardcoded strings? 
 *       " %100[^=]=%100[^\n]" should replace 100 with MAX_LINE_LENGTH.
 *       setConfigValue uses hardcoded comparisons. Any way to just compare the key and struct members?
 * TODO: Look for config.ini in a few places. First in the same folder, then under config folder?
 */



#include <stdio.h>
#include <ctype.h>      // isspace().
#include <stdlib.h>     // atoi() for converting string to int.
#include <string.h>     // strcmp()

#include "config_handler.h"

// These are the files that we read values to.
#include "config.h"



// TODO: Keep ALL values read from config.ini here and create this when we open the file. 
// Then pass all the values to the structs in the relevant files, and this will be destroyed when
// we exit the function reading the file.
//struct ConfigData configData;



const char *fileName = "../config/config.ini";



void readKeypadData(struct ConfigData *configData, char *key, char *value)
{
    //////////////
    // [KEYPAD] //
    //////////////

    if (strcmp(key, "MAX_PIN_LENGTH") == 0)
    {
        config.MAX_PIN_LENGTH = atoi(value);
        configData->MAX_PIN_LENGTH = atoi(value);
    }

    else if (strcmp(key, "KEYPRESS_TIMEOUT") == 0)
    {
        config.KEYPRESS_TIMEOUT = atoi(value);
        configData->MAX_PIN_LENGTH = atoi(value);
    }

    ///////////////////
    // [KEYPAD_KEYS] //
    ///////////////////

    else if (strcmp(key, "KEYPAD_ROWS") == 0)
    {
        config.KEYPAD_ROWS = atoi(value);
        configData->KEYPAD_ROWS = atoi(value);
        configData->keypad_row_pins = calloc(configData->KEYPAD_ROWS, sizeof(int));
    }

    else if (strcmp(key, "KEYPAD_COLUMNS") == 0)
    {
        config.KEYPAD_COLUMNS = atoi(value);
        configData->KEYPAD_COLUMNS = atoi(value);
        configData->keypad_column_pins = calloc(configData->KEYPAD_COLUMNS, sizeof(int));
    }

    // TODO: These need two-dimensional malloc-initialized array.
    /* KEYPAD_ROW_0_COLUMN_0_KEY = 1
    KEYPAD_ROW_0_COLUMN_1_KEY = 2
    KEYPAD_ROW_0_COLUMN_2_KEY = 3
    KEYPAD_ROW_0_COLUMN_3_KEY = A

    KEYPAD_ROW_1_COLUMN_0_KEY = 4
    KEYPAD_ROW_1_COLUMN_1_KEY = 5
    KEYPAD_ROW_1_COLUMN_2_KEY = 6
    KEYPAD_ROW_1_COLUMN_3_KEY = B

    KEYPAD_ROW_2_COLUMN_0_KEY = 7
    KEYPAD_ROW_2_COLUMN_1_KEY = 8
    KEYPAD_ROW_2_COLUMN_2_KEY = 9
    KEYPAD_ROW_2_COLUMN_3_KEY = C

    KEYPAD_ROW_3_COLUMN_0_KEY = *
    KEYPAD_ROW_3_COLUMN_1_KEY = 0
    KEYPAD_ROW_3_COLUMN_2_KEY = #
    KEYPAD_ROW_3_COLUMN_3_KEY = D */

    ///////////////////////////////
    // [KEYPAD_GPIO_PIN_NUMBERS] //
    ///////////////////////////////

    // TODO: These need two calloc-initialized rows.
    /* KEYPAD_ROW_0 = 11
    KEYPAD_ROW_1 = 9
    KEYPAD_ROW_2 = 10
    KEYPAD_ROW_3 = 22

    KEYPAD_COLUMN_0 = 17
    KEYPAD_COLUMN_1 = 4
    KEYPAD_COLUMN_2 = 3
    KEYPAD_COLUMN_3 = 2 */
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
}



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
        if (line[0] == '[' && sscanf(line, "[%99[^]]]", currentSection) == 1) {
            //handleSection(currentSection);
            continue;
        }

        //struct KeyValuePair keyValuePair;
        // Key name of the key-value pair. Example: MAX_PIN_LENGTH
        char key[MAX_KEY_LENGTH];
        // Value for the key as a string. Example: "4"
        char value[MAX_VALUE_LENGTH];

        // sscanf parses data from a string. 
        // It tries to find max length 100 of key part, =, and then max 100 length value part and a linebreak.
        // Then it assings the values to the keyValuePair.
        // TODO: Key gets an extra space at the end. Strip both key and value?
        if (sscanf(line, " %100[^=]=%100[^\n]", key, value) == 2) 
        {
            //printf("Key before stripping: '%s'.", key);
            stripWhitespace(key);
            //printf("Key after stripping: '%s'.", key);
            //printf("Value before stripping: '%s'.", value);
            sscanf(value, " %100[^\n]", value);
            //stripWhitespace(value);
            //printf(" Value after stripping: '%s'.\n", value);
            
            // Save the values to config struct.
            setConfigValue(&configData, currentSection, key, value);
        }
    }

    /* printf("MAX_PIN_LENGTH: '%d'.\n", config.MAX_PIN_LENGTH);
    printf("KEYPRESS_TIMEOUT: '%d'.\n", config.KEYPRESS_TIMEOUT);
    printf("KEYPAD_ROWS: '%d'.\n", config.KEYPAD_ROWS);
    printf("KEYPAD_COLUMNS: '%d'.\n", config.KEYPAD_COLUMNS); */

    setLedVariables(&configData.ledPins, configData.ledStaysOnFor);

    free(configData.keypad_row_pins);
    free(configData.keypad_column_pins);

    fclose(file);
}



/* char *stripStringLeading(char *string)
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
    string = stripStringLeading(string);

    return stripStringTrailing(string);
    //return rtrim(ltrim(string)); 
} */

void stripWhitespace(char *str) 
{
    // Strip leading whitespace.
    while (isspace((unsigned char)*str)) 
    {
        str++;
    }

    // TODO: String becomes empty, handle this somehow?
    if (*str == '\0') 
    {
        return;
    }

    // TODO: Doesn't work, stop doing this?
    // Strip trailing whitespace.
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) 
    {
        end--;
    }

    // Null-terminate the modified string after the last non-whitespace character.
    *(end + 1) = '\0';
}
