import os
from dotenv import load_dotenv
import paho.mqtt.client as mqtt
from paho.mqtt.publish import single
import africastalking
from datetime import datetime, date
from models import Humidity, Light, Temperature, Moisture


class Functions:
    def __init__(self):
        load_dotenv(dotenv_path="broker.env")
        self.broker_url = os.getenv("MQTT_HOST")
        self.broker_port = int(os.getenv("MQTT_PORT"))
        self.broker_username = os.getenv("MQTT_USERNAME")
        self.broker_password = os.getenv("MQTT_PASSWORD")
        self.client_id = os.getenv("CLIENT_ID")
        self.username = os.getenv("AT_USERNAME")
        self.api_key = os.getenv("AT_PASSWORD")
        self.number = os.getenv("FARMER_NUMBER")
        africastalking.initialize(self.username, self.api_key)
        self.sms = africastalking.SMS

    def start_irrigation(self):
        # starts irrigation
        single("rodneyeris/demo2eris/relay",
               payload='on',
               qos=1,
               retain=False,
               hostname=self.broker_url,
               port=self.broker_port,
               client_id=self.client_id,
               keepalive=60,
               will=None,
               auth={
                   'username': self.broker_username,
                   'password': self.broker_password
               },
               tls=None,
               protocol=mqtt.MQTTv311,
               transport="tcp")

    def stop_irrigation(self):
        # Stops irrigation
        single("rodneyeris/demo2eris/relay",
               payload='off',
               qos=1,
               retain=False,
               hostname=self.broker_url,
               port=self.broker_port,
               client_id=self.client_id,
               keepalive=60,
               will=None,
               auth={
                   'username': self.broker_username,
                   'password': self.broker_password
               },
               tls=None,
               protocol=mqtt.MQTTv311,
               transport="tcp")

    def send_alert(self, data, topic, level):
        # Sends and sms alert to farmers number
        message = "Your {} level is {}".format(topic, level)
        self.sms.send_sms(message, [self.number])

    @staticmethod
    def get_date_data(date_info):
        # Get data for and individual date
        today = date(date_info[7:], date_info[5:6], date_info[2:4])
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
            "humidity": humidity_av / count,
            "temperature": temperature_av / count,
            "light": light_av / count,
            "moisture": moisture_av / count
        }
        return average

    def check_extreme(self, data: float, topic: str):
        # Check for above optimum and below optimum
        if topic == "humidity":
            if data >= 10:
                self.send_alert(data, "humidity", "high")
                self.stop_irrigation()
            elif data <= 30:
                self.send_alert(data, "humidity", "low")
                self.start_irrigation()
        elif topic == "temperature":
            if data >= 10:
                self.send_alert(data, "temperature", "high")
                self.start_irrigation()
            elif data <= 10:
                self.send_alert(data, "temperature", "low")
                self.stop_irrigation()
        elif topic == "moisture":
            if data >= 1:
                self.send_alert(data, "moisture", "high")
                self.stop_irrigation()
            elif data <= 40:
                self.send_alert(data, "moisture", "low")
                self.start_irrigation()

    @staticmethod
    def latest_data():
        # Gets the latest entries into the database
        data = {
            "humidity": Humidity.query.all()[-1].humidity,
            "temperature": Temperature.query.all()[-1].temperature,
            "light": Light.query.all()[-1].light,
            "moisture": Moisture.query.all()[-1].moisture
        }
        return data

    @staticmethod
    def today_data():
        # Gets today's average data
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
            "humidity": humidity_av / count,
            "temperature": temperature_av / count,
            "light": light_av / count,
            "moisture": moisture_av / count
        }
        return average
