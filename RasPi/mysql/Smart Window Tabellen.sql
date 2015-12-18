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

/* -- Drop Befehle
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

	-- Befehle

-- Modus 0: Manuel 1: Automatik
Create table SmartWindow.Modus(
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
Create table SmartWindow.StateWindow(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		bool not null);
    
-- Außentemperatur in Grad Celsius
Create table SmartWindow.OutTemp(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
    
-- Innentemperatur in Grad Celsius
Create table SmartWindow.InTemp(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
    
-- Luftfeuchtigkeit außen in %
Create table SmartWindow.OutHum(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
    
-- Luftfeuchtigkeit innen in %
Create table SmartWindow.InHum(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
    
-- Luftqualität in Vol. %
Create table SmartWindow.Air(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
      
-- Windgeschwindigkeit in m/s
Create table SmartWindow.Wind(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
        
-- Geräuschlevel in dB
Create table SmartWindow.Sound(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float not null);
    
    
    -- Limits zur Berechnung
    
-- Minimale Temperatur in Grad Celsius
Create table SmartWindow.MinTemp(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float default 20.0);
    
-- Maximale Temperatur in Grad Celsius
Create table SmartWindow.MaxTemp(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float default 26.0);
    
-- Minimale  Luftfeuchtigkeit in %
Create table SmartWindow.MinHum(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float default 30.0);
    
-- Maximale Luftfeuchtigkeit in %
Create table SmartWindow.MaxHum(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float default 65.0);
    
-- Maximales CO2 in Vol.%
Create table SmartWindow.MaxAir(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float default 0.1);
    
-- Maximale Windgeschwindigkeit in m/s
Create table SmartWindow.MaxWind(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float default 20.8);
    
-- Maximale Lautstärke in db
Create table SmartWindow.MaxSound(
	ID			INT primary key	Auto_increment,
    Zeitstempel	timestamp unique not null,
    Wert		float default 60.0);


	-- Anfangswerte aller Tabellen
Insert Into SmartWindow.Air(Zeitstempel, Wert) Value (current_timestamp(), 0.01);    
Insert Into SmartWindow.InHum(Zeitstempel, Wert) Value (current_timestamp(), 40); 
Insert Into SmartWindow.InTemp(Zeitstempel, Wert) Value (current_timestamp(), 22); 
Insert Into SmartWindow.ManClose(Zeitstempel, Wert) Value (current_timestamp(), FALSE); 
Insert Into SmartWindow.ManOpen(Zeitstempel, Wert) Value (current_timestamp(), FALSE); 
Insert Into SmartWindow.MaxAir(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.MaxHum(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.MaxSound(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.MaxWind(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.MaxTemp(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.MinHum(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.MinTemp(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.Modus(Zeitstempel) Value (current_timestamp()); 
Insert Into SmartWindow.OutHum(Zeitstempel, Wert) Value (current_timestamp(), 70); 
Insert Into SmartWindow.OutTemp(Zeitstempel, Wert) Value (current_timestamp(), 4); 
Insert Into SmartWindow.Sound(Zeitstempel, Wert) Value (current_timestamp(), 20); 
Insert Into SmartWindow.StateWindow(Zeitstempel, Wert) Value (current_timestamp(), FALSE); 
Insert Into SmartWindow.Wind(Zeitstempel, Wert) Value (current_timestamp(), 5.8); 


Commit;