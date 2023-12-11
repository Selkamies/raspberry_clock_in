/**
 * @file database.c
 * @author jaakho & Selkamies.
 * 
 * @brief Database operations.
 * 
 * @date Created  2023-12-08
 * @date Modified 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 */



#ifndef DATABASE_H
#define DATABASE_H



#include <stdbool.h>
#include <sqlite3.h>            // sqlite3.



#define DATABASE_PATH "database/"
#define DATABASE_NAME "database.db"
#define DATABASE_FILEPATH DATABASE_PATH DATABASE_NAME



/**
 * @brief Checks if the database exists, and if not, creates a new one with tables.
 * 
 * @param database Database we're using.
 * @param filePath Path with the database file name, relative to the executable location.
 * 
 * @return true If the database already exists or a new one was created successfully.
 * @return false If something went wrong when opening the database or creating a new one.
 */
bool openOrCreateDatabase(sqlite3 **database, const char *const filePath);

/**
 * @brief Selects user ID from the database user table using a PIN code.
 * 
 * @param database Database we're using.
 * @param pin The PIN code we're looking for a match for.
 * @param user_id_ptr Pointer to the user ID we're looking to get.
 * 
 * @return true If user with the PIN code was found and their user ID was returned.
 * @return false If no user with matching PIN code was found.
 */
bool selectUserIDByPIN(sqlite3 **database, const char *const pin, int *user_id_ptr);

/**
 * @brief Inserts a row to the log table.
 * 
 * @param database Database we're using.
 * @param user_id User id number that will be added to the row.
 * 
 * @return true If the insert was successful.
 * @return false If something went wrong with the insert.
 */
bool insertLogRow(sqlite3 **database, const int user_id);



int show_menu();
//int create_db(sqlite3 **db, char *db_name);
//int create_table_user(sqlite3 **db);
//int create_table_log(sqlite3 **db);
int insert_user_row(sqlite3 **db, char *rfid_id, char *name, int pin);
//int insert_log_row(sqlite3 **db, int user_id, int status);
int show_table_user(sqlite3 **db);
int show_table_log(sqlite3 **db);
int update_log_row(sqlite3 **db, int user_id, int status, char *datetime, int log_id);
int update_user_table_name(sqlite3 **db, int user_id, char *rfid_id, char *name, int pin);
int remove_user_row(sqlite3 **db, int user_id);
int remove_log_row(sqlite3 **db, int log_id);
int select_user_id(sqlite3 **db, char *table, char *rfid_id);
int select_userid_by_rfidid(sqlite3 **db, char *rfid_id, int *user_id_ptr);
int select_pin_by_userid(sqlite3 **db, int user_id, int *pin_value);
void pause_ch();



#endif // DATABASE_H

