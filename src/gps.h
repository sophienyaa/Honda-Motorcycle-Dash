/*
GPS Wiring

ESP32       GPS
------------------
12          TX
14          RX
5V          VCC
GND         GND
*/

struct GPSRESPONSE {
    double speedMPH;
    double speedKPH;
    int gpsLock;
    const char* cardinalDirection;
};

void setupGPS();
GPSRESPONSE readGPS();