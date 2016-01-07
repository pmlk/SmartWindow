### Webserver ###

# created: 20.10.2015
# Autor: Mailina Lohmann
# Content:   initializing the webserver and the connection to the database,
#            opening HTML files,
#            reading and writing values of the database,
#            sending and extracting values to the HTML pages

# Rev: 0.0 - 23.10.2015 first creation
#      1.0 - 18.12.2015 comment adjustment

from flask import Flask
from flask import render_template
from flask import request
import MySQLdb

# Help-Strings for inserting values into the database
Ins = ("INSERT INTO ")
Val = ("(Zeitstempel, Wert) VALUES (current_timestamp, ")

# Connection to the database
connection = MySQLdb.connect("localhost","root","1234","SmartWindow")
cursor = connection.cursor()

app = Flask(__name__)

# All tamplates are found in /templates
# Starting the website construction for HOME
@app.route("/")
def Start():
    return render_template('Home-Construction.html')

# Header HTML
@app.route("/titel")
def Titel():
    return render_template('Header.html')

# Link HTML
@app.route("/links")
def Links():
    return render_template('Links.html')

# Home HTML
@app.route("/main")
def Main():
    connection.commit()          # resetting the database connection
    # getting current values out of the database
    cursor.execute("SELECT Wert FROM AirPressure_IN WHERE ID=(SELECT Max(ID) FROM AirPressure_IN)")
    InPres = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM AirPressure_OUT WHERE ID=(SELECT Max(ID) FROM AirPressure_OUT)")
    OutPres = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM AirQuality_IN WHERE ID=(SELECT Max(ID) FROM AirQuality_IN)")
    InQual = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM AirQuality_OUT WHERE ID=(SELECT Max(ID) FROM AirQuality_OUT)")
    OutQual = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Win_Open WHERE ID=(SELECT Max(ID) FROM Win_Open)")
    if (cursor.fetchone())[0]:   # if the window is opend, show 'opend' and disable the button 'Open'
        StateWindow = "opend"
        ManOpen = "disabled"
        ManClose = " "
    else:                        # else show 'closed' and disable the button 'Close'
        StateWindow = "closed"
        ManOpen = " "
        ManClose = "disabled"
    cursor.execute("SELECT Wert FROM AutoModus WHERE ID=(SELECT Max(ID) FROM AutoModus)")
    if (cursor.fetchone())[0]:   # if the mode is automatic, show 'automatic' and disable the button 'Automatic', 'Open' and 'Close'
        Modus = "automatic"
        Manu = " "
        Auto = "disabled"
        ManOpen = "disabled"
        ManClose = "disabled"    # else show 'manual' and disable the button 'Manual'
    else:
        Modus = "manual"
        Manu = "disabled"
        Auto = " "
    cursor.execute("SELECT Wert FROM Humidity_IN WHERE ID=(SELECT Max(ID) FROM Humidity_IN)")
    InHum = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Humidity_OUT WHERE ID=(SELECT Max(ID) FROM Humidity_OUT)")
    OutHum = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Temp_IN WHERE ID=(SELECT Max(ID) FROM Temp_IN)")
    InTemp = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Temp_OUT WHERE ID=(SELECT Max(ID) FROM Temp_OUT)")
    OutTemp = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Volume_IN WHERE ID=(SELECT Max(ID) FROM Volume_IN)")
    InSound = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Volume_OUT WHERE ID=(SELECT Max(ID) FROM Volume_OUT)")
    OutSound = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Priority WHERE ID=(SELECT Max(ID) FROM Volume_OUT)")
    Prio = (cursor.fetchone())[0]
    if (Prio==3):                # Displaying priority in words
        Priority = "Air Quality, Temperature and Humidity"
    elif (Prio==2):
        Priority = "Air Quality"
    elif (Prio==1):
        Priority = "Temperature"
    elif (Prio==0):
        Priority = "Humidity"
    # execute the HTML file with the values of the database
    return render_template('Home.html', Modus=Modus, StateWindow=StateWindow, ManOpen=ManOpen, ManClose=ManClose, Manu = Manu, Auto = Auto, OutTemp=OutTemp, InTemp=InTemp, OutHum=OutHum, InHum=InHum, InQual=InQual, OutQual=OutQual, InPres=InPres, OutPres=OutPres, InSound=InSound, OutSound=OutSound, Priority = Priority)

# Bottom 'AUTOMATIC'
@app.route("/auto")
def auto():
    # set the mode to automatic and reset the open and close value
    cursor.execute(Ins + "AutoModus" + Val + "1" +")")
    cursor.execute(Ins + "ManOpen" + Val + "0" +")")
    cursor.execute(Ins + "ManClose" + Val + "0" +")")
    connection.commit()  # commiting the values to the database
    return Main()        # executing Home HTML

# Bottom 'MANUAL'
@app.route("/manu")
def manu():
    # set the mode to manual
    cursor.execute(Ins + "AutoModus" + Val + "0" +")")
    connection.commit() # commiting the values to the database
    return Main()       # executing Home HTML

# Bottom 'OPEN'
@app.route("/open")
def open():
    # request current mode
    cursor.execute("SELECT Wert FROM AutoModus WHERE ID=(SELECT Max(ID) FROM AutoModus)")
    Modus = (cursor.fetchone())[0]
    if not (Modus):          # if the mode is not automatic, send a close and cancel open
        cursor.execute(Ins + "ManOpen" + Val + "1" +")")
        cursor.execute(Ins + "ManClose" + Val + "0" +")")
        connection.commit()  # commiting the values to the database
    return Main()            # executing Home HTML

