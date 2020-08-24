#include <DHT.h>
#define DHTTYPE DHT11

int loop_time = 5000;
const int DHT_PIN = PA6;
const int soilMoisturePin = PB1;
const int ldrPin = PB0;

float humidity, temperature, soilMoistureValue, ldrValue;

DHT dht(DHT_PIN, DHTTYPE);

void readTemp(void);
void readHum(void);
void readSoilMoisture(void);
void readLightIntensity(void);

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
    // Wait for a few seconds to loop
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