/**
 * @file database.c
 * @author jaakho & Selkamies.
 * 
 * @brief Database operations.
 * 
 * @date Created  2023-12-08
 * @date Modified 2023-12-08
 * 
 * @copyright Copyright (c) 2023
 */



#include <stdio.h>               // stderr.
#include <stdbool.h>

#include <sqlite3.h>

#include "database.h"
#include "database_config.h"



bool selectUserIDByPIN(sqlite3 **database, const char *pin, int *user_id_ptr)
{
   sqlite3_stmt *statement;
   int resultCode;

   // Prepare the SQL statement.
   resultCode = sqlite3_prepare_v2(*database, SELECT_USER_ID_BY_PIN, -1, &statement, 0);

   if (resultCode != SQLITE_OK) 
   {
      fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(*database));
      return false;
   }

   // Bind the pin value to the ? placeholder.
   // 1 is the number of ? to bind to, in this case the first and only one.
   // -1 tells the function that the number of bytes should be determined automatically.
   // SQLITE_STATIC tells the function not to delete the string paratemer (pin) afterwards.
   sqlite3_bind_text(statement, 1, pin, -1, SQLITE_STATIC);

   // Execute the prepared statement.
   while ((resultCode = sqlite3_step(statement)) == SQLITE_ROW) 
   {
      // Fetch the user_id from the result set.
      *user_id_ptr = sqlite3_column_int(statement, 0);
   }

   if (resultCode != SQLITE_DONE) 
   {
      fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(*database));
      sqlite3_finalize(statement);
      return false;
   }

   // Finalize the statement.
   sqlite3_finalize(statement);
   fprintf(stdout, "Operation done successfully\n");

   return true;	
}

bool insertLogRow(sqlite3 **database, const int user_id)
{
   sqlite3_stmt *statement;
   int resultCode;

   // Prepare the SQL statement.
   resultCode = sqlite3_prepare_v2(*database, INSERT_LOG_ROW, -1, &statement, 0);

   if (resultCode != SQLITE_OK) 
   {
      fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(*database));
      return false;
   }

   sqlite3_bind_int(statement, 1, user_id);

   // Execute the prepared statement.
   while ((resultCode = sqlite3_step(statement)) == SQLITE_ROW) 
   {
      // Fetch the user_id from the result set.
      //*user_id_ptr = sqlite3_column_int(statement, 0);
   }

   if (resultCode != SQLITE_DONE) 
   {
      fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(*database));
      sqlite3_finalize(statement);
      return false;
   }

   // Finalize the statement.
   sqlite3_finalize(statement);
   fprintf(stdout, "Operation done successfully\n");

   return true;
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
int create_db(sqlite3 **db, char *db_name){
   
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
}

int create_table_user(sqlite3 **db){
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
}
int create_table_log(sqlite3 **db){
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
}

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
int insert_log_row(sqlite3 **db, int user_id, int status){
	char *zErrMsg = 0;
	int rc=0;
	char sql[500];
/* Create SQL statement */
     sprintf(sql, "INSERT INTO LOG (USER_ID, STATUS) VALUES (%d, %d);" ,user_id,status);
   
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

