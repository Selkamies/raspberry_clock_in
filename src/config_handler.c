/**
 * @file config_handler.c
 * @author Selkamies
 * 
 * @brief Reads values from config.ini and sets them in a config struct in config.h.
 * 
 * @date Created 2023-11-14
 * 
 * @copyright Copyright (c) 2023
 * 
 * TODO: Use inih?
 * TODO: More config values, possibly separated to different structs.
 */


#include <stdio.h>
#include <stdlib.h>     // atoi() for converting string to int.
#include <string.h>     // strcmp()

#include "config.h"



#define MAX_KEY_LENGTH 50
#define MAX_VALUE_LENGTH 50
#define MAX_LINE_LENGTH 100



struct KeyValuePair
{
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
};



void setConfigValue(char key, char value)
{
    if (strcmp(key, "MAX_PIN_LENGTH") == 0)
    {
        config.MAX_PIN_LENGTH = atoi(value);
    }

    else if (strcmp(key, "KEYPRESS_TIMEOUT") == 0)
    {
        config.KEYPRESS_TIMEOUT = atoi(value);
    }

    else if (strcmp(key, "KEYPAD_ROWS") == 0)
    {
        config.KEYPAD_ROWS = atoi(value);
    }

    else if (strcmp(key, "KEYPAD_COLUMNS") == 0)
    {
        config.KEYPAD_COLUMNS = atoi(value);
    }
}

void readConfigFile()
{
    char *fileName = "config.ini";

    FILE *file = fopen(fileName, "r");
    if (!file) 
    {
        fprintf(stderr, "Error opening file: %s\n", fileName);
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) 
    {
        // Skip comments and empty lines
        if (line[0] == ';' || line[0] == '#' || line[0] == '\n' || line[0] == '\r') 
        {
            continue;
        }

        struct KeyValuePair keyValuePair;

        // sscanf parses data from a string. 
        // It tries to find max length 100 of key part, =, and then max 100 length value part and a linebreak.
        // Then it assings the values to the keyValuePair.
        if (sscanf(line, " %100[^=]=%100[^\n]", keyValuePair.key, keyValuePair.value) == 2) 
        {
            printf("Key: %s, Value: %s\n", keyValuePair.key, keyValuePair.value);
            
            // Save the values to config struct.
            setConfigValue(keyValuePair.key, keyValuePair.value);
        }
    }

    fclose(file);
}

