from flask import Flask
from flask import render_template
from flask import request
import MySQLdb


connection = MySQLdb.connect("raspberrypi","root","1234","SmartWindow")

cursor = connection.cursor()

Ins = ("INSERT INTO ")
Val = ("(Zeitstempel, Wert) VALUES (current_timestamp, ")

app = Flask(__name__)

@app.route("/")
def Start():
    return render_template('Startseite.html')

@app.route("/links")
def Links():
    return render_template('Links.html')

@app.route("/main")
def Main():
    cursor.execute("SELECT Wert FROM AirPressure_IN WHERE ID=(SELECT Max(ID) FROM AirPressure_IN)")
    InPres = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM AirPressure_OUT WHERE ID=(SELECT Max(ID) FROM AirPressure_OUT)")
    OutPres = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM AirQuality_IN WHERE ID=(SELECT Max(ID) FROM AirQuality_IN)")
    InQual = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM AirQuality_OUT WHERE ID=(SELECT Max(ID) FROM AirQuality_OUT)")
    OutQual = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM AutoModus WHERE ID=(SELECT Max(ID) FROM AutoModus)")
    if (cursor.fetchone())[0]:
        Modus = "automatic"
        Manu = " "
        Auto = "disabled"
    else:
        Modus = "manuel"
        Manu = "disabled"
        Auto = " "
    cursor.execute("SELECT Wert FROM Humidity_IN WHERE ID=(SELECT Max(ID) FROM Humidity_IN)")
    InHum = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Humidity_OUT WHERE ID=(SELECT Max(ID) FROM Humidity_OUT)")
    OutHum = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Temp_IN WHERE ID=(SELECT Max(ID) FROM Temp_IN)")
    InTemp = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Temp_OUT WHERE ID=(SELECT Max(ID) FROM Temp_Out)")
    OutTemp = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Volume_IN WHERE ID=(SELECT Max(ID) FROM Volume_IN)")
    InSound = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Volume_OUT WHERE ID=(SELECT Max(ID) FROM Volume_OUT)")
    OutSound = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Win_Open WHERE ID=(SELECT Max(ID) FROM Win_Open)")
    if (cursor.fetchone())[0]:
        StateWindow = "opend"
        ManOpen = "disabled"
        ManClose = " "
    else:
        StateWindow = "closed"
        ManOpen = " "
        ManClose = "disabled"
    return render_template('Main.html', Modus=Modus, StateWindow=StateWindow, ManOpen=ManOpen, ManClose=ManClose, Manu = Manu, Auto = Auto, OutTemp=OutTemp, InTemp=InTemp, OutHum=OutHum, InHum=InHum, InQual=InQual, OutQual=OutQual, InPres=InPres, OutPres=OutPres, InSound=InSound, OutSound=OutSound)


@app.route("/titel")
def Titel():
    return render_template('Titel.html')


@app.route("/discription")
def Discription():
    return render_template('Discription.html')


@app.route("/discriptionText")
def DiscriptionText():
    return render_template('Discription-Text.html')


@app.route("/limits")
def Limits():
    return render_template('Limits.html')


@app.route("/limitsText")
def LimitsText():
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
    return render_template('Limits-Text.html', MinTemp = MinTemp, MaxTemp = MaxTemp, MinHum = MinHum, MaxHum = MaxHum, MaxQual = MaxQual, MaxPres = MaxPres, MaxSound =MaxSound)

@app.route("/submit", methods=['POST'])
def submit():
    MinTemp=request.form['MinTemp']
    MaxTemp=request.form['MaxTemp']
    MinHum=request.form['MinHum']
    MaxHum=request.form['MaxHum']
    MaxQual=request.form['MaxCO2']
    MaxPres=request.form['MaxPres']
    MaxSound=request.form['MaxNoise']
    cursor.execute(Ins + "Temp_MIN" + Val + MinTemp +")")
    cursor.execute(Ins + "Temp_MAX" + Val + MaxTemp +")")
    cursor.execute(Ins + "Humidity_MIN" + Val + MinHum +")")
    cursor.execute(Ins + "Humidity_MAX" + Val + MaxHum +")")
    cursor.execute(Ins + "AirQuality_MAX" + Val + MaxQual +")")
    cursor.execute(Ins + "AirPressure_MAX" + Val + MaxPres +")")
    cursor.execute(Ins + "Volume_MAX" + Val + MaxSound +")")
    connection.commit()
    return LimitsText()

@app.route("/reset")
def reset():
    MinTemp="20.0"
    MaxTemp="26.0"
    MinHum="30.0"
    MaxHum="65.0"
    MaxAir="0.1"
    MaxWind="20.8"
    MaxSound="60.0"
    cursor.execute(Ins + "Temp_MIN" + Val + MinTemp +")")
    cursor.execute(Ins + "Temp_MAX" + Val + MaxTemp +")")
    cursor.execute(Ins + "Humidity_MIN" + Val + MinHum +")")
    cursor.execute(Ins + "Humidity_MAX" + Val + MaxHum +")")
    cursor.execute(Ins + "AirQuality_MAX" + Val + MaxQual +")")
    cursor.execute(Ins + "AirPressure_MAX" + Val + MaxPres +")")
    cursor.execute(Ins + "Volume_MAX" + Val + MaxSound +")")
    connection.commit()
    return LimitsText()

@app.route("/open")
def open():
    cursor.execute("SELECT Wert FROM AutoModus WHERE ID=(SELECT Max(ID) FROM AutoModus)")
    Modus = (cursor.fetchone())[0]
    if not (Modus):
        cursor.execute(Ins + "ManOpen" + Val + "1" +")")
        cursor.execute(Ins + "ManClose" + Val + "0" +")")
        connection.commit()
    return Main()


@app.route("/close")
def close():
    cursor.execute("SELECT Wert FROM AutoModus WHERE ID=(SELECT Max(ID) FROM AutoModus)")
    Modus = (cursor.fetchone())[0]
    if not (Modus):
        cursor.execute(Ins + "ManOpen" + Val + "0" +")")
        cursor.execute(Ins + "ManClose" + Val + "1" +")")
        connection.commit()
    return Main()

@app.route("/manu")
def manu():
    cursor.execute(Ins + "AutoModus" + Val + "0" +")")
    connection.commit()
    return Main()

@app.route("/auto")
def auto():
    cursor.execute(Ins + "AutoModus" + Val + "1" +")")
    cursor.execute(Ins + "ManOpen" + Val + "0" +")")
    cursor.execute(Ins + "ManClose" + Val + "0" +")")
    connection.commit()
    return Main()


if __name__ == "__main__":
    app.run(host='192.168.0.3', port=80)