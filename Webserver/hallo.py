from flask import Flask
from flask import render_template
app = Flask(__name__)

@app.route("/")
def hallo(name=None):
    return render_template('Startseite.html')

@app.route("/links")
def Links():
    return render_template('Links.html')

@app.route("/main")
def Main():
    return render_template('Main.html')


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
    return render_template('Limits-Text.html')

if __name__ == "__main__":
    app.run(host='192.168.0.3')

