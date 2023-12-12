/**
 * @file database.c
 * @author jaakho & Selkamies.
 * 
 * @brief Database operations.
 * 
 * @date Created  2023-12-08
 * @date Modified 2023-12-12
 * 
 * @copyright Copyright (c) 2023
 */



#include <stdio.h>               // stderr.
#include <stdbool.h>
#include <string.h>              // strcmp().

#include <sqlite3.h>             // sqlite3, sqlite3_stmt, sqlite3_prepare_v2(), etc.

#include "database.h"            // DATABASE_FILEPATH, DATABASE_PATH, DATABASE_NAME.
#include "database_sql.h"        // #defines for SQL statements, table and column names.



#pragma region FunctionDeclarations

/**
 * @brief Function pointer type named RowCallback that takes statement and data paremeters.
 * Used for SELECT statements to handle and set the data we get back from the statement to the data pointer.
 */
typedef void (*RowCallback)(sqlite3_stmt *statement, void *data);

/**
 * @brief Create all the tables in the database.
 * 
 * @param database Database we're using.
 * @return true If the tables were created successfully.
 * @return false If something went wrong when creating the tables.
 */
static bool createTables(sqlite3 **database);

/**
 * @brief Checks if a table already exists in the database.
 * 
 * @param database Database we're using.
 * @param tableName Name of the table to check.
 * 
 * @return true If the table already exists.
 * @return false If the table doesn't exist.
 */
static bool tableExists(sqlite3 **database, const char *tableName);

/**
 * @brief Inserts test data to users table.
 * 
 * @param database Database we're using.
 * 
 * @return true If the insert was successful.
 * @return false If something went wrong with the insert.
 */
static bool insertUserTestData(sqlite3 **database);

/**
 * @brief Executes a SELECT SQL statement.
 * 
 * @param statement SQL statement in a format that SQLite uses.
 * @param callback Callback function for SELECT statements.
 * @param data Pointer to a variable or stuct to get return values from SELECT statements.
 * 
 * @return true If the statement was executed successfully.
 * @return false If something went wrong with the statement.
 */
static bool executeSelect(sqlite3_stmt *statement, RowCallback callback, void *data);

/**
 * @brief Executes an INSERT SQL statement.
 * 
 * @param statement SQL statement in a format that SQLite uses.
 * 
 * @return true If the statement was executed successfully.
 * @return false If something went wrong with the statement.
 */
static bool executeInsert(sqlite3_stmt *statement);

/**
 * @brief Callback function for selectUserIDByPIN(), used to get SELECT statement data.
 * 
 * @param statement SQL statement in a format that SQLite uses.
 * @param data Pointer to the data we need back. In this case, user's ID number.
 */
static void selectUserIDByPINCallback(sqlite3_stmt *statement, void *data);

#pragma endregion // FunctionDeclatarions



bool openOrCreateDatabase(sqlite3 **database, const char *const filePath)
{
    int returnCode = sqlite3_open_v2(filePath, database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    // Something went wrong with opening or creating the database.
    if (returnCode != SQLITE_OK) 
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*database));

        return false;
    }

    if (!tableExists(database, TABLE_USER) && !tableExists(database, TABLE_LOG)) 
    {
        createTables(database);
        insertUserTestData(database);
    }

    return true;
}

static bool tableExists(sqlite3 **database, const char *tableName)
{;
    sqlite3_stmt *statement;

    int resultCode = sqlite3_prepare_v2(*database, SELECT_TABLE_EXISTS, -1, &statement, 0);

    if (resultCode != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(*database));
        return false;
    }

    sqlite3_bind_text(statement, 1, tableName, -1, SQLITE_STATIC);

    int exists = 0;
    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        exists = 1;
    }

    sqlite3_finalize(statement);

    return exists;
}

static bool createTables(sqlite3 **database)
{
    int returnCode = sqlite3_exec(*database, CREATE_TABLE_USER, 0, 0, 0);

    if (returnCode != SQLITE_OK) 
    {
        fprintf(stderr, "Cannot create table: %s\n", sqlite3_errmsg(*database));

        return false;
    }

    returnCode = sqlite3_exec(*database, CREATE_TABLE_LOG, 0, 0, 0);

    if (returnCode != SQLITE_OK) 
    {
        fprintf(stderr, "Cannot create table: %s\n", sqlite3_errmsg(*database));
        
        return false;
    }

    printf("Tables created.\n");

    return true;
}

static bool insertUserTestData(sqlite3 **database)
{
    sqlite3_stmt *statement;
    int resultCode = sqlite3_prepare_v2(*database, INSERT_INTO_USER_TEST_ROWS, -1, &statement, 0);

    if (resultCode != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(*database));

        return false;
    }

    return executeInsert(statement);
}



