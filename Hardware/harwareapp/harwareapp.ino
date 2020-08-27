// defining the  GSM module before importations
#define TINY_GSM_MODEM_SIM800

// IMPORTATION OF NECESSARY HEADER FILES

#include <DHT.h> // DHT library
#include <TinyGsmClient.h> // GSM library
#include <Countdown.h>
#include <IPStack.h>
#include <MQTTClient.h> // MQTT client library 
#include <MQTTPacket.h> // MQTT library to send data as packets
#include <Arduino.h>
#include <U8x8lib.h>

#include "./config.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// DEFINITION OF VARIABLES

#define DHTTYPE DHT11
// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT Serial2
// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon
// Define how you're planning to connect to the internet
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// The GSM module power is at PB15 and needs to be set HIGH for 3 seconds then LOW once
#define GSM_POWER_KEY PB15

// time before another iteration of the loop
int loop_time = 5000;
// short delay time
int delay_time = 2000;
// where the DHT data pin is located
const int DHT_PIN = PA6;
// where the Soil Moisture sensor data pin is located
const int soilMoisturePin = PB1;
// where the Light Dependent Resistor data pin is located
const int ldrPin = PB0;
// where the Solid state relay data pin is located
#define relayPin PA7

// Your GPRS credentials, if any
const char apn[] = "iot.safaricom.com";
const char gprsUser[] = "";
const char gprsPass[] = "";
// Buffer to stop messages when formating before publishing or printing
char buffer[100];
// Default return code
int returnCode = 0;
// BEGIN MQTT CONFIG

const char mqttUsername[] = "rodneyeris:demo2eris";
const char mqttPassword[] = "qwertyuiop123";
const char mqttDeviceID[] = "rodneylocalhost2";
#define TOPIC_PREFIX "rodneyeris/demo2eris/"

// birth topic - This is a topic that the device publishes to when it makes the first MQTT Connection
//  will topic - This is a topic that the device should publish to when  it disconnects.

const char *birthTopic = TOPIC_PREFIX "birth";
const char *willTopic = TOPIC_PREFIX "will";
const char *humidityTopic = TOPIC_PREFIX "humidity";
const char *soilMoistureTopic = TOPIC_PREFIX "moisture";
const char *temperatureTopic = TOPIC_PREFIX "temperature";
const char *relayTopic = TOPIC_PREFIX "relay";
const char *lightIntensityTopic = TOPIC_PREFIX "light";

const char birthMessage[] = "CONNECTED";
const char willMessage[] = "DISCONNECTED";

char brokerAddress[] = "broker.africastalking.com";
int brokerPort = 1883;

float humidity, temperature, soilMoistureValue, ldrValue;
// iterator variable
int r;

DHT dht(DHT_PIN, DHTTYPE);
TinyGsm modem(SerialAT);
TinyGsmClient tinyGSMClient(modem);
IPStack ipstack(tinyGSMClient);
MQTT::Client<IPStack, Countdown, 128, 2> mqttClient = MQTT::Client<IPStack, Countdown, 128, 2>(ipstack);
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8x8setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ PB8, /* data=*/ PB9, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display

float readTemp(void);
float readHum(void);
float readSoilMoisture(void);
float readLightIntensity(void);
void connectGSM(void);
void connectMqtt(void);
void publishMessage(char *payload, const char *topic);
void sendTemperature(void);
void sendHumidity(void);
void sendSoilMoisture(void);
void sendLightIntensity(void);
void incomingMessageHandler(MQTT::MessageData &messageData);
void pre(void);

void setup()
{
    u8x8.begin();
    u8x8.setPowerSave(0);
    SerialMon.begin(115200);
    delay(delay_time);
    SerialAT.begin(115200);
    delay(delay_time);
    pinMode(soilMoisturePin, INPUT);
    pinMode(ldrPin, INPUT);

    pinMode(GSM_POWER_KEY, OUTPUT);
    pinMode(relayPin, OUTPUT);

        // GSM ON
    digitalWrite(GSM_POWER_KEY, 1);
    delay(delay_time);
    digitalWrite(GSM_POWER_KEY, 0);

    connectGSM();
    connectMqtt();
    delay(delay_time);
    dht.begin();
}

void loop()
{
    pre();
    u8x8.print("AFRICASTALKING");
    u8x8.setCursor(0,2);
    u8x8.print("HACKATHON");

    delay(delay_time);
    sendTemperature();
    delay(delay_time);
    sendHumidity();
    delay(delay_time);
    sendSoilMoisture();
    delay(delay_time);
    sendLightIntensity();

    pre();
    u8x8.fillDisplay();
    for( r = 0; r < u8x8.getRows(); r++ )
    {
        u8x8.clearLine(r);
        delay(delay_time);
    }
    delay(delay_time);

    delay(loop_time);
    // Wait for a few seconds to loop again
}

