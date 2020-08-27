# from flask import Flask, request
# # import paho.mqtt.client as mqtt
# # from dotenv import load_dotenv
# # import os

# # load_dotenv(dotenv_path="broker.env")

# # broker_url = os.getenv("MQTT_HOST")
# # broker_port = int(os.getenv("MQTT_PORT"))
# # mqtt_topic = os.getenv("MQTT_TOPIC")

# # client = mqtt.Client()
# # client.connect(broker_url, broker_port)

# app = Flask(__name__)


# @app.route('/', methods=['POST', 'GET'])
# def publish():
#     if request.method == "GET":
#         return "Server is up"
#     elif request.method == "POST":
#         command = request.get_json()
#         print(command)
#         return "True"


# if __name__ == "__main__":
#     app.run(port=5000)
from flask import Flask, request
from flask_sqlalchemy import SQLAlchemy
import os
from dotenv import load_dotenv
import paho.mqtt.client as mqtt
from paho.mqtt.publish import single
import africastalking
from sqlalchemy import desc, asc
from datetime import datetime, date


load_dotenv(dotenv_path="broker.env")

broker_url = os.getenv("MQTT_HOST")
broker_port = int(os.getenv("MQTT_PORT"))
broker_username = os.getenv("MQTT_USERNAME")
broker_password = os.getenv("MQTT_PASSWORD")
client_id = os.getenv("CLIENT_ID")

app = Flask(__name__)
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
app.config['DEBUG'] = False
app.config['TESTING'] = False
app.config['SECRET_KEY'] = 'this-really-needs-to-be-changed'
app.config['SQLALCHEMY_DATABASE_URI'] = "postgresql://postgres:postgres@127.0.0.1:1001/mode"


db = SQLAlchemy(app)

from models import Humidity, Light, Temperature, Moisture

username = os.getenv("AT_USERNAME")
api_key = os.getenv("AT_PASSWORD")
number = os.getenv("FARMER_NUMBER")
africastalking.initialize(username, api_key)
sms = africastalking.SMS

def start_irrigation():
    single("rodneyeris/demo2eris/relay", payload='on', qos=1, retain=False,
            hostname=broker_url, port=broker_port, client_id=client_id,
            keepalive=60, will=None, auth={'username':broker_username, 'password':broker_password},
            tls=None, protocol=mqtt.MQTTv311, transport="tcp")

def stop_irrigation():
    single("rodneyeris/demo2eris/relay", payload='off', qos=1, retain=False,
        hostname=broker_url, port=broker_port, client_id=client_id,
        keepalive=60, will=None, auth={'username':broker_username, 'password':broker_password},
        tls=None, protocol=mqtt.MQTTv311, transport="tcp")

def send_alert(data, topic, level):
    global number
    message = "Your {} level is {}".format(topic, level)
    sms.send_sms(message, [number])

def get_date_data(date):
    date = date(date[7:], date[5:6], date[2:4])
    return "True"

def check_extreme(data: float, topic: str):
    if topic == "humidity":
        if data >= 10:
            send_alert(data, "humidity", "high")
            stop_irrigation()
        elif data <= 30:
            send_alert(data, "humidity", "low")
            start_irrigation()
    elif topic == "temperature":
        if data >= 10:
            send_alert(data, "temperature", "high")
            start_irrigation()
        elif data <= 10:
            send_alert(data, "temperature", "low")
            stop_irrigation()
    elif topic == "moisture":
        if data >= 1:
            send_alert(data, "moisture", "high")
            stop_irrigation()
        elif data <= 40:
            send_alert(data, "moisture", "low")
            start_irrigation()

def latest_data():
    data = {
        "humidity" : Humidity.query.all()[-1].humidity,
        "temperature" : Temperature.query.all()[-1].temperature,
        "light" : Light.query.all()[-1].light,
        "moisture" : Moisture.query.all()[-1].moisture
    }
    return data

