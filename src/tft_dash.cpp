#include <Arduino.h>
#include "utils.h"
#include "graphics.h"
#include "ecu.h"
#include "dht.h"
#include "gps.h"

//Flag for debug mode, prints to UART0
#define DEBUG true
//Flag for bike, if false sets fake values on display for bench testing
#define BIKE false

int ecuConnected = 0;

void setup() {
  setupDHT();
  setupDisplay();
  ecuConnected = setupECU(DEBUG);
  setupUI();
  delay(50);
}

void loop() {

TABLE11RESPONSE table11;
TABLED1RESPONSE tableD1;
DHTRESPONSE dhtValues;
GPSRESPONSE gpsValues;

  //GLOBAL VALUES TO DISPLAY
  uint16_t rpm = 0;
  int8_t engTempC = 0;
  float ambiC = 0;
  float battV = 0;
  uint8_t tpsP = 0;
  float iatC = 0;
  uint8_t gear = 0;
  uint8_t engState = 0;
  uint8_t switches = 0;
  uint16_t gpsSpeed = 0;
  uint16_t bikeSpeed = 0;

  if(BIKE) {
    table11 = showDataTable11();
    tableD1 = showDataTableD1();
    dhtValues = getDHTValues();
    gpsValues = readGPS();
  }
  else {
    //set some values for testing display
    rpm = 10000;
    engTempC = 120;
    ambiC = 2.5;
    battV = 11.9;
    tpsP = 69;
    iatC = 45;
    gear = 6;
    gpsSpeed = 100;
    bikeSpeed = 105;
  }
  
  //draw code
  drawRPMBar(rpm);
  drawTempBar(engTempC);
  drawAmbiTemp(ambiC);
  drawBattVolt(battV);
  drawTPS(tpsP);
  drawIATC(iatC);
  drawGear(gear);

  if(DEBUG) {
    //printDebugValues();
  }
}