float readHum(void)
{
    humidity = dht.readHumidity();
    // Check if reads failed and exit
    if (humidity == NULL){
        Serial.println("Failed to read from DHT");
        exit;
    }
    sprintf(buffer, "%.2f", humidity);
    SerialMon.print("Humidity: ");
    SerialMon.println(buffer);
    pre();
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_f); 
    u8x8.drawString(0, 2, "Humidity");
    u8x8.draw2x2String(0, 5, buffer);
    return humidity;
}


float readTemp(void)
{
    // Read temperature in Celsius(default)
    temperature = dht.readTemperature();
        // Check if reads failed and exit
    if (temperature == NULL){
        Serial.println("Failed to read from DHT");
        exit;
    }
    sprintf(buffer, "%.2f°C", temperature);
    SerialMon.print("Temperature: ");
    SerialMon.println(buffer);
    pre();
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_f); 
    u8x8.drawString(0, 2, "Temperature");
    u8x8.draw2x2String(0, 5, buffer);

    return temperature;
}

float readSoilMoisture(void)
{
    // Read soil moisture
    soilMoistureValue = analogRead(soilMoisturePin);
    soilMoistureValue = map(soilMoistureValue, 0, 4095, 0, 100);
    // Map value : 0 will be 0 and 4095 will be 1000

    sprintf(buffer, "%.2f", soilMoistureValue);
    SerialMon.print("Soil moisture: ");
    SerialMon.println(buffer);
    pre();
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_f); 
    u8x8.drawString(0, 2, "Soil moisture");
    u8x8.draw2x2String(0, 5, buffer);
    return soilMoistureValue;
}

float readLightIntensity(void)
{
    // Read light intensity
    ldrValue = analogRead(ldrPin);
    ldrValue = map(ldrValue, 0, 4095, 100, 0);
    // Map value : 0 will be 100 and 4095 will be 0
    sprintf(buffer, "%.2f", ldrValue);
    SerialMon.print("Light intensity: ");
    SerialMon.println(buffer);
    pre();
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_f); 
    u8x8.drawString(0, 2, "Light intensity");
    u8x8.draw2x2String(0, 5, buffer);

    return ldrValue;
}

void connectGSM(void)
{
    SerialMon.println("Getting the modem ready");
    modem.init();
    String name = modem.getModemName();
    String modemInfo = modem.getModemInfo();
    SerialMon.print("Name: ");
    SerialMon.println(name);
    SerialMon.print("Modem info: ");
    SerialMon.println(modemInfo);
    SerialMon.println("Initializing GSM network registration");
    if (!modem.waitForNetwork())
    {
        SerialMon.println("Unable to initialize registration. Reset and try again.");
        modem.restart();
        while (true)
        {
            ;
        }
    }
    if (modem.isNetworkConnected()) {
        SerialMon.println("Network connected");
        pre();
        u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
        u8x8.setCursor(0,4); 
        u8x8.print("Network: ");
        u8x8.setCursor(0,5); 
        u8x8.print("connected");
    }
    SerialMon.println("GSM OK");
    SerialMon.println("Attempting to establish GPRS connection");
    if (!modem.gprsConnect(apn, gprsUser, gprsPass))
    {
        SerialMon.println("Unable to connect to APN. Reset and try again");
        while (true)
        {
            ;
        }
    }
    if(modem.isGprsConnected())
    {
        SerialMon.println("GPRS status: connected" );
            pre();
            u8x8.setFont(u8x8_font_amstrad_cpc_extended_f); 
            u8x8.setCursor(0,4);
            u8x8.print("GPRS: ");
            u8x8.setCursor(0,5); 
            u8x8.print("connected");
    }

    String ccid = modem.getSimCCID();
    SerialMon.print("CCID: ");
    SerialMon.println(ccid);

    String imei = modem.getIMEI();
    SerialMon.print("IMEI: ");
    SerialMon.println(imei);

    String imsi = modem.getIMSI();
    SerialMon.print("IMSI: ");
    SerialMon.println(imsi);

    String cop = modem.getOperator();
    SerialMon.print("Operator: ");
    SerialMon.println(cop);

    IPAddress local = modem.localIP();
    SerialMon.print("Local IP: ");
    SerialMon.println(local);

    int csq = modem.getSignalQuality();
    SerialMon.print("Signal quality: ");
    SerialMon.println(csq);
}

