/*
DHT Wiring

ESP32       DJT22
------------------
22          Data
5V          VCC
GND         GND
*/

struct DHTRESPONSE {
    float tempC;
    float humidP;
};

void setupDHT();
DHTRESPONSE getDHTValues();