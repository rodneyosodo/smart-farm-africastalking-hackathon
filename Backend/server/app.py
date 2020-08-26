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


app = Flask(__name__)
app.config.from_object(os.environ['APP_SETTINGS'])
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)

from models import Humidity, Light, Temperature, Moisture

@app.route('/<name>')
def hello_name(name):
    return "Hello {}!".format(name)

@app.route('/', methods=['POST', 'GET'])
def callback():
    if request.method == "GET":
        return "Server is up"
    elif request.method == "POST":
        command = request.get_json()
        if command['topic'] == 'rodneyeris/demo2eris/temperature':
            temperature = command['payload'][:-1]
            temp = Temperature(temperature)
            temp.save()
            return "True"
        elif command['topic'] == 'rodneyeris/demo2eris/humidity':
            humidity = command['payload'][:-1]
            hum = Humidity(humidity)
            hum.save()
            return "True"
        elif command['topic'] == 'rodneyeris/demo2eris/light':
            light = command['payload'][:-1]
            lig = Light(light)
            lig.save()
            return "True"
        elif command['topic'] == 'rodneyeris/demo2eris/moisture':
            moisture = command['payload'][:-1]
            moi = Moisture(moisture)
            moi.save()
            return "True"
        else:
            return "False"

if __name__ == '__main__':
    app.run()

# {'payload': '0\x00', 'topic': 'rodneyeris/demo2eris/distance'}
# {'payload': '26.800\x00', 'topic': 'rodneyeris/demo2eris/temperature'}
# {'payload': '71.000\x00', 'topic': 'rodneyeris/demo2eris/humidity'}
# {'payload': '2411\x00', 'topic': 'rodneyeris/demo2eris/light'}
# {'payload': '22.400\x00', 'topic': 'rodneyeris/demo2eris/temperature'}
# {'payload': '60.000\x00', 'topic': 'rodneyeris/demo2eris/humidity'}
# {'payload': '0.000\x00', 'topic': 'rodneyeris/demo2eris/moisture'}
# {'payload': '"on"', 'topic': 'rodneyeris/demo2eris/relay'}
# {'payload': 'on\n', 'topic': 'rodneyeris/demo2eris/relay'}