def today_data():
    today = date.today()
    humidity = Humidity.query.all()
    temperature = Temperature.query.all()
    light = Light.query.all()
    moisture = Moisture.query.all()
    count = 0
    humidity_av = 0
    temperature_av = 0
    moisture_av = 0
    light_av = 0
    print(humidity[0].updated_on.date == today)
    for i in humidity:
        if i.updated_on.date == today:
            humidity_av += i.humidity
            print(humidity_av)
            count += 1
    for i in temperature:
        if i.updated_on.date == today:
            temperature_av += i.temperature
    for i in light:
        if i.updated_on.date == today:
            light_av += i.light
    for i in moisture:
        if i.updated_on.date == today:
            moisture_av += i.moisture
    if count == 0:
        count = 1
    average = {
        "humidity" : humidity_av/count,
        "temperature" : temperature_av/count,
        "light" : light_av/count,
        "moisture" : moisture_av/count
    }
    return average

@app.route('/', methods=['POST', 'GET'])
def callback():
    if request.method == "GET":
        data = Humidity.query.all()[-1]
        print(data)
        # today_data()
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
        temp  = today_data()['temperature']
        humidity = today_data()['humidity']
        light = today_data()['light']
        moisture = today_data()['moisture']
        response       = "END The today's average data is\nTemperature: {}\nHumidity: {}\nSoil moisture: {}\nLight intensity: {}".format(temp, humidity, light, moisture)


    elif text   == '3':
        response = "CON Enter the day, month, year e.g 27,2,2020"
 
    elif text   == '4':
        response = "CON Irrigation action \n"
        response += "1. Start irrigation \n"
        response += "2. Stop irrigation"

    elif text          == '1*1':
        temp  = latest_data()['temperature']
        response       = "END The current temperature is " + str(temp)

    elif text          == '1*2':
        humidity  = latest_data()['humidity']
        response       = "END The current humidity is " + str(humidity)

    elif text          == '1*3':
        light  = latest_data()['light']
        response       = "END The current light intensity is " + str(light)

    elif text          == '1*4':
        moisture  = latest_data()['moisture']
        response       = "END The current soil moisture is " + str(moisture)

    elif text          == '1*5':
        temp  = latest_data()['temperature']
        humidity = latest_data()['humidity']
        light = latest_data()['light']
        moisture = latest_data()['moisture']
        response       = "END The current data is\nTemperature: {}\nHumidity: {}\nSoil moisture: {}\nLight intensity: {}".format(temp, humidity, light, moisture)
    
    elif text.__contains__(","):
        date_data = get_date_data(text)
        response = "END {} average is {}".format(text[2:], date_data)


    elif text    == '4*1':
        start_irrigation()
        response = "END Irrigation has started "

    elif text    == '4*2':
        stop_irrigation()
        response = "END Irrigation has stoped "

    else :
        response = "END Invalid choice"

    return response

# @app.route("/voice", methods = ['GET', 'POST'])
# def voice():
#     session_id   = request.values.get("sessionId", None)
#     isActive  = request.values.get("isActive", None)
#     phone_number = request.values.get("callerNumber", None)

#     response = '<Response> <GetDigits timeout="30" finishOnKey="#">'
#     response += '<Say voice="man" playBeep="false">Please enter your account '
#     response += 'number followed by the hash sign</Say> </GetDigits> </Response>'

#     dtmfDigits = request.values.get("dtmfDigits", None)

#     if dtmfDigits == '1234':
#         response = '<Response> <GetDigits timeout="30" finishOnKey="#">'
#         response +=' <Say voice="man" playBeep="false"> Press 1 followed by a hash '
#         response +='sign to get your account balance or 0 followed by a hash sign to'
#         response += ' quit</Say> </GetDigits></Response>'

#     elif dtmfDigits == '1':
#         response = '<Response>'
#         response += '<Say voice="man" playBeep="false" >Your balance is 1234 Shillings</Say>'
#         response+= '<Reject/> </Response>'

#     elif dtmfDigits == '0':
#         response = '<Response>'
#         response += '<Say voice="man" playBeep="false" >Its been a pleasure, good bye </Say>'
#         response+= '<Reject/> </Response>'

#     return response

if __name__ == '__main__':
    app.run()
