#include <TinyGPS++.h>

#define gpsUART Serial1
#define GPS_RX 12
#define GPX_TX 14

TinyGPSPlus gps;

void setupGPS() {
    gpsUART.begin(4800, SERIAL_8N1,GPS_RX,GPX_TX);
}

void readGPS() {
  while (gpsUART.available() > 0) {
    gps.encode(gpsUART.read());
  }
  //gpsSpeed = gps.speed.mph();
}