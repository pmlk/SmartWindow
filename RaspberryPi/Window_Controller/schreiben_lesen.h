/* Erstellt von Simon Stallbaum
 * Funktion: Header mit Funktionen, die in Datenbank schreiben und jüngsten Wert aus Tabellen herauslesen können
 *
 * ----  In der Funktion "verbinden" müssen Hostname, Username, PW etc. für die Verbindung zum Server angegeben werden ------
 *
 *           --------- Folgende zwei Funktionen dienen als Schnittstellen zum schreiben und lesen -----------
 *
 *Funktion: int schreibe_in_db(MYSQL *mysql,const char *db, const char *tabelle, struct CGI_DATEN *daten, const char *wert);
 *Parameter 1: mysql-Handle für ein mysql-Objekt
 *Parameter 2: name der existierenden Datenbank in die Geschrieben werden soll
 *Parameter 3: name der existierenden Tabelle in die geschrieben werden soll
 *Parameter 4: struct CGI_DATEN *daten -> beinhaltet Zeiger auf Spaltennamen
 *Parameter 5: wert der geschrieben werden soll (als String)
 *
 *
 *Funktion: double daten_suchen(MYSQL *mysql, const char *db, const char *tabelle, struct CGI_DATEN *daten);
 *Parameter: siehe Funktion "schreibe_in_db" -> Datenbankname und Tabellenname aus der herausgelesen werden soll
 *Rückgabewert: jüngster Wert in der ausgelesenen Tabelle
 */



#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
//#include "mysql/headers/mysql.h"
#include <string.h>
#include <time.h>

#define BUF 255

struct CGI_DATEN {
	const char *p_wert;
	char *p_spaltenname_zeitstempel;
	char *p_spaltenname_wert;
};

void check_error(MYSQL *mysql);
void verbinden(MYSQL *mysql);
void verbindung_schliessen(MYSQL *mysql);
void db_waehlen(MYSQL *mysql, char *db);
int schreibe_in_db(MYSQL *mysql,const char *db, const char *tabelle, struct CGI_DATEN *daten, const char *wert);
double daten_suchen(MYSQL *mysql, const char *db, const char *tabelle, struct CGI_DATEN *daten);

double daten_suchen(MYSQL *mysql, const char *db, const char *tabelle, struct CGI_DATEN *daten) {
	MYSQL_ROW  row;
	MYSQL_RES  *mysql_res;
	unsigned long  anzahl_reihen;
	unsigned int i;
	//char *max_id;
	//char *wert_max_id;
	char *max_id = new char[128];
	char *wert_max_id = new char[128];
	//char *max_id[128];
	//char *wert_max_id[128];

	double return_val;

	char spaltenname_wert []= "Wert";
	char spaltenname_zeit [] = "Zeitstempel";
	daten->p_spaltenname_wert = spaltenname_wert;
	daten->p_spaltenname_zeitstempel = spaltenname_zeit;


	verbinden(mysql);										// Verbindung herstellen
	check_error(mysql);

	if(mysql_select_db(mysql, db)==0){
			printf("db erfolgreich ausgewählt\n");
	}

	char buf[BUF] = "SELECT MAX(id) AS id FROM ";			// suchen nach höchster id
			strcat(buf,	tabelle);

	mysql_real_query(mysql, buf, strlen(buf));
	mysql_res = mysql_store_result(mysql);
	check_error(mysql);

	anzahl_reihen = (unsigned long) mysql_num_rows (mysql_res);		// gefundene Datensätze ermitteln
//	printf ("Anzahl gefunden: %lu\n", anzahl_reihen);

	/* gefundenen Datensatz bzw. Datensätze ausgeben */
	while ((row = mysql_fetch_row (mysql_res)) != NULL) {
	   /* einzelne Spalten der Zeile ausgeben */
		for (i = 0;  i < mysql_num_fields(mysql_res);  i ++){
			printf ("Hoechste id in Zeile %s\n",row[i]);
			max_id = row[i];
		}
	}


	char buf2[BUF] = "SELECT Wert FROM ";
				strcat(buf2, tabelle);
				strcat(buf2, " WHERE ID=");				// suchen nach Wert der hinter der höchsten
				strcat(buf2, max_id);					// ID steckt

	mysql_real_query(mysql, buf2, strlen(buf2));
	mysql_res = mysql_store_result(mysql);
	check_error(mysql);

	while ((row = mysql_fetch_row (mysql_res)) != NULL) {
	   /* einzelne Spalten der Zeile ausgeben */
		for (i = 0;  i < mysql_num_fields(mysql_res);  i ++){
			printf ("Wert in Zelle: %s",row[i]);
			printf("\n");
			wert_max_id = row[i];
		}
	}
	verbindung_schliessen(mysql);
	return_val = atof(wert_max_id);

	//printf("wert_max_id: %s return_val: %f\n", wert_max_id, return_val);
	return return_val;
}


