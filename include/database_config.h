/**
 * @file database_config.h
 * @author Selkamies
 * 
 * @brief Holds important database values. Depending on file contents, 
 * this file should not be exposed to GitHub normally, but this is an exercise, so it will be included.

 * @date Created  2023-12-08
 * @date Modified 2023-12-08
 * 
 * @copyright Copyright (c) 2023
 */

#ifndef DATABASE_CONFIG_H
#define DATABASE_CONFIG_H



#define TABLE_USER "user"
#define COLUMN_ID_USER "id"
// TODO: What is this?
//#define COLUMN_RFID_ID_USER "RFID_ID"
#define COLUMN_NAME_USER "name"
#define COLUMN_PIN_USER "pin"

// ? is used to bind values to.
#define SELECT_USER_ID_BY_PIN "SELECT " COLUMN_ID_USER \
                              " FROM " TABLE_USER \
                              " WHERE " COLUMN_PIN_USER " = ?;"

#define TABLE_LOG "log"
#define COLUMN_ID_LOG "id"
#define COLUMN_USER_ID_LOG "user_id"
#define COLUMN_DATE_LOG "date"
#define COLUMN_TIME_LOG "time"

#define CREATE_TABLE_LOG "CREATE TABLE " TABLE_LOG " (" \
                         COLUMN_ID_LOG " INTEGER PRIMARY KEY, " \
                         COLUMN_USER_ID_LOG " INTEGER, " \
                         COLUMN_DATE_LOG " DATE DEFAULT CURRENT_DATE, " \
                         COLUMN_TIME_LOG " TIME DEFAULT CURRENT_TIME);"

#define INSERT_LOG_ROW "INSERT INTO " TABLE_LOG \
                       " (" COLUMN_USER_ID_LOG ", " COLUMN_DATE_LOG ", " COLUMN_TIME_LOG ") " \
                       "VALUES (?, CURRENT_DATE, CURRENT_TIME);"

//#define COLUMN_LOG_TIME_LOG "LOG_TIME"
// TODO: What is this?
//#define COLUMN_STATUS_LOG "STATUS"

//#define INSERT_LOG_ROW "INSERT INTO " TABLE_LOG " (" COLUMN_USER_ID_LOG ", " COLUMN_STATUS_LOG ") VALUES (?, ?);"



#endif // DATABASE_CONFIG_H