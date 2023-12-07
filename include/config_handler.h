/**
 * @file config_handler.h
 * @author Selkamies
 * 
 * @brief Reads key-value pairs from config.ini and passes relevant values to other files.
 * 
 * @date Created 2023-11-15
 * @date Modified 2023-11-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H



#include "config_data.h"



/**
 * @brief Reads value from config.ini to a struct.
 * 
 * @param configData Struct holding basically all variables used by the program.
 */
void readConfigFile(struct ConfigData *configData);



#endif // CONFIG_HANDLER_H