int schreibe_in_db(MYSQL *mysql, const char *db, const char *tabelle, struct CGI_DATEN *daten, const char *wert) {			// daten in DB eintragen

	verbinden(mysql);										// Verbindung herstellen
	check_error(mysql);										// Auswerten ob Verbindungsversuch erfolgreich

	char spaltenname_wert []= "Wert";
	char spaltenname_zeit [] = "Zeitstempel";
	daten->p_wert = wert;
	daten->p_spaltenname_wert = spaltenname_wert;
	daten->p_spaltenname_zeitstempel = spaltenname_zeit;

	if(mysql_select_db(mysql, db)==0){
//				 printf("db erfolgreich ausgewählt\n");
			 }

	char buf[BUF] = "INSERT INTO ";
			strcat(buf,	tabelle);
			strcat(buf, " (");
			strcat(buf,	daten->p_spaltenname_zeitstempel);
			strcat(buf, ", ");
			strcat(buf, daten->p_spaltenname_wert);
			strcat(buf, ") VALUES (");		// Eintrag manuell erzeugen
			strcat(buf, "NOW()");
			strcat(buf, ", ");
			strcat(buf, daten->p_wert);
			strcat(buf, ");");


   /* Jetzt die Anfrage an den Datenbankserver */
    mysql_real_query(mysql, buf, strlen(buf));			 						// Eintrag schreiben
    check_error(mysql);
    verbindung_schliessen(mysql);
    return EXIT_SUCCESS;
}




void check_error(MYSQL *mysql)  {					// Fehlerüberprüfung
    if (mysql_errno(mysql) != 0) {
       fprintf (stderr, "Fehler: %s\n", mysql_error(mysql));
       exit(EXIT_FAILURE);
    }
}


void verbinden(MYSQL *mysql){						// verbindung zum Server aufbauen
   mysql=mysql_init(mysql);
   check_error(mysql);

   if (mysql_real_connect (
           mysql,   /* Zeiger auf MYSQL-Handler */
           "127.0.0.1", /* Host-Name */
           "root", /* User-Name */
		   "1234", /* Passwort für user_name */
		   "SmartWindow",  /* Name der Datenbank */
           3306,     /* Port (default=0) */
		   "/var/run/mysqld/mysqld.sock",  /* Socket (default=NULL) */
           0      /* keine Flags */  )  == NULL) { fprintf(stderr, "Verbindung fehlgeschlagen");
	   	   }
   	   	   else {
 //          printf ("Verbindung wurde erfolgreich aufgebaut\n");
       	   	   	   }
}


void verbindung_schliessen(MYSQL *mysql)  {			// Verbindung zum Server abbauen
   mysql_close(mysql);
}


void db_waehlen(MYSQL *mysql, char *db) {			//zum wechseln der DB
   mysql_select_db(mysql, db);
   check_error(mysql);
}
