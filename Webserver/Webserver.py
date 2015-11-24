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
def hallo(name=None):
    return render_template('Startseite.html')

@app.route("/links")
def Links():
    return render_template('Links.html')

@app.route("/main")
def Main():
    cursor.execute("SELECT Wert FROM Air WHERE ID=(SELECT Max(ID) FROM Air)")
    Air = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM InHum WHERE ID=(SELECT Max(ID) FROM InHum)")
    InHum = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM InTemp WHERE ID=(SELECT Max(ID) FROM InTemp)")
    InTemp = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Modus WHERE ID=(SELECT Max(ID) FROM Modus)")
    Modus = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM OutHum WHERE ID=(SELECT Max(ID) FROM OutHum)")
    OutHum = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM OutTemp WHERE ID=(SELECT Max(ID) FROM OutTemp)")
    OutTemp = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM Sound WHERE ID=(SELECT Max(ID) FROM Sound)")
    Sound = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM StateWindow WHERE ID=(SELECT Max(ID) FROM StateWindow)")
    if (cursor.fetchone())[0]:
        StateWindow = "opend"
    else:
        StateWindow = "closed"
    cursor.execute("SELECT Wert FROM Wind WHERE ID=(SELECT Max(ID) FROM Wind)")
    Wind = (cursor.fetchone())[0]
    return render_template('Main.html', Modus=Modus, StateWindow=StateWindow, OutTemp=OutTemp, InTemp=InTemp, OutHum=OutHum, InHum=InHum, Air=Air, Wind=Wind, Sound=Sound)


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
    cursor.execute("SELECT Wert FROM MaxAir WHERE ID=(SELECT Max(ID) FROM MaxAir)")
    MaxAir = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM MaxHum WHERE ID=(SELECT Max(ID) FROM MaxHum)")
    MaxHum = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM MaxSound WHERE ID=(SELECT Max(ID) FROM MaxSound)")
    MaxSound = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM MaxTemp WHERE ID=(SELECT Max(ID) FROM MaxTemp)")
    MaxTemp = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM MaxWind WHERE ID=(SELECT Max(ID) FROM MaxWind)")
    MaxWind = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM MinHum WHERE ID=(SELECT Max(ID) FROM MinHum)")
    MinHum = (cursor.fetchone())[0]
    cursor.execute("SELECT Wert FROM MinTemp WHERE ID=(SELECT Max(ID) FROM MinTemp)")
    MinTemp = (cursor.fetchone())[0]
    return render_template('Limits-Text.html', MinTemp = MinTemp, MaxTemp = MaxTemp, MinHum = MinHum, MaxHum = MaxHum, MaxAir = MaxAir, MaxWind = MaxWind, MaxSound =MaxSound)

@app.route("/submit", methods=['POST'])
def submit():
    MinTemp=request.form['MinTemp']
    MaxTemp=request.form['MaxTemp']
    MinHum=request.form['MinHum']
    MaxHum=request.form['MaxHum']
    MaxAir=request.form['MaxCO2']
    MaxWind=request.form['MaxWind']
    MaxSound=request.form['MaxNoise']
    cursor.execute(Ins + "MinTemp" + Val + MinTemp +")")
    cursor.execute(Ins + "MaxTemp" + Val + MaxTemp +")")
    cursor.execute(Ins + "MinHum" + Val + MinHum +")")
    cursor.execute(Ins + "MaxHum" + Val + MaxHum +")")
    cursor.execute(Ins + "MaxAir" + Val + MaxAir +")")
    cursor.execute(Ins + "MaxWind" + Val + MaxWind +")")
    cursor.execute(Ins + "MaxSound" + Val + MaxSound +")")
    connection.commit()
    return render_template('Limits-Text.html', MinTemp = MinTemp, MaxTemp = MaxTemp, MinHum = MinHum, MaxHum = MaxHum, MaxAir = MaxAir, MaxWind = MaxWind, MaxSound =MaxSound)

@app.route("/reset")
def reset():
    MinTemp="20"
    MaxTemp="26"
    MinHum="30"
    MaxHum="65"
    MaxAir="0.1"
    MaxWind="20.8"
    MaxSound="60"
    cursor.execute(Ins + "MinTemp" + Val + MinTemp +")")
    cursor.execute(Ins + "MaxTemp" + Val + MaxTemp +")")
    cursor.execute(Ins + "MinHum" + Val + MinHum +")")
    cursor.execute(Ins + "MaxHum" + Val + MaxHum +")")
    cursor.execute(Ins + "MaxAir" + Val + MaxAir +")")
    cursor.execute(Ins + "MaxWind" + Val + MaxWind +")")
    cursor.execute(Ins + "MaxSound" + Val + MaxSound +")")
    connection.commit()
    return render_template('Limits-Text.html', MinTemp = MinTemp, MaxTemp = MaxTemp, MinHum = MinHum, MaxHum = MaxHum, MaxAir = MaxAir, MaxWind = MaxWind, MaxSound =MaxSound)


if __name__ == "__main__":
    app.run(host='192.168.0.3')