void connectMqtt(void)
{
    MQTT::Message mqttMessage;
    snprintf(buffer, sizeof(buffer), "Connecting to %s on port %u\n", brokerAddress, brokerPort);
    SerialMon.println(buffer);
    returnCode = ipstack.connect(brokerAddress, brokerPort);
    if (returnCode != 1)
    {
        SerialMon.println("Unable to connect to TCP Port.");
    }
    else
    {
        SerialMon.println("TCP port open");
    }
    delay(delay_time);
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 4;
    data.clientID.cstring = (char *)mqttDeviceID;
    data.username.cstring = (char *)mqttUsername;
    data.password.cstring = (char *)mqttPassword;
    data.keepAliveInterval = 60;
    data.cleansession = 1;
    data.will.message.cstring = (char *)willMessage;
    data.will.qos = MQTT::QOS1;
    data.will.retained = 0;
    data.will.topicName.cstring = (char *)willTopic;
    returnCode = mqttClient.connect(data);
    if (returnCode != 0)
    {
        snprintf(buffer, sizeof(buffer), "Code %i. Unable to contact the broker. Is it up? Reset and try again \r\n", returnCode);
        SerialMon.print(buffer);
        while (true)
        {
            ;
        }
    }
    mqttMessage.qos = MQTT::QOS1;
    mqttMessage.retained = false;
    mqttMessage.dup = false;
    mqttMessage.payload = (void *)birthMessage;
    mqttMessage.payloadlen = strlen(birthMessage) + 1;
    returnCode = mqttClient.publish(birthTopic, mqttMessage);
    snprintf(buffer, sizeof(buffer), "Birth topic publish return code %i \n", returnCode);
    SerialMon.println(buffer);
    returnCode = mqttClient.subscribe(relayTopic, MQTT::QOS1, incomingMessageHandler);
    if (returnCode != 0)
    {
        snprintf(buffer, sizeof(buffer), "Unable to subscribe to relay topic \n");
        SerialMon.print(buffer);
        while (true)
        {
            ;
        }
    }
    SerialMon.println("Successfully connected to the broker");
    pre();
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_f); 
    u8x8.setCursor(0,4);
    u8x8.print("Broker: ");
    u8x8.setCursor(0,5); 
    u8x8.print("connected");
}

void publishMessage(char *payload, const char *topic)
{
    MQTT::Message message;
    message.qos = MQTT::QOS1;
    message.payload = (void *)payload;
    message.payloadlen = strlen(payload) + 1;
    returnCode = mqttClient.publish(topic, message);
    snprintf(buffer, sizeof(buffer), "%s topic publish return code %i", topic, returnCode);
    SerialMon.println(buffer);
}

void sendHumidity(void)
{
    humidity = readHum();
    snprintf(buffer, sizeof(buffer), "%.3f", humidity);
    publishMessage(buffer, humidityTopic);
}

void sendTemperature(void)
{
    temperature = readTemp();
    snprintf(buffer, sizeof(buffer), "%.3f", temperature);
    publishMessage(buffer, temperatureTopic);
}

void sendLightIntensity(void)
{
    ldrValue = readLightIntensity();
    snprintf(buffer, sizeof(buffer), "%.3f", ldrValue);
    publishMessage(buffer, lightIntensityTopic);
}
void sendSoilMoisture(void)
{
    soilMoistureValue = readSoilMoisture();
    snprintf(buffer, sizeof(buffer), "%.3f", soilMoistureValue);
    publishMessage(buffer, soilMoistureTopic);
}

void incomingMessageHandler(MQTT::MessageData &messageData)
{
    MQTT::Message &message = messageData.message;
    MQTTString topic = messageData.topicName;
    snprintf(buffer, sizeof(buffer), "%s", messageData.topicName);
    if (buffer == relayTopic)
    {
        snprintf(buffer, sizeof(buffer), "%s", messageData.message.payload);
        if (buffer == "on")
        {
            digitalWrite(relayPin, 1);
        }
        else if (buffer == "off")
        {
            digitalWrite(relayPin, 0);
        }
        else
        {
            SerialMon.print("Unknown LED Command: ");
            SerialMon.println(buffer);
        }
    }
    else
    {
        SerialMon.print("Unknown Topic: ");
        SerialMon.println(buffer);
    }
}

void pre(void)
{
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);    
    u8x8.clear();

    u8x8.inverse();
    u8x8.print(" SMART FARM    ");
    u8x8.setFont(u8x8_font_chroma48medium8_r);  
    u8x8.noInverse();
    u8x8.setCursor(0,1);
}