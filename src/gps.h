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
    //more to come
};

void setupGPS();
GPSRESPONSE readGPS();