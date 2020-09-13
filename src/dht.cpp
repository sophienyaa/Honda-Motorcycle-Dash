#include <DHT.h>
#include <DHT_U.h>

#define DHT_PIN 22
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

void setupDHT() {
    pinMode(DHT_PIN, INPUT);
    dht.begin();
}

float getAmbiTempDHT() {
  return dht.readTemperature();
}

float getAmbiHumidDHT() {
  return dht.readHumidity();
}