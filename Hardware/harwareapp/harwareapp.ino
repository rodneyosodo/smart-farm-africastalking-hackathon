#include <DHT.h>
#define DHTTYPE DHT11

int loop_time = 5000;
const int DHT_PIN = PA6;
const int soilMoisturePin = PB1;

float humidity, temperature, soilMoistureValue;

DHT dht(DHT_PIN, DHTTYPE);

void readTemp(void);
void readHum(void);
void readSoilMoisture(void);

void setup()
{
    Serial.begin(9600);
    Serial.println("Starting DHT11");
    pinMode(soilMoisturePin, INPUT);
    dht.begin();
}

void loop()
{
    readTemp();
    readHum();
    readSoilMoisture();
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
    // Read temperatire ad Celsius(default)
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
    // Map value : 400 will be 100 and 1023 will be 0
    Serial.print("Soil moisture content: ");
    Serial.println(soilMoistureValue);
}
