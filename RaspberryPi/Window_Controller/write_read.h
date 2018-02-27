/**
 * written by Simon Stallbaum
 * header for write_read.cpp
 */
#define BUF 255

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "stddef.h"

#ifdef XCOMPILE
// when Cross-Compiling
#include "xcompile/headers/mysql.h"
#else
// when compiling locally on RasPi
#include <mysql/mysql.h>
#endif

bool check_error(MYSQL *mysql);		// check for errors
void connect(MYSQL *mysql);		// connect to server
void close_session(MYSQL *mysql);	// close connection to server
bool write_in_db(MYSQL *mysql,const char *db, const char *table, const char *value);	// write data in database
bool get_latest_value_bool(MYSQL *mysql, const char *db, const char *table, bool *succeeded);	//function gets and returns latest value from the stated table (as bool)
double get_latest_value_double(MYSQL *mysql, const char *db, const char *table, bool *succeeded);	//function gets and returns latest value from the stated table (as double)
int get_latest_value_int(MYSQL *mysql, const char *db, const char *table, bool *succeeded);		//function gets and returns latest value from the stated table (as int)