static bool executeSelect(sqlite3_stmt *statement, RowCallback callback, void *data)
{
    // Execute the prepared statement.
    int resultCode = sqlite3_step(statement);

    // No rows returned.
    if (resultCode == SQLITE_DONE)
    {
        //fprintf(stderr, "No rows in the result set.\n");
        sqlite3_finalize(statement);

        return false;
    }

    // Execute the prepared statement row by row and handle callbacks for return values.
    while (resultCode == SQLITE_ROW)
    {
        // If a callback function is provided to get return values of SELECT, it's called here.
        if (callback)
        {
            // Callback function gets the statement and pointer to data we want back.
            callback(statement, data);
        }

        // Fetch the next row.
        resultCode = sqlite3_step(statement);
    }

    // Finalize the statement. This frees up memory/resources created for the statement.
    sqlite3_finalize(statement);

    return true;
}

static bool executeInsert(sqlite3_stmt *statement)
{
     // Execute the prepared statement.
    int resultCode = sqlite3_step(statement);

    // Check the result code to determine success or failure.
    if (resultCode == SQLITE_DONE)
    {
        fprintf(stdout, "Insert successful.\n");
        sqlite3_finalize(statement);

        return true;
    }

    else
    {
        fprintf(stderr, "Insert failed. SQL error: %s\n", sqlite3_errmsg(sqlite3_db_handle(statement)));
        sqlite3_finalize(statement);

        return false;
    }
}

bool selectUserIDByPIN(sqlite3 **database, const char *const pin, int *user_id_ptr)
{
    // SQL statement in a format that SQLite uses.
    sqlite3_stmt *statement;

    // Compiles the sql statement from string (SELECT_USER_ID_BY_PIN) to a format (*statement) that SQLite uses.
    // -1 (int nByte) tells to use the length of the entire string statement.
    // 0 (const char **psTail) is an output parameter.
    int resultCode = sqlite3_prepare_v2(*database, SELECT_USER_ID_BY_PIN, -1, &statement, 0);

    if (resultCode != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(*database));

        return false;
    }

    // Bind the pin value to the ? placeholder in the SQL statement.
    // 1 is the number of ? to bind to, in this case the first and only one.
    // -1 tells the function that the number of bytes should be determined automatically.
    // SQLITE_STATIC tells the function not to delete the string paratemer (pin) afterwards.
    sqlite3_bind_text(statement, 1, pin, -1, SQLITE_STATIC);

    // Pass the statement to a function that executes it. Since we're selecting something,
    // we also pass a callback function and data pointer to get the values we want to select.
    return executeSelect(statement, selectUserIDByPINCallback, user_id_ptr);
}

// Callback function for selectUserIDByPIN. Callback functions are needed for SELECT statements.
static void selectUserIDByPINCallback(sqlite3_stmt *statement, void *data)
{
    // Cast the generic void pointer to int. This points to the same address as *data.
    int *user_id_ptr = (int *)data;

    // Assigns the value to the pointer, which also changes original pointer passed as *data.
    *user_id_ptr = sqlite3_column_int64(statement, 0);
}

bool insertLogRow(sqlite3 **database, const int user_id)
{
    sqlite3_stmt *statement;
    int resultCode = sqlite3_prepare_v2(*database, INSERT_LOG_ROW, -1, &statement, 0);

    if (resultCode != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(*database));
        return false;
    }

    sqlite3_bind_int(statement, 1, user_id);

    return executeInsert(statement);
}





// TODO: Intellisense says sqlite3_callback is deprecated, but no idea about alternatives.
typedef int (*sqlite3_callback)(
   void*,    /* Data provided in the 4th argument of sqlite3_exec() */
   int,      /* The number of columns in row */
   char**,   /* An array of strings representing fields in the row */
   char**    /* An array of strings representing column names */
);

