#include <TinyGPS++.h>
#include "gps.h"

#define gpsUART Serial1
#define GPS_RX 14
#define GPX_TX 12

TinyGPSPlus gps;

void setupGPS() {
    gpsUART.begin(4800, SERIAL_8N1,GPS_RX,GPX_TX);
}

GPSRESPONSE readGPS() {
  while (gpsUART.available() > 0) {
    gps.encode(gpsUART.read());
  }
  GPSRESPONSE response;
  response.speedKPH = gps.speed.kmph();
  response.speedMPH = gps.speed.mph();
  response.gpsLock = gps.sentencesWithFix() > 0 ? 1 : 0;
  response.cardinalDirection = gps.course.isValid() ? gps.cardinal(gps.course.deg()) : "---";

  return response;
}