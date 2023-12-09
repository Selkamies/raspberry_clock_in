/**
 * @file database_config.h
 * @author Selkamies
 * 
 * @brief Holds important database values. Depending on file contents, 
 * this file should not be exposed to GitHub normally, but this is an exercise, so it will be included.

 * @date Created  2023-12-08
 * @date Modified 2023-12-09
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef DATABASE_CONFIG_H
#define DATABASE_CONFIG_H



////////////////
// USER TABLE //
////////////////

#define TABLE_USER "user"
#define COLUMN_ID_USER "id"
// TODO: What is this?
//#define COLUMN_RFID_ID_USER "RFID_ID"
#define COLUMN_FIRST_NAME_USER "first_name"
#define COLUMN_LAST_NAME_USER "last_name"
#define COLUMN_PIN_USER "pin"

// Index of the ID column in user table. Needed by sqlite3_column_int() in the callback function.
#define COLUMN_INDEX_ID_USER 0

// SQLite notes:
// Adding AUTOINCREMENT to PRIMARY KEY is not recommended unless you explicitly need it.
// The default functionality without AUTOINCREMENT is similar but not quite the same.
// Adding UNIQUE to PRIMARY KEY is also redundant.
// For INTEGER PRIMARY KEYs NOT NULL is also not needed.
// Adding STRICT at the end causes the table to be more strict about data types.
// NOTE: There CANNOT be a space after '\'.

#define CREATE_TABLE_USER \
    "CREATE TABLE IF NOT EXISTS " TABLE_USER " (" \
            COLUMN_ID_USER " INTEGER PRIMARY KEY, " \
            COLUMN_FIRST_NAME_USER " TEXT NOT NULL, " \
            COLUMN_LAST_NAME_USER " TEXT NOT NULL, " \
            COLUMN_PIN_USER " TEXT NOT NULL) STRICT;"

// ? is used by SQLite to bind values to.
#define INSERT_USER_ROW \
    "INSERT INTO " TABLE_USER \
        " (" COLUMN_FIRST_NAME_USER ", " COLUMN_LAST_NAME_USER ", " COLUMN_PIN_USER ") " \
    "VALUES (?, ?, ?);"

#define SELECT_USER_ID_BY_PIN \
    "SELECT " COLUMN_ID_USER \
    " FROM " TABLE_USER \
    " WHERE " COLUMN_PIN_USER " = ?;"

#define INSERT_INTO_USER_TEST_ROWS \
    "INSERT INTO " TABLE_USER \
        " (" COLUMN_FIRST_NAME_USER ", " COLUMN_LAST_NAME_USER ", " COLUMN_PIN_USER ") " \
    "VALUES " \
        "('first name', 'last name', '123A')," \
        "('John', 'Doe', 'ABCD')," \
        "('Jane', 'Doe', '123A');"



///////////////
// LOG TABLE //
///////////////

#define TABLE_LOG "log"
#define COLUMN_ID_LOG "id"
#define COLUMN_USER_ID_LOG "user_id"
#define COLUMN_DATE_LOG "date"
#define COLUMN_TIME_LOG "time"

#define CREATE_TABLE_LOG \
    "CREATE TABLE IF NOT EXISTS " TABLE_LOG " (" \
        COLUMN_ID_LOG " INTEGER PRIMARY KEY, " \
        COLUMN_USER_ID_LOG " INTEGER NOT NULL, " \
        COLUMN_DATE_LOG " DATE DEFAULT CURRENT_DATE NOT NULL, " \
        COLUMN_TIME_LOG " TIME DEFAULT CURRENT_TIME NOT NULL, " \
        "FOREIGN KEY (" COLUMN_USER_ID_LOG ") " \
            "REFERENCES " TABLE_USER " (" COLUMN_ID_USER ")) STRICT;"

// CURRENT_DATE and CURRENT_TIME are SQLite clauses. Date will be TEXT in "yyyy-mm-dd" format
// and time will be TEXT in "hh:mm:ss" format.
#define INSERT_LOG_ROW \
    "INSERT INTO " TABLE_LOG \
        " (" COLUMN_USER_ID_LOG ", " COLUMN_DATE_LOG ", " COLUMN_TIME_LOG ") " \
    "VALUES (?, CURRENT_DATE, CURRENT_TIME);"

//#define COLUMN_LOG_TIME_LOG "LOG_TIME"
// TODO: What is this?
//#define COLUMN_STATUS_LOG "STATUS"

//#define INSERT_LOG_ROW "INSERT INTO " TABLE_LOG " (" COLUMN_USER_ID_LOG ", " COLUMN_STATUS_LOG ") VALUES (?, ?);"



#endif // DATABASE_CONFIG_H