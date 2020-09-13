#include <TinyGPS++.h>
#include "gps.h"

#define gpsUART Serial1
#define GPS_RX 12
#define GPX_TX 14

TinyGPSPlus gps;

void setupGPS() {
    gpsUART.begin(4800, SERIAL_8N1,GPS_RX,GPX_TX);
}

GPSRESPONSE readGPS() {
  while (gpsUART.available() > 0) {
    gps.encode(gpsUART.read());
  }
  GPSRESPONSE response;
  //response.speedKPH = gps.speed.mph;
  //response.speedMPH = gps.speed.mph;
  return response;
}