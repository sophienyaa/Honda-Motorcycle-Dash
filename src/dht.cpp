#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include "dht.h"

#define DHT_PIN 22
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

void setupDHT() {
    pinMode(DHT_PIN, INPUT);
    dht.begin();
}

DHTRESPONSE getDHTValues() {
    DHTRESPONSE response;
    response.tempC = dht.readTemperature();
    response.humidP = dht.readHumidity();
    return response;
}