static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   
   printf("\n");
   return 0;
}
int show_menu(){
	int menu_choice;
	
printf("\n###############################################\n");
printf("# Tietokanta olemassa nimella db_jaju\n");
printf("#\n");
printf("# kannassa USER ja LOG taulu  \n");
printf("##################################################\n");
printf("(1) lisaa USER tauluun rivi (oletusarvoilla) \n");
printf("(2) lisaa LOG tauluun rivi (oletusarvoilla)\n");
printf("----------------------------------------- \n");
printf("(3) lisaa USER tauluun rivi (anna tiedot sarakkeisiin)\n");
printf("(4) lisaa LOG tauluun rivi (anna tiedot sarakkeisiin)\n");
printf("----------------------------------------- \n");
printf("(5) nayta USER taulun kaikki rivit\n");
printf("(6) nayta USER taulun user_id hakuna rfid_id\n");
printf("(12) nayta USER taulun pin hakuna user_id\n");
printf("(7) nayta LOG taulun kaikki rivit\n");
printf("----------------------------------------- \n");
printf("(8) paivita USER taulun user_id rivi (anna tiedot sarakkeisiin)\n");
printf("(9) paivita LOG taulun log_id rivi (anna tiedot sarakkeisiin)\n");
printf("----------------------------------------- \n");
printf("(10) poista USER taulun user_id rivi\n");
printf("(11) poista LOG taulun log_id rivi\n");
printf("----------------------------------------- \n");
printf("(0) Ohjelman lopetus\n");
printf("----------------------------------------- \n");
printf("Valitse toiminto numerolla (0-11) :");

  scanf("%d", &menu_choice);
  return menu_choice;
}
/*
 
  
*/
/* int create_db(sqlite3 **db, char *db_name){
   
   char *zErrMsg = 0;
   int rc=0;
   //rc = sqlite3_open("jaju.db", db);
   rc = sqlite3_open(db_name, db);
  if( rc ) {
	fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
	return(0);
  } else {
	fprintf(stderr, "Opened database successfully\n");
	return (1);
  }
  return rc;
} */

/* int create_table_user(sqlite3 **db){
	char *zErrMsg = 0;
	int rc=0;
	char *sql;
	// Create SQL statement 
   sql = "CREATE TABLE IF NOT EXISTS USER (" \
		"USER_ID INTEGER PRIMARY KEY AUTOINCREMENT, " \
		"RFID_ID CHAR(20), " \
		"NAME TEXT, " \
		"PIN INTEGER);";

   // Execute SQL statement
   rc = sqlite3_exec(*db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      	return(0);
   } else {
      fprintf(stdout, "Table created successfully\n");
      	return(1);
   }
	return rc;
} */

/* int create_table_log(sqlite3 **db){
	char *zErrMsg = 0;
	int rc;
	char *sql;
	// Create SQL statement 
   sql = "CREATE TABLE IF NOT EXISTS LOG (" \
		"LOG_ID INTEGER PRIMARY KEY AUTOINCREMENT, " \
		"USER_ID INTEGER NOT NULL," \
		"STATUS INTEGER, "\
		"LOG_TIME DATETIME DEFAULT CURRENT_TIMESTAMP);";
		
   // Execute SQL statement
   rc = sqlite3_exec(*db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return(0);
   } else {
      fprintf(stdout, "Table created successfully\n");
       return(1);
   }
	return rc;
} */

int insert_user_row(sqlite3 **db, char *rfid_id, char *name, int pin){
	char *zErrMsg = 0;
	int rc=0;
	char sql[500];
/* Create SQL statement */
    sprintf(sql, "INSERT INTO USER (RFID_ID,NAME,PIN) "  \
        "VALUES ('%s', '%s', %d ) ;" ,rfid_id,name, pin);
 
   /* Execute SQL statement */
   rc = sqlite3_exec(*db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return(0);
   } else {
      fprintf(stdout, "Records created successfully\n");
      return(1);
   }
	return rc;
}

/* int insert_log_row(sqlite3 **db, int user_id, int status){
	char *zErrMsg = 0;
	int rc=0;
	char sql[500];
// Create SQL statement 
     sprintf(sql, "INSERT INTO LOG (USER_ID, STATUS) VALUES (%d, %d);" ,user_id,status);
   
   // Execute SQL statement
   rc = sqlite3_exec(*db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return(0);
   } else {
      fprintf(stdout, "Records created successfully\n");
      return(1);
   }
	return rc;
} */

int show_table_user(sqlite3 **db){
	char *zErrMsg = 0;
	int rc;
	const char* data = "Callback function called\n";

	char *sql;
	/* Create SQL statement */
   sql = "SELECT * from USER";

   /* Execute SQL statement */
   rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
   
      if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return(0);
   } else {
      fprintf(stdout, "Operation done successfully\n");
      return(1);
   }
	return rc;
}
int show_table_log(sqlite3 **db){
	char *zErrMsg = 0;
	int rc;
	const char* data = "Callback function called\n";
	char *sql;
	/* Create SQL statement */
   sql = "SELECT * from LOG";

   /* Execute SQL statement */
   rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
   
     if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return(0);
   } else {
      fprintf(stdout, "Operation done successfully\n");
      return(1);
   }
	return rc;
}

