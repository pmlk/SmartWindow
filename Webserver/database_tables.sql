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

 -- Drop Befehle Neue Tabellen

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
Drop table SmartWindow.Wind_OUT;
Drop table SmartWindow.Wind_MAX;
Drop table SmartWindow.Win_Open;
Drop table SmartWindow.Temp_IN;
Drop table SmartWindow.Temp_MAX;
Drop table SmartWindow.Temp_MIN;
Drop table SmartWindow.Temp_OUT;
Drop table SmartWindow.Volume_IN;
Drop table SmartWindow.Volume_MAX;
Drop table SmartWindow.Volume_OUT;
Drop table SmartWindow.LimitChange;
Drop table SmartWindow.Air_Quality_AlarmState;
Drop table SmartWindow.Humidity_AlarmState;
Drop table SmartWindow.Temp_AlarmState;
Drop table SmartWindow.Volume_AlarmState;
Drop table SmartWindow.Wind_AlarmState;
Drop table SmartWindow.Priority;



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
    
-- Signal, dass die Limits sich geändert haben 0: keine neuen Änderungen 1: neue Änderungen
Create table SmartWindow.LimitChange(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		bool not null);	
    
-- Priorität 0: Humidity, 1: Temperature 2: Quality 3: All    
 Create table SmartWindow.Priority(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 3); 
    
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
Create table SmartWindow.Humidity_OUT(
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
            
-- Luftdruck innen in Pa
Create table SmartWindow.AirPressure_OUT(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int not null);
 
-- Luftdruck außen in Pa
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
    
-- Windgeschwindigkeit außen in m/s
Create table SmartWindow.Wind_OUT(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float);  
    
    
    -- Limits zur Berechnung
    
-- Minimale Temperatur in Grad Celsius
Create table SmartWindow.Temp_MIN(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 20);
    
-- Maximale Temperatur in Grad Celsius
Create table SmartWindow.Temp_MAX(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 26);
    
-- Minimale  Luftfeuchtigkeit in %
Create table SmartWindow.Humidity_MIN(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 30);
    
-- Maximale Luftfeuchtigkeit in %
Create table SmartWindow.Humidity_MAX(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 65);
    
-- Maximales CO2 in Vol.%
Create table SmartWindow.AirQuality_MAX(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 0);
    
-- Maximaler Luftdruck in Pa
Create table SmartWindow.AirPressure_MAX(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 1020);
    
-- Maximale Lautstärke in db
Create table SmartWindow.Volume_MAX(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 60);

-- Maximale Windgeschwindigkeit in m/s
Create table SmartWindow.Wind_MAX(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 21);
    
    
    -- Alarme
  
-- Air_Quality_Alarm (0: OK, 1: Warning, 2:Alarm)
 Create table SmartWindow.Air_Quality_AlarmState(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 0);   
    
-- Humidity_Alarm (0: OK, 1: Warning, 2:Alarm)
 Create table SmartWindow.Humidity_AlarmState(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 0);    
  
-- Temp_Alarm (0: OK, 1: Warning, 2:Alarm)
 Create table SmartWindow.Temp_AlarmState(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 0);   
      
-- Volume_Alarm (0: OK, 1: Warning, 2:Alarm)
 Create table SmartWindow.Volume_AlarmState(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 0);   
    
-- Wind_Alarm (0: OK, 1: Warning, 2:Alarm)
 Create table SmartWindow.Wind_AlarmState(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		int default 0);      
    

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
Insert Into SmartWindow.Wind_OUT(Zeitstempel, Wert) Value (current_timestamp(), 0); 
Insert Into SmartWindow.Wind_MAX(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.Temp_IN(Zeitstempel, Wert) Value (current_timestamp(), 22); 
Insert Into SmartWindow.Temp_MAX(Zeitstempel) Value (current_timestamp());  
Insert Into SmartWindow.Temp_MIN(Zeitstempel) Value (current_timestamp());  
Insert Into SmartWindow.Temp_OUT(Zeitstempel, Wert) Value (current_timestamp(), 4); 
Insert Into SmartWindow.Volume_IN(Zeitstempel, Wert) Value (current_timestamp(), 20);      
Insert Into SmartWindow.Volume_MAX(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.Volume_OUT(Zeitstempel, Wert) Value (current_timestamp(), 20);
Insert Into SmartWindow.Air_Quality_AlarmState(Zeitstempel) Value (current_timestamp());
Insert Into SmartWindow.Humidity_AlarmState(Zeitstempel) Value (current_timestamp());
Insert Into SmartWindow.Temp_AlarmState(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.Volume_AlarmState(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.Wind_AlarmState(Zeitstempel, Wert) Value (current_timestamp(), 1);
Insert Into SmartWindow.Priority(Zeitstempel) Value (current_timestamp());


Commit;