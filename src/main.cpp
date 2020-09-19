#include <Arduino.h>
#include "utils.h"
#include "graphics.h"
#include "ecu.h"
#include "dht.h"
#include "gps.h"

//Flag for debug mode, prints to UART0
#define DEBUG true
//Flag for bike, if false sets fake values on display for bench testing
#define BIKE true

int ecuConnected = 0;

void printDebugValues();

void setup() {
  setupDHT();
  setupGPS();
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

  if(BIKE) {
    table11 = showDataTable11();
    tableD1 = showDataTableD1();
    dhtValues = getDHTValues();
    gpsValues = readGPS();
  }
  else {
    //set some values for testing display
    table11.rpm = 10000;
    table11.ectTemp = 120;
    dhtValues.tempC = 2.5;
    table11.battVolts = 11.9;
    table11.tpsPercent = 69;
    table11.iatTemp = 45;
    gpsValues.speedMPH = 100;
    table11.speedKPH = 105;
    tableD1.engState = 1;
    tableD1.switchState = 3;
  }
  
  //draw code
  drawRPMBar(table11.rpm);
  drawTempBar(table11.ectTemp);
  drawAmbiTemp(dhtValues.tempC);
  drawBattVolt(table11.battVolts);
  drawTPS(table11.tpsPercent);
  drawIATC(table11.iatTemp);
  drawSidestandIcon(tableD1.switchState);
  drawEngineIcon(tableD1.engState);
  drawGPSIcon(gpsValues.gpsLock);
  drawSpeed(gpsValues.speedKPH, table11.speedKPH);
  drawCardinal(gpsValues.cardinalDirection);

  //TODO: Implement below methods, set to 0 so show as off
  //drawGear(0);
  //drawFanIcon(0);

  if(DEBUG) {
    printDebugValues();
  }
}

void printDebugValues() {
  //TODO: this
}