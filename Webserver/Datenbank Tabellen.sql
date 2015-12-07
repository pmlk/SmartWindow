/* Tabellen für das Projekt SmartWindow
erstellt am: 22.11.2015
von: Mailina
Inhalt: Alle Create Table aufrufe, die für das Projekt SmartWindow notwendig sind.
		Gemacht für eine Datenbank mit dem Namen `SmartWindow´
Rev: 0.0 - 22.11.2015 erstellt
*/

/* -- Erstellung der Datenbank `SmartWindow´
Create Schema SmartWindow;
*/

/* -- Drop Befehle Alte Tabellen
Drop table SmartWindow.Air;
Drop table SmartWindow.InHum;
Drop table SmartWindow.InTemp;
Drop table SmartWindow.ManClose;
Drop table SmartWindow.ManOpen;
Drop table SmartWindow.MaxAir;
Drop table SmartWindow.MaxHum;
Drop table SmartWindow.MaxSound;
Drop table SmartWindow.MaxTemp;
Drop table SmartWindow.MaxWind;
Drop table SmartWindow.MinHum;
Drop table SmartWindow.MinTemp;
Drop table SmartWindow.Modus;
Drop table SmartWindow.OutHum;
Drop table SmartWindow.OutTemp;
Drop table SmartWindow.Sound;
Drop table SmartWindow.StateWindow;
Drop table SmartWindow.Wind;
*/

/* -- Drop Befehle Neue Tabellen
Drop table SmartWindow.AirPressure_IN;
Drop table SmartWindow.AirPressure_MAX;
Drop table SmartWindow.AirPressure_OUT;
Drop table SmartWindow.AirQuality_IN;
Drop table SmartWindow.AirQuality_MAX;
Drop table SmartWindow.AirQuality_OUT;
Drop table SmartWindow.AutoModus;
Drop table SmartWindow.Humidity_IN;
Drop table SmartWindow.Humidity_MAX;
Drop table SmartWindow.Humidity_MIN;
Drop table SmartWindow.Humidity_OUT;
Drop table SmartWindow.ManClose;
Drop table SmartWindow.ManOpen;
Drop table SmartWindow.Win_Open;
Drop table SmartWindow.Temp_IN;
Drop table SmartWindow.Temp_MAX;
Drop table SmartWindow.Temp_MIN;
Drop table SmartWindow.Temp_OUT;
Drop table SmartWindow.Volume_IN;
Drop table SmartWindow.Volume_MAX;
Drop table SmartWindow.Volume_OUT;
*/


	-- Befehle

-- Modus 0: Manuel 1: Automatik
Create table SmartWindow.AutoModus(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		bool not null default TRUE);
     
-- Manueller Befehl zum Öffnen 0: Deaktiviert (vom Befehl Schließen oder Automatikmodus zurückgesetzt) 1: Aktiviert
Create table SmartWindow.ManOpen(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		bool not null);
    
-- Manueller Befehl zum Schließen 0: Deaktiviert (vom Befehl Öffnen oder Automatikmodus zurückgesetzt) 1: Aktiviert
Create table SmartWindow.ManClose(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		bool not null);
    
    
	-- Werte der Sensoren
    
-- Fenster Status 0: geschlossen 1: offen
Create table SmartWindow.Win_Open(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		bool not null);
    
-- Außentemperatur in Grad Celsius
Create table SmartWindow.Temp_OUT(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
    
-- Innentemperatur in Grad Celsius
Create table SmartWindow.Temp_IN(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
    
-- Luftfeuchtigkeit außen in %
Create table SmartWindow.Humidity_Out(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
    
-- Luftfeuchtigkeit innen in %
Create table SmartWindow.Humidity_IN(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
    
-- Außenluftqualität in Vol. %
Create table SmartWindow.AirQuality_OUT(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);

-- Innenluftqualität in Vol. %
Create table SmartWindow.AirQuality_IN(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
            
-- Außenwindgeschwindigkeit in m/s
Create table SmartWindow.AirPressure_OUT(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int not null);
 
-- Innenwindgeschwindigkeit in m/s
Create table SmartWindow.AirPressure_IN(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int not null);
    
-- Außengeräuschlevel in dB
Create table SmartWindow.Volume_OUT(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
  
-- Innengeräuschlevel in dB
Create table SmartWindow.Volume_IN(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
    
    
    -- Limits zur Berechnung
    
-- Minimale Temperatur in Grad Celsius
Create table SmartWindow.Temp_MIN(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float default 20.0);
    
-- Maximale Temperatur in Grad Celsius
Create table SmartWindow.Temp_MAX(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float default 26.0);
    
-- Minimale  Luftfeuchtigkeit in %
Create table SmartWindow.Humidity_MIN(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float default 30.0);
    
-- Maximale Luftfeuchtigkeit in %
Create table SmartWindow.Humidity_MAX(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float default 65.0);
    
-- Maximales CO2 in Vol.%
Create table SmartWindow.AirQuality_MAX(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float default 0.1);
    
-- Maximale Windgeschwindigkeit in m/s
Create table SmartWindow.AirPressure_MAX(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 1020);
    
-- Maximale Lautstärke in db
Create table SmartWindow.Volume_MAX(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float default 60.0);


	-- Anfangswerte aller Tabellen
Insert Into SmartWindow.AirPressure_IN(Zeitstempel, Wert) Value (current_timestamp(), 1010); 
Insert Into SmartWindow.AirPressure_MAX(Zeitstempel) Value (current_timestamp());
Insert Into SmartWindow.AirPressure_OUT(Zeitstempel, Wert) Value (current_timestamp(), 1016);
Insert Into SmartWindow.AirQuality_IN(Zeitstempel, Wert) Value (current_timestamp(), 0.01);
Insert Into SmartWindow.AirQuality_MAX(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.AirQuality_OUT(Zeitstempel, Wert) Value (current_timestamp(), 0.01);
Insert Into SmartWindow.AutoModus(Zeitstempel) Value (current_timestamp());
Insert Into SmartWindow.Humidity_IN(Zeitstempel, Wert) Value (current_timestamp(), 40);  
Insert Into SmartWindow.Humidity_MAX(Zeitstempel) Value (current_timestamp());
Insert Into SmartWindow.Humidity_MIN(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.Humidity_OUT(Zeitstempel, Wert) Value (current_timestamp(), 70);
Insert Into SmartWindow.ManClose(Zeitstempel, Wert) Value (current_timestamp(), FALSE); 
Insert Into SmartWindow.ManOpen(Zeitstempel, Wert) Value (current_timestamp(), FALSE);
Insert Into SmartWindow.Win_Open(Zeitstempel, Wert) Value (current_timestamp(), FALSE); 
Insert Into SmartWindow.Temp_IN(Zeitstempel, Wert) Value (current_timestamp(), 22); 
Insert Into SmartWindow.Temp_MAX(Zeitstempel) Value (current_timestamp());  
Insert Into SmartWindow.Temp_MIN(Zeitstempel) Value (current_timestamp());  
Insert Into SmartWindow.Temp_OUT(Zeitstempel, Wert) Value (current_timestamp(), 4); 
Insert Into SmartWindow.Volume_IN(Zeitstempel, Wert) Value (current_timestamp(), 20);      
Insert Into SmartWindow.Volume_MAX(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.Volume_OUT(Zeitstempel, Wert) Value (current_timestamp(), 20);  


Commit;