int update_user_table_name(sqlite3 **db, int user_id, char *rfid_id, char *name, int pin){
	char *zErrMsg = 0;
	int rc;
	const char* data = "Callback function called\n";
	// Create SQL statement 
	char sql[500];
	sprintf(sql, "UPDATE USER set RFID_ID = '%s', NAME = '%s', PIN = %d where USER_ID = %d;" ,rfid_id, name, pin, user_id);

   
   rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
   
     if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return(0);
   } else {
      fprintf(stdout, "Operation done successfully\n");
      return(1);
   }
	return rc;
}
int update_log_row(sqlite3 **db, int user_id, int status, char *datetime, int log_id){
	char *zErrMsg = 0;
	int rc;
	const char* data = "Callback function called\n";
	// Create SQL statement 
	char sql[500];
	sprintf(sql, "UPDATE LOG set USER_ID = %d , STATUS = %d ,  LOG_TIME = strftime('%s') where LOG_ID = %d;" ,user_id, status, datetime, log_id);   
   rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
   
     if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return(0);
   } else {
      fprintf(stdout, "Operation done successfully\n");
      return(1);
   }
	return rc;
}

int remove_user_row(sqlite3 **db, int user_id){
	char *zErrMsg = 0;
	int rc;
	const char* data = "Callback function called\n";
	// Create SQL statement 
	char sql[500];

   sprintf(sql, "DELETE from USER where USER_ID=%d; " ,user_id );

   /* Execute SQL statement */
   rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
     if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return(0);
   } else {
      fprintf(stdout, "Operation done successfully\n");
      return(1);
   }
	return rc;
}
int remove_log_row(sqlite3 **db, int log_id){
	char *zErrMsg = 0;
	int rc;
	const char* data = "Callback function called\n";
	// Create SQL statement 
	char sql[500];

   sprintf(sql, "DELETE from LOG where LOG_ID=%d; " ,log_id );

   /* Execute SQL statement */
   rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
     if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return(0);
   } else {
      fprintf(stdout, "Operation done successfully\n");
      return(1);
   }
	return rc;
}
int select_user_id(sqlite3 **db, char *table, char *rfid_id){
	char *zErrMsg = 0;
	int rc;
	const char* data = "Callback function called\n";


	char sql[500];
    sprintf(sql, "SELECT * from '%s' where RFID_ID='%s'; " ,table, rfid_id );

   /* Execute SQL statement */
   rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
   
      if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return(0);
   } else {
      fprintf(stdout, "Operation done successfully\n");
      return(1);
   }
	return rc;
	
}
int select_userid_by_rfidid(sqlite3 **db, char *rfid_id, int *user_id_ptr){
   
    char *err_msg = 0;
    sqlite3_stmt *res;
    char *zErrMsg = 0;
    int rc;
    
    char sql[500];
	sprintf(sql, "SELECT USER_ID FROM USER WHERE RFID_ID='%s';" ,rfid_id);
	
	rc = sqlite3_prepare_v2(*db, sql, -1, &res, 0);
	
    int perke;
	
	if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return(0);
	} 
	else {
			while(1)
			{
			// fetch a row's status
			rc = sqlite3_step(res);
				if(rc == SQLITE_ROW) {
					//some = (int)sqlite3_column_int(res, 0);
						// or other type - sqlite3_column_text etc.
					// ... fetch other columns, if there are any'
					*user_id_ptr = (int)sqlite3_column_int(res, 0);
					//if first result row
					break;
				}
				else if(rc == SQLITE_DONE){
					//loppu
					break;
				}
				else {
					sqlite3_finalize(res);
					printf("some error encountered\n");
					break;
				}    
			}
			fprintf(stdout, "Operation done successfully\n");
			return(1);
	}
	
	return rc;	
}
int select_pin_by_userid(sqlite3 **db, int user_id, int *pin_value){
   
    char *err_msg = 0;
    sqlite3_stmt *res;
    char *zErrMsg = 0;
    int rc;
    
    char sql[500];
	sprintf(sql, "SELECT PIN FROM USER WHERE USER_ID= %d ;" ,user_id);
	
	rc = sqlite3_prepare_v2(*db, sql, -1, &res, 0);
	
    int perke;
	
	if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return(0);
	} 
	else {
			while(1)
			{
			// fetch a row's status
			rc = sqlite3_step(res);
				if(rc == SQLITE_ROW) {
					//some = (int)sqlite3_column_int(res, 0);
						// or other type - sqlite3_column_text etc.
					// ... fetch other columns, if there are any'
					*pin_value = (int)sqlite3_column_int(res, 0);
					//if first result row
					break;
				}
				else if(rc == SQLITE_DONE){
					//loppu
					break;
				}
				else {
					sqlite3_finalize(res);
					printf("some error encountered\n");
					break;
				}    
			}
			fprintf(stdout, "Operation done successfully\n");
			return(1);
	}
	
	return rc;	
}
void pause_ch(){
	char ch;
	while((getchar()) != '\n');
		printf("\n\nEnter the Character to continue: ");
							scanf("%c", &ch);
	
}

