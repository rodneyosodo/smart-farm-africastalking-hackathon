#define TINY_GSM_MODEM_SIM800

#include <DHT.h>
#include <TinyGsmClient.h>
#include <Countdown.h>
#include <IPStack.h>

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
#define LED_PIN PC13

// Your GPRS credentials, if any
const char apn[] = "iot.safaricom.com";
const char gprsUser[] = "";
const char gprsPass[] = "";
char buffer[100];
int returnCode = 0;

float humidity, temperature, soilMoistureValue, ldrValue;

DHT dht(DHT_PIN, DHTTYPE);
TinyGsm modem(SerialAT);
TinyGsmClient tinyGSMClient(modem);

void readTemp(void);
void readHum(void);
void readSoilMoisture(void);
void readLightIntensity(void);
void connectGSM(void);

void setup()
{
    Serial.begin(9600);
    Serial.println("Starting DHT11");
    pinMode(soilMoisturePin, INPUT);
    pinMode(ldrPin, INPUT);
    dht.begin();
}

void loop()
{
    readTemp();
    readHum();
    readSoilMoisture();
    readLightIntensity();
    delay(loop_time);
    // Wait for a few seconds to loop again
}

void readHum(void)
{
    humidity = dht.readHumidity();
    // Check if reads failed and exit
    if (humidity == NULL){
        Serial.println("Failed to read from DHT");
        return;
    }
    Serial.print("Humidity: ");
    Serial.println(humidity);
}


void readTemp(void)
{
    // Read temperature in Celsius(default)
    temperature = dht.readTemperature();
        // Check if reads failed and exit
    if (temperature == NULL){
        Serial.println("Failed to read from DHT");
        return;
    }
    Serial.print("Temperature: ");
    Serial.println(temperature);
}

void readSoilMoisture(void)
{
    // Read soil moisture
    soilMoistureValue = analogRead(soilMoisturePin);
    soilMoistureValue = map(soilMoistureValue, 0, 4095, 0, 100);
    // Map value : 0 will be 0 and 4095 will be 1000
    Serial.print("Soil moisture content: ");
    Serial.println(soilMoistureValue);
}

void readLightIntensity(void)
{
    // Read light intensity
    int ldrValue = analogRead(ldrPin);
    ldrValue = map(ldrValue, 0, 4095, 100, 0);
    // Map value : 0 will be 100 and 4095 will be 0
    Serial.print("Light intensity Value: ");
    Serial.println(ldrValue);
}