# Bottom 'CLOSE'
@app.route("/close")
def close():
    # request current mode
    cursor.execute("SELECT Wert FROM AutoModus WHERE ID=(SELECT Max(ID) FROM AutoModus)")
    Modus = (cursor.fetchone())[0]
    if not (Modus):          # if the mode is not automatic, send a close and cancel open
        cursor.execute(Ins + "ManOpen" + Val + "0" +")")
        cursor.execute(Ins + "ManClose" + Val + "1" +")")
        connection.commit()  # commiting the values to the database
    return Main()            # executing Home HTML



# Starting the website construction for SETTINGS
@app.route("/settings")
def Limits():
    return render_template('Settings-Construction.html')

# Settings HTML
@app.route("/settingsText")
def LimitsText():
    # getting current values out of the database
    cursor.execute("SELECT Wert FROM AirPressure_MAX WHERE ID=(SELECT Max(ID) FROM AirPressure_MAX)")
    MaxPres = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM AirQuality_MAX WHERE ID=(SELECT Max(ID) FROM AirQuality_MAX)")
    MaxQual = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Humidity_MAX WHERE ID=(SELECT Max(ID) FROM Humidity_MAX)")
    MaxHum = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Humidity_MIN WHERE ID=(SELECT Max(ID) FROM Humidity_MIN)")
    MinHum = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Temp_MAX WHERE ID=(SELECT Max(ID) FROM Temp_MAX)")
    MaxTemp = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Temp_MIN WHERE ID=(SELECT Max(ID) FROM Temp_MIN)")
    MinTemp = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Volume_MAX WHERE ID=(SELECT Max(ID) FROM Volume_MAX)")
    MaxSound = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Priority WHERE ID=(SELECT Max(ID) FROM Volume_MAX)")
    Prio = (cursor.fetchone())[0]
    if (Prio == 0):          # if the Priority is on Humidity, 'Humidity' should be selected
        prio0 = "seleced"
        prio1 = " "
        prio2 = " "
        prio3 = " "
    elif (Prio == 1):        # if the Priority is on Temperature, 'Temperature' should be selected
        prio0 = " "
        prio1 = "seleced"
        prio2 = " "
        prio3 = " "
    elif (Prio == 2):        # if the Priority is on Air Quality, 'Air Quality' should be selected
        prio0 = " "
        prio1 = " "
        prio2 = "seleced"
        prio3 = " "
    elif (Prio == 3):        # if the Priority is on all three, 'All three' should be selected
        prio0 = " "
        prio1 = " "
        prio2 = " "
        prio3 = "seleced"
    # execute the HTML file with the values of the database
    return render_template('Settings.html', MinTemp = MinTemp, MaxTemp = MaxTemp, MinHum = MinHum, MaxHum = MaxHum, MaxQual = MaxQual, MaxPres = MaxPres, MaxSound =MaxSound, prio0 = prio0, prio1 = prio1, prio2 = prio2, prio3 = prio3)


# Bottom 'SUBMIT', submitting the new limit values
@app.route("/submit", methods=['POST'])
def submit():
    # loading the values out of the HTML Page
    MinTemp=request.form['MinTemp']
    MaxTemp=request.form['MaxTemp']
    MinHum=request.form['MinHum']
    MaxHum=request.form['MaxHum']
    MaxQual=request.form['MaxCO2']
    MaxPres=request.form['MaxPres']
    MaxSound=request.form['MaxNoise']
    Prio=request.form['Priority']
    # executing the database INSERT command
    cursor.execute(Ins + "Temp_MIN" + Val + MinTemp +")")
    cursor.execute(Ins + "Temp_MAX" + Val + MaxTemp +")")
    cursor.execute(Ins + "Humidity_MIN" + Val + MinHum +")")
    cursor.execute(Ins + "Humidity_MAX" + Val + MaxHum +")")
    cursor.execute(Ins + "AirQuality_MAX" + Val + MaxQual +")")
    cursor.execute(Ins + "AirPressure_MAX" + Val + MaxPres +")")
    cursor.execute(Ins + "Volume_MAX" + Val + MaxSound +")")
    cursor.execute(Ins + "Priority" + Val + Prio + ")")
    cursor.execute(Ins + "LimitChange" + Val + "1)")
    connection.commit()   # commiting the values to the database
    return LimitsText()   # executing Limits HTML

# Bottom 'RESET', resetting the limit values to the old values
@app.route("/reset")
def reset():
    # loading the old values
    MinTemp="20.0"
    MaxTemp="26.0"
    MinHum="30.0"
    MaxHum="65.0"
    MaxQual="0.1"
    MaxPres="1000"
    MaxSound="60.0"
    Prio="3"
    # executing the database INSERT command
    cursor.execute(Ins + "Temp_MIN" + Val + MinTemp +")")
    cursor.execute(Ins + "Temp_MAX" + Val + MaxTemp +")")
    cursor.execute(Ins + "Humidity_MIN" + Val + MinHum +")")
    cursor.execute(Ins + "Humidity_MAX" + Val + MaxHum +")")
    cursor.execute(Ins + "AirQuality_MAX" + Val + MaxQual +")")
    cursor.execute(Ins + "AirPressure_MAX" + Val + MaxPres +")")
    cursor.execute(Ins + "Volume_MAX" + Val + MaxSound +")")
    cursor.execute(Ins + "Priority" + Val + Prio + ")")
    cursor.execute(Ins + "LimitChange" + Val + "1)")
    connection.commit()    # commiting the values to the database
    return LimitsText()    # executing Limits HTML



# Starting the website construction for DISCRIPTION
@app.route("/discription")
def Discription():
    return render_template('Discription-Construction.html')

# Discription HTML
@app.route("/discriptionText")
def DiscriptionText():
    return render_template('Discription.html')


# executing this python code as main funciton
if __name__ == "__main__":
    app.run(host='169.254.160.79', port=80)