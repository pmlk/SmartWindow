/* written by Simon Stallbaum
 * contains functions for the communication with a database
 *
 * ----  in the function "connect": hostname, username, pw etc. have to be set for the connection to the MySQL server ------
 *
 *           --------- read and write with the following three functions -----------
 *
 *bool write_in_db(MYSQL *mysql, const char *db, const char *table, const char *value)
 *parameter 1: mysql-Handle for a mysql-object
 *parameter 2: name of the database in wich you want to write
 *parameter 3: name of the table of the database in which you want to write
 *parameter 4: value you want to write (as string) - returns with sucess when successful
 *return value: true -> writing succeeded, false -> error occurred
 *
 *function: double get_latest_value_double(MYSQL *mysql, const char *db, const char *table, bool *succeeded)
 *function: bool get_latest_value_bool(MYSQL *mysql, const char *db, const char *table, bool *succeeded)
 *like function "write_in_db" -> name and table from wich you want to get data
 *parameter 4: you give a pointer where the function will put the information whether the writing
 *parameter 4: succeeded or not (true -> successful; false -> error occurred)
 *return value: latest value of the table you are reading from
 * -> depending on the function you use, you get a bool or a double
 */

#include "write_read.h"

double get_latest_value_double(MYSQL *mysql, const char *db, const char *table, bool *succeeded) {	//function gets and returns latest value from the stated table (as double)
	MYSQL_ROW  row;
	MYSQL_RES  *mysql_res;
	unsigned int i;
	char *max_id;
	char *value_max_id;
	double return_val;
	unsigned long int lbuf;
	bool error;

	connect(mysql);												// connect to database
	error = check_error(mysql);

	if (error == true){
		*succeeded = false;
		return 0;
	}

	if(mysql_select_db(mysql, db)==0){
//			printf("db successfully selected\n");
			}
	else{
		*succeeded = false;
		return 0;
	}

	char buf[BUF] = "SELECT MAX(id) AS id FROM ";					// search for highest id
			strcat(buf,	table);

	lbuf = strlen(buf);

	mysql_real_query(mysql, buf , lbuf);
	mysql_res = mysql_store_result(mysql);
	error = check_error(mysql);

	if (error == true){
		*succeeded = false;
		return 0;
	}


	while ((row = mysql_fetch_row (mysql_res)) != NULL) {			// print found data
		for (i = 0;  i < mysql_num_fields(mysql_res);  i ++){		// print single coulumns of the row
//			printf ("higest id is in row %s\n",row[i]);
			max_id = row[i];
		}
	}


	char buf2[BUF] = "SELECT Wert FROM ";							// get the value belonging the highest id
				strcat(buf2, table);								// create string for the query
				strcat(buf2, " WHERE ID=");
				strcat(buf2, max_id);


	lbuf = strlen(buf2);

	mysql_real_query(mysql, buf2 , lbuf);							// query server to search for the value belonging to the highest id
	mysql_res = mysql_store_result(mysql);
	check_error(mysql);

	if (error == true){
		*succeeded = false;
		return 0;
	}

	while ((row = mysql_fetch_row (mysql_res)) != NULL) {
		for (i = 0;  i < mysql_num_fields(mysql_res);  i ++){		// print single coulumns of the row
//			printf ("value in cell: %s \n",row[i]);
			value_max_id = row[i];
		}
	}

	close_session(mysql);
	return_val = atof(value_max_id);								// convert the value from sting to double
	*succeeded = true;
	return return_val;
}


bool get_latest_value_bool(MYSQL *mysql, const char *db, const char *table, bool *succeeded) {	//function gets and returns latest value from the stated table (as bool)
	MYSQL_ROW  row;
	MYSQL_RES  *mysql_res;
	unsigned int i;
	char *max_id;
	char *value_max_id;
	double return_val;
	unsigned long int lbuf;
	bool error;

	connect(mysql);												// connect to database
	check_error(mysql);

	if(mysql_select_db(mysql, db)==0){
//			printf("db successfully selected\n");
			}
	else{
			*succeeded = false;
			return 0;
	}

	char buf[BUF] = "SELECT MAX(id) AS id FROM ";				// search for highest id
			strcat(buf,	table);

	lbuf = strlen(buf);

	mysql_real_query(mysql, buf , lbuf);
	mysql_res = mysql_store_result(mysql);
	error = check_error(mysql);

	if (error == true){
		*succeeded = false;
		return 0;
	}


	while ((row = mysql_fetch_row (mysql_res)) != NULL) {			// print found data
		for (i = 0;  i < mysql_num_fields(mysql_res);  i ++){		// print single coulumns of the row
//			printf ("highest id is in row %s\n",row[i]);
			max_id = row[i];
		}
	}


	char buf2[BUF] = "SELECT Wert FROM ";							// get the value belonging the highest id
				strcat(buf2, table);								// create string for the query
				strcat(buf2, " WHERE ID=");							
				strcat(buf2, max_id);								


	lbuf = strlen(buf2);

	mysql_real_query(mysql, buf2 , lbuf);							// query server to search for the value belonging to the highest id
	mysql_res = mysql_store_result(mysql);
	error = check_error(mysql);

	if (error == true){
			*succeeded = false;
			return 0;
		}


	while ((row = mysql_fetch_row (mysql_res)) != NULL) {			// print single coulumns of the row
		for (i = 0;  i < mysql_num_fields(mysql_res);  i ++){
//			printf ("value in cell: %s \n",row[i]);
			value_max_id = row[i];
		}
	}

	close_session(mysql);
	return_val = atof(value_max_id);								// convert the value from sting to double
	*succeeded = true;
	return bool(return_val);
}

