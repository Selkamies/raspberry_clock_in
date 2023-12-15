/**
 * @file database_sql.h
 * @author Selkamies
 * 
 * @brief Holds #defines with SQL variables like table and column names and SQL statements.

 * @date Created  2023-12-08
 * @date Modified 2023-12-15
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef DATABASE_CONFIG_H
#define DATABASE_CONFIG_H



#define SELECT_TABLE_EXISTS "SELECT name FROM sqlite_master WHERE type = 'table' AND name = ?;"



////////////////
// USER TABLE //
////////////////

#define TABLE_USER "user"
#define COLUMN_ID_USER "id"
#define COLUMN_FIRST_NAME_USER "first_name"
#define COLUMN_LAST_NAME_USER "last_name"
#define COLUMN_PIN_USER "pin"

// SQLite notes:
// Adding AUTOINCREMENT to PRIMARY KEY is not recommended unless you explicitly need it.
// The default functionality without AUTOINCREMENT is similar but not quite the same.
// Adding UNIQUE to PRIMARY KEY is also redundant.
// For INTEGER PRIMARY KEYs NOT NULL is also not needed.
// Adding STRICT at the end causes the table to be more strict about data types.
// NOTE: There CANNOT be a space after '\'.
// ? is used by SQLite to bind values to.

#define CREATE_TABLE_USER \
    "CREATE TABLE IF NOT EXISTS " TABLE_USER " (" \
            COLUMN_ID_USER " INTEGER PRIMARY KEY, " \
            COLUMN_FIRST_NAME_USER " TEXT NOT NULL, " \
            COLUMN_LAST_NAME_USER " TEXT NOT NULL, " \
            COLUMN_PIN_USER " TEXT NOT NULL UNIQUE) STRICT;"

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
        "('Jane', 'Doe', '2580');"



///////////////
// LOG TABLE //
///////////////

#define TABLE_LOG "log"
#define COLUMN_ID_LOG "id"
#define COLUMN_USER_ID_LOG "user_id"
#define COLUMN_DATETIME_LOG "datetime"
#define COLUMN_STATUS_LOG "status"

// TODO: Is there a difference between (datetime('now')) and CURRENT_TIMESTAMP?
#define CREATE_TABLE_LOG \
    "CREATE TABLE IF NOT EXISTS " TABLE_LOG " (" \
        COLUMN_ID_LOG " INTEGER PRIMARY KEY, " \
        COLUMN_USER_ID_LOG " INTEGER NOT NULL, " \
        COLUMN_DATETIME_LOG " TEXT DEFAULT CURRENT_TIMESTAMP NOT NULL, " \
        COLUMN_STATUS_LOG " INTEGER NOT NULL, " \
        "FOREIGN KEY (" COLUMN_USER_ID_LOG ") " \
            "REFERENCES " TABLE_USER " (" COLUMN_ID_USER ")) STRICT;"

// We don't need to insert datetime, it's set automatically.
#define INSERT_LOG_ROW "INSERT INTO " TABLE_LOG " (" COLUMN_USER_ID_LOG ", " COLUMN_STATUS_LOG ") VALUES (?, ?);"

// SELECTs the status on the latest log row of the user.
#define SELECT_LOG_ROW_BY_USER_ID_LATEST \
    "SELECT " COLUMN_STATUS_LOG \
    " FROM " TABLE_LOG \
    " WHERE " COLUMN_USER_ID_LOG " = ?" \
    " ORDER BY " COLUMN_DATETIME_LOG " DESC LIMIT 1;"



#endif // DATABASE_CONFIG_H