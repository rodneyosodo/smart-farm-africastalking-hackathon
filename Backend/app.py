from flask import Flask, request
from flask_sqlalchemy import SQLAlchemy
import os
from dotenv import load_dotenv
import paho.mqtt.client as mqtt
from paho.mqtt.publish import single
import africastalking
from sqlalchemy import desc, asc
from datetime import datetime, date

app = Flask(__name__)
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
app.config['DEBUG'] = False
app.config['TESTING'] = False
app.config['SECRET_KEY'] = 'this-really-needs-to-be-changed'
app.config['SQLALCHEMY_DATABASE_URI'] = "postgresql://postgres:postgres@127.0.0.1:1001/mode"

db = SQLAlchemy(app)

from models import Humidity, Light, Temperature, Moisture
from utils import Functions
funcs = Functions()

@app.route('/', methods=['POST', 'GET'])
def callback():
    if request.method == "GET":
        return "Server is up"
    elif request.method == "POST":
        command = request.get_json()
        if command['topic'] == 'rodneyeris/demo2eris/temperature':
            temperature = float(command['payload'][:-1])
            temp = Temperature(temperature)
            temp.save()
            return "True"
        elif command['topic'] == 'rodneyeris/demo2eris/humidity':
            humidity = floatemperature = float(command['payload'][:-1])
            hum = Humidity(humidity)
            hum.save()
            return "True"
        elif command['topic'] == 'rodneyeris/demo2eris/light':
            light = floatemperature = float(command['payload'][:-1])
            lig = Light(light)
            lig.save()
            return "True"
        elif command['topic'] == 'rodneyeris/demo2eris/moisture':
            moisture = floatemperature = float(command['payload'][:-1])
            moi = Moisture(moisture)
            moi.save()
            return "True"
        else:
            return "False"
@app.route("/ussd", methods = ['GET', 'POST'])
def ussd():
    session_id   = request.values.get("sessionId", None)
    serviceCode  = request.values.get("serviceCode", None)
    phone_number = request.values.get("phoneNumber", None)
    text         = request.values.get("text", "default")

    if text      == '':
        response  = "CON What would you want to check \n"
        response += "1. Get current data \n"
        response += "2. Get today's average data \n"
        response += "3. Get any day's average data \n"
        response += "4. Send commands"

    elif text    == '1':
        response  = "CON Choose information you want to view \n"
        response += "1. Temperature \n"
        response += "2. Humidity \n"
        response += "3. Light intensity \n"
        response += "4. Soil Moisture \n"
        response += "5. Combined"

    elif text   == '2':
        temp  = funcs.today_data()['temperature']
        humidity = funcs.today_data()['humidity']
        light = funcs.today_data()['light']
        moisture = funcs.today_data()['moisture']
        response       = "END The today's average data is\nTemperature: {}\nHumidity: {}\nSoil moisture: {}\nLight intensity: {}".format(temp, humidity, light, moisture)


    elif text   == '3':
        response = "CON Enter the day, month, year e.g 27,2,2020"
 
    elif text   == '4':
        response = "CON Irrigation action \n"
        response += "1. Start irrigation \n"
        response += "2. Stop irrigation"

    elif text          == '1*1':
        temp  = funcs.latest_data()['temperature']
        response       = "END The current temperature is " + str(temp)

    elif text          == '1*2':
        humidity  = funcs.latest_data()['humidity']
        response       = "END The current humidity is " + str(humidity)

    elif text          == '1*3':
        light  = funcs.latest_data()['light']
        response       = "END The current light intensity is " + str(light)

    elif text          == '1*4':
        moisture  = funcs.latest_data()['moisture']
        response       = "END The current soil moisture is " + str(moisture)

    elif text          == '1*5':
        temp  = funcs.latest_data()['temperature']
        humidity = funcs.latest_data()['humidity']
        light = funcs.latest_data()['light']
        moisture = funcs.latest_data()['moisture']
        response       = "END The current data is\nTemperature: {}\nHumidity: {}\nSoil moisture: {}\nLight intensity: {}".format(temp, humidity, light, moisture)
    
    elif text.__contains__(","):
        date_data = funcs.get_date_data(text)
        response = "END {} average is {}".format(text[2:], date_data)


    elif text    == '4*1':
        funcs.start_irrigation()
        response = "END Irrigation has started "

    elif text    == '4*2':
        funcs.stop_irrigation()
        response = "END Irrigation has stoped "

    else :
        response = "END Invalid choice"

    return response


if __name__ == '__main__':
    app.run()
