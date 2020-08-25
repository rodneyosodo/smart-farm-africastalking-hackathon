#define TINY_GSM_MODEM_SIM800

#include <DHT.h>
#include <TinyGsmClient.h>
#include <Countdown.h>
#include <IPStack.h>
#include <MQTTClient.h>
#include <MQTTPacket.h>

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

// set GSM PIN, if any
#define GSM_POWER_KEY PB15

int loop_time = 5000;
int delay_time = 2000;
const int DHT_PIN = PA6;
const int soilMoisturePin = PB1;
const int ldrPin = PB0;
#define relayPin PC13

// Your GPRS credentials, if any
const char apn[] = "iot.safaricom.com";
const char gprsUser[] = "";
const char gprsPass[] = "";
char buffer[100];
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

DHT dht(DHT_PIN, DHTTYPE);
TinyGsm modem(SerialAT);
TinyGsmClient tinyGSMClient(modem);
IPStack ipstack(tinyGSMClient);
MQTT::Client<IPStack, Countdown, 128, 2> mqttClient = MQTT::Client<IPStack, Countdown, 128, 2>(ipstack);

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

void setup()
{
    SerialMon.begin(115200);
    delay(delay_time);
    SerialAT.begin(115200);
    delay(delay_time);
    pinMode(soilMoisturePin, INPUT);
    pinMode(ldrPin, INPUT);

    pinMode(GSM_POWER_KEY, OUTPUT);
    pinMode(LED_PIN, OUTPUT);

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
    sendTemperature();
    sendHumidity();
    sendSoilMoisture();
    sendLightIntensity();
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
    Serial.print("Humidity: ");
    Serial.println(humidity);
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
    Serial.print("Temperature: ");
    Serial.println(temperature);
    return temperature;
}

float readSoilMoisture(void)
{
    // Read soil moisture
    soilMoistureValue = analogRead(soilMoisturePin);
    soilMoistureValue = map(soilMoistureValue, 0, 4095, 0, 100);
    // Map value : 0 will be 0 and 4095 will be 1000
    Serial.print("Soil moisture content: ");
    Serial.println(soilMoistureValue);
    return soilMoistureValue;
}

float readLightIntensity(void)
{
    // Read light intensity
    int ldrValue = analogRead(ldrPin);
    ldrValue = map(ldrValue, 0, 4095, 100, 0);
    // Map value : 0 will be 100 and 4095 will be 0
    Serial.print("Light intensity Value: ");
    Serial.println(ldrValue);
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
    snprintf(buffer, sizeof(buffer), "%i", ldrValue);
    publishMessage(buffer, lightIntensityTopic);
}
void sendSoilMoisture(void)
{
    soilMoistureValue = readSoilMoisture();
    snprintf(buffer, sizeof(buffer), "%.3f", soilMoistureValue);
    publishMessage(buffer, soilMoistureTopic);
}