int get_latest_value_int(MYSQL *mysql, const char *db, const char *table, bool *succeeded) {	//function gets and returns latest value from the stated table (as bool)
	MYSQL_ROW  row;
	MYSQL_RES  *mysql_res;
	unsigned int i;
	char *max_id;
	char *value_max_id;
	double return_val;
	unsigned long int lbuf;
	bool error;

	connect(mysql);												// connect to database
	check_error(mysql);

	if(mysql_select_db(mysql, db)==0){
//			printf("db successfully selected\n");
			}
	else{
			*succeeded = false;
			return 0;
	}

	char buf[BUF] = "SELECT MAX(id) AS id FROM ";				// search for highest id
			strcat(buf,	table);

	lbuf = strlen(buf);

	mysql_real_query(mysql, buf , lbuf);
	mysql_res = mysql_store_result(mysql);
	error = check_error(mysql);

	if (error == true){
		*succeeded = false;
		return 0;
	}


	while ((row = mysql_fetch_row (mysql_res)) != NULL) {			// print found data
		for (i = 0;  i < mysql_num_fields(mysql_res);  i ++){		// print single coulumns of the row
//			printf ("highest id is in row %s\n",row[i]);
			max_id = row[i];
		}
	}


	char buf2[BUF] = "SELECT Wert FROM ";							// get the value belonging the highest id
				strcat(buf2, table);								// create string for the query
				strcat(buf2, " WHERE ID=");
				strcat(buf2, max_id);


	lbuf = strlen(buf2);

	mysql_real_query(mysql, buf2 , lbuf);							// query server to search for the value belonging to the highest id
	mysql_res = mysql_store_result(mysql);
	error = check_error(mysql);

	if (error == true){
			*succeeded = false;
			return 0;
		}


	while ((row = mysql_fetch_row (mysql_res)) != NULL) {			// print single coulumns of the row
		for (i = 0;  i < mysql_num_fields(mysql_res);  i ++){
//			printf ("value in cell: %s \n",row[i]);
			value_max_id = row[i];
		}
	}

	close_session(mysql);
	return_val = atoi(value_max_id);								// convert the value from sting to double
	*succeeded = true;
	return bool(return_val);
}


bool write_in_db(MYSQL *mysql, const char *db, const char *table, const char *value) {			// write data in database
	
	unsigned long int lbuf;
	connect(mysql);													// connect to db
	check_error(mysql);												// check if attempt to connect was successful
	bool error;

	char columnname_value []= "Wert";
	char columnname_time [] = "Zeitstempel";						// specify the names of the columns in the table


	if(mysql_select_db(mysql, db)==0){
//			printf("db successfully selected\n");
	}
	else{
		return false;
	}

	char buf[BUF] = "INSERT INTO ";									// create string for the query
			strcat(buf,	table);
			strcat(buf, " (");
			strcat(buf,	columnname_time);
			strcat(buf, ", ");
			strcat(buf, columnname_value);
			strcat(buf, ") VALUES (");							
			strcat(buf, "NOW()");
			strcat(buf, ", ");
			strcat(buf, value);
			strcat(buf, ");");

	lbuf = strlen(buf);

    mysql_real_query(mysql, buf , lbuf);			 				// query server with the above created string
    error = check_error(mysql);
    close_session(mysql);
    if (error == true) {
    	return false;
    }
    return true;
}


bool check_error(MYSQL *mysql)  {									// check for errors
    if (mysql_errno(mysql) != 0) {
//       fprintf (stderr, "error: %s\n", mysql_error(mysql));
//       exit(EXIT_FAILURE);
    	 return true;
    }
    return false;
}


void connect(MYSQL *mysql){											// connect to server
   mysql=mysql_init(mysql);
   check_error(mysql);

   if (mysql_real_connect (											// specify data for the connection to server
           mysql,   /* pointer to MYSQL-handler */
           "127.0.0.1", /* host-name */
           "root", /* user-Name */
		   "1234", /* password for user-name */
		   "SmartWindow",  /* name of database */
           3306,     /* port (default=0) */
		   "/var/run/mysqld/mysqld.sock",  /* socket (default=NULL) */
           0      /* no flags */  )  == NULL) {
	   fprintf (stderr, "error mysql_real_connect():"
	           "%u (%s)\n",mysql_errno (mysql), mysql_error (mysql));

	   fprintf(stderr, "connection failed");
	   	   }
   	   	   else {
 //          printf ("connection was build successfully\n");
       	   }
}


void close_session(MYSQL *mysql)  {									// close connection to server
   mysql_close(mysql);
}
