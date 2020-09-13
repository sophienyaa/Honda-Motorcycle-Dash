//LIBS
#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
//FONTS
#include <fonts/DS_DIGI16pt7b.h> //DS_DIGI 16pt
#include <fonts/DS_DIGI6pt7b.h> //DS_DIGI 6pt
#include <fonts/DS_DIGI60pt7b.h> //DS_DIGI 60pt
#include <fonts/Furara8pt7b.h> //Futara 8pt
//GFX
#include <gfx/hondaLogo.h> //honda logo for splash
#include <gfx/engineIcon.h> //check engine icon
#include <gfx/gpsIcon.h> //gps lock icon
#include <gfx/sidestand.h> //sidestand down icon
#include <gfx/engTemp.h> //eng temp high icon
#include <gfx/snow.h> //snow warning icon
#include <gfx/shiftLight.h> //shift warning icon
#include <gfx/lowBatt.h> //battery warning icon
#include <gfx/fanIcon.h> //fan on icon

//DISPLAY
#define TFT_DC 2
#define TFT_CS 5
#define TFT_RST 4
/* 
Display Wiring

ESP32       LCD
---------------
5V          VCC
GND	        GND
5	          CS
4	          RESET
2	          DC
23	        MOSI
18	        SCK
19	        MISO
*/

//SERIAL
#define debug Serial
#define bike Serial2
#define gpsUART Serial1
#define PACKET_BUFFER_SIZE (128)

/*
L9637D Wiring

ESP32       L9637D
------------------
17          TX
16          RX
3V3         VCC
GND         GND
*/
#define TX_PIN 17
#define RX_PIN 16

/*
GPS Wiring

ESP32       GPS
------------------
12          TX
14          RX
5V          VCC
GND         GND
*/
#define GPS_RX 12
#define GPX_TX 14

//DHT
/*
DHT Wiring

ESP32       DJT22
------------------
22          Data
5V          VCC
GND         GND
*/
#define DHT_PIN 22
#define DHT_TYPE DHT22

//Flag for debug mode, prints to UART0
#define DEBUG true
//Flag for bike, if false sets fake values on display for bench testing
#define BIKE false

//ECU CONSTANTS
byte ECU_WAKEUP_MESSAGE[] = {0xFE, 0x04, 0x72, 0x8C}; 
byte ECU_INIT_MESSAGE[] = {0x72, 0x05, 0x00, 0xF0, 0x99};
uint16_t ECU_SUCCESS_CHECKSUM = 0x6FB;

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

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHT_PIN, DHT_TYPE);
TinyGPSPlus gps;

uint16_t RPM_CONST = 310;
uint8_t TEMP_CONST = 10;

//GFX COLOURS
uint16_t DARK_GREEN  = 0x0200;


//METHODS
void debugValues();
void drawUIElements();
void drawRPMBar(int rpm);
void drawGear(int gear);
void drawTempBar(int temp);
void drawBattVolt(float volts);
void drawTPS(int tps);
void drawIATC(float iatc);
void drawAmbiTemp(float temp);
void drawRPMUI();
void drawGearIndBox();
void drawEngTempBar();
void getAmbiTempDHT();
void showDataTable11();
void showDataTableD1();
float calcValueDivide256(int val);
float calcValueMinus40(int val);
float calcValueDivide10(int val);
float calcValueDivide10(int val);
float calcValueDivide16(int val);
byte bufferECUResponse();
byte initHonda();
int initComms();
uint8_t calcChecksum(const uint8_t* data, uint8_t len);
void readGPS();
void drawIconBox();


void setup() {
  pinMode(DHT_PIN, INPUT);
  dht.begin();
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.drawBitmap(0,0, hondaLogo, 320, 240, ILI9341_WHITE);
  debug.begin(115200);
  gpsUART.begin(4800, SERIAL_8N1,GPS_RX,GPX_TX);
  initHonda();
  tft.fillScreen(ILI9341_BLACK);
  drawUIElements();
  delay(50);
}

void loop() {

  if(BIKE) {
    showDataTable11();
    showDataTableD1();
    getAmbiTempDHT();
    readGPS();
    debugValues();
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
  //drawSpeed(gpsSpeed);
  
  //delay(50);

}

void debugValues() {
  debug.print("RPM: ");
  debug.print(rpm);
  
  debug.print(" ENG C: ");
  debug.print(engTempC);
  
  debug.print(" AMBI C: ");
  debug.print(ambiC);
  
  debug.print(" BATT V: ");
  debug.print(battV);
  
  debug.print(" TPS %: ");
  debug.print(tpsP);
  
  debug.print(" IAT C: ");
  debug.print(iatC);
  
  debug.print(" GEAR: ");
  debug.print(gear);
  
  debug.print(" ENG: ");
  debug.print(engState);
  
  debug.print(" SWS: ");
  debug.print(switches);

  debug.print(" GSPD: ");
  debug.print(gpsSpeed);
  
  debug.println();
  
}

void drawUIElements() {
  //used for static elements
  drawRPMUI();
  drawGearIndBox();
  drawEngTempBar();
  drawIconBox();
}

void drawIconBox() {
    tft.setCursor(0,0);
    tft.drawBitmap(0,0, engineIcon, 32, 32, ILI9341_ORANGE);
    tft.drawBitmap(40,0, engTemp, 32, 32, DARK_GREEN);
    tft.drawBitmap(80,0, sidestand, 32, 32, DARK_GREEN);
    tft.drawBitmap(120,0, gpsIcon, 32, 32, DARK_GREEN);
    tft.drawBitmap(160,0, snow, 32, 32, DARK_GREEN);
    tft.drawBitmap(200,0, shiftLight, 32, 32, DARK_GREEN);
    tft.drawBitmap(240,0, lowBatt, 32, 32, DARK_GREEN);
    tft.drawBitmap(280,0, fanIcon, 32, 32, DARK_GREEN);
}

void drawRPMBar(int rpm) { //32 bars
  int numBars = rpm / RPM_CONST;
  
  for (int i = 0; i <= 32; i++) { //32 bars max
    if(i <= numBars) {
        if(i <= 20) {
          tft.fillRect(i*10,32,6,25, ILI9341_GREEN);
        }
        if(i > 20 && i <= 25) {
          tft.fillRect(i*10,32,6,25, ILI9341_YELLOW);
        }
        if(i > 25 && i <= 32) {
          tft.fillRect(i*10,32,6,25, ILI9341_RED);
        }
    }
    else {
      tft.fillRect(i*10,32,6,25, DARK_GREEN);
    }
  }   
  
  GFXcanvas16 canvas(90, 24);
  canvas.setTextColor(ILI9341_WHITE);
  canvas.setFont(&DS_DIGI16pt7b);
  canvas.setCursor(0,20);
  canvas.print(rpm);
  tft.drawRGBBitmap(240,72,canvas.getBuffer(),90,24);

  if(rpm > 9000) {
    tft.drawBitmap(200,0, shiftLight, 32, 32, ILI9341_RED);
  }
  else {
    tft.drawBitmap(200,0, shiftLight, 32, 32, DARK_GREEN);
  }

}

void drawGear(int gear) {

  String gearS;

  if(gear == 0) {
    gearS = "N";
  }
  else {
    gearS = gear;
  }
  
  GFXcanvas16 canvas(58, 78);
  canvas.setTextColor(ILI9341_WHITE);
  canvas.setFont(&DS_DIGI60pt7b);
  canvas.setCursor(0,75);
  canvas.print(gearS);
  tft.drawRGBBitmap(255,102,canvas.getBuffer(),58,78);
}

void drawTempBar(int temp) { // 12 bars

  int numBars = temp / TEMP_CONST;
  
  for (int i = 0; i <= 12; i++) { //12 bars max
    if(i <= numBars) {
        if(i < 9) {
          tft.fillRect(0,190-(i*10),20,6, ILI9341_GREEN);
        }
        if(i == 9) {
          tft.fillRect(0,190-(i*10),20,6, ILI9341_YELLOW);
        }
        if(i > 9 && i <= 12) {
          tft.fillRect(0,190-(i*10),20,6, ILI9341_RED);
        }
    }
    else {
      tft.fillRect(0,190-(i*10),20,6, DARK_GREEN);
    }

  }   
  
  GFXcanvas16 canvas(40, 24);
  canvas.setTextColor(ILI9341_WHITE);
  canvas.setFont(&DS_DIGI16pt7b);
  canvas.setCursor(0,20);
  canvas.print(temp);
  tft.drawRGBBitmap(0,200,canvas.getBuffer(),40,24);

  //Show Temp Warning
  if(temp > 105) {
    tft.drawBitmap(40,0, engTemp, 32, 32, ILI9341_RED);
  }
  else {
    tft.drawBitmap(40,0, engTemp, 32, 32, DARK_GREEN);
  }
}

void drawBattVolt(float volts) {
  GFXcanvas16 canvas(50, 22);
  canvas.setTextColor(ILI9341_WHITE);
  canvas.setFont(&DS_DIGI16pt7b);
  canvas.setCursor(0,20);
  canvas.print(volts,1);
  tft.drawRGBBitmap(64,200,canvas.getBuffer(),50,22);

  //show low batt warning
  if(volts < 12) {
    tft.drawBitmap(240,0, lowBatt, 32, 32, ILI9341_RED);
  }
  else {
    tft.drawBitmap(240,0, lowBatt, 32, 32, DARK_GREEN);
  }

}

void drawTPS(int tps) {  
  GFXcanvas16 canvas(50, 24);
  canvas.setTextColor(ILI9341_WHITE);
  canvas.setFont(&DS_DIGI16pt7b);
  canvas.setCursor(0,20);
  canvas.print(tps);
  tft.drawRGBBitmap(128,200,canvas.getBuffer(),50,24);
}

void drawIATC(float iatc) {
  GFXcanvas16 canvas(50, 22);
  canvas.setTextColor(ILI9341_WHITE);
  canvas.setFont(&DS_DIGI16pt7b);
  canvas.setCursor(0,20);
  canvas.print(iatc,1);
  tft.drawRGBBitmap(192,200,canvas.getBuffer(),50,22);
}

void drawAmbiTemp(float temp) {
  GFXcanvas16 canvas(50, 24);
  canvas.setTextColor(ILI9341_WHITE);
  canvas.setFont(&DS_DIGI16pt7b);
  canvas.setCursor(0,20);
  canvas.print(temp,1);
  tft.drawRGBBitmap(256,200,canvas.getBuffer(),50,24);

  //show snow warning
  if(temp < 3) {
    tft.drawBitmap(160,0, snow, 32, 32, ILI9341_WHITE);
  }
  else {
    tft.drawBitmap(160,0, snow, 32, 32, DARK_GREEN);
  }
}

void drawRPMUI() {

  //bar under gauge
  tft.drawFastHLine(0,59,250,ILI9341_WHITE);
  tft.drawFastHLine(0,60,250,ILI9341_WHITE);
  tft.drawFastHLine(251,59,70,ILI9341_RED);
  tft.drawFastHLine(251,60,70,ILI9341_RED);

  //text for numbers under bar
  tft.setFont();
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
  tft.setTextSize(1);
  
  int cursorPos = 0;
  for (int i = 0; i <= 9; i++) { 
    cursorPos = cursorPos+32;
    int gaugeNum = i+1;  
    
    if(gaugeNum >= 8) { //red text for 8-10k
        tft.setTextColor(ILI9341_RED,ILI9341_BLACK);
    }
    if(gaugeNum == 1) { // bring 1 in by 2px
      cursorPos = cursorPos - 2;
    }
    if(gaugeNum == 10) { // bring 10 in by 8px
      cursorPos = cursorPos - 10;
    }
    
    tft.setCursor(cursorPos,62);
    tft.print(gaugeNum);
  }
}

void drawGearIndBox() {

  tft.setTextColor(ILI9341_WHITE);
  tft.setFont(&Furara8pt7b);
  tft.setCursor(264,194);
  tft.print("GEAR");
  
}

void drawEngTempBar() {

  tft.drawFastVLine(24,110,86,ILI9341_WHITE);
  tft.drawFastVLine(25,110,86,ILI9341_WHITE);
  tft.fillRect(25,194,4,2, ILI9341_WHITE);
  tft.fillRect(25,110,4,2, ILI9341_WHITE);
  
  tft.drawFastVLine(24,70,26,ILI9341_RED);
  tft.drawFastVLine(25,70,26,ILI9341_RED);
  tft.fillRect(25,70,4,2, ILI9341_RED);
  tft.fillRect(25,94,4,2, ILI9341_RED);
  
  tft.setFont(&Furara8pt7b);
  tft.setCursor(0,236);
  tft.print("TEMP °C");

  tft.setCursor(64,236);
  tft.print("BATT V");

  tft.setCursor(128,236);
  tft.print("TPS %");

  tft.setCursor(192,236);
  tft.print("IAT °C");

  tft.setCursor(256,236);
  tft.print("AMBI °C");
}

void getAmbiTempDHT() {
  ambiC = dht.readTemperature();
}

//Shows most other info
void showDataTable11() {

  byte data[] = {0x72, 0x05, 0x71, 0x11};
  int chk = calcChecksum(data, sizeof(data));
  byte dataWithChk[] = {0x72, 0x05, 0x71, 0x11, chk};

  bike.write(dataWithChk, sizeof(dataWithChk));
  delay(50);
  
  int buffCount = 0;
  byte buff[PACKET_BUFFER_SIZE];  
  while ( (bike.available() > 0 ) && ( buffCount < PACKET_BUFFER_SIZE)) {
    buff[buffCount++] = bike.read();
  }

  /*debug.print("RAW: ");
    for(int i=0; i<buffCount; i++) {
    debug.print(buff[i], HEX);
    if(i != buffCount-1) {
        debug.print(";");  
    }
  }
  debug.println();*/

  rpm = (uint16_t) (buff[9] << 8) + buff[10];
  tpsP = calcValueDivide16(buff[12]);
  engTempC = calcValueMinus40(buff[14]);
  iatC = calcValueMinus40(buff[16]);
  battV = calcValueDivide10(buff[21]);
  int spdk = buff[22];
  int tpsv = calcValueDivide256(buff[11]);
  int mapv = calcValueDivide256(buff[17]);
  int mapk = buff[18];
  int iatv = calcValueDivide256(buff[15]);
  int ectv = calcValueDivide256(buff[13]);

}

//Shows info like neutral switch, engine on
void showDataTableD1() { 
  byte data[] = {0x72, 0x05, 0x71, 0xD1};
  int chk = calcChecksum(data, sizeof(data));
  byte dataWithChk[] = {0x72, 0x05, 0x71, 0xD1, chk};
  
  bike.write(dataWithChk, sizeof(dataWithChk));
  delay(50);
  
  int buffCount = 0;
  byte buff[PACKET_BUFFER_SIZE];  
  while ( (bike.available() > 0 ) && ( buffCount < PACKET_BUFFER_SIZE)) {
  buff[buffCount++] = bike.read();
  } 
  
  /*debug.print("RAW: ");
  for(int i=0; i<buffCount; i++) {
    debug.print(buff[i], HEX);
    if(i != buffCount-1) {
        debug.print(";");  
    }
  }
  debug.println();*/
  
  engState = buff[13];
  switches = buff[9];
  
}

//Calc methods
float calcValueDivide256(int val) {
  //convert to dec, multiple by 5, then divide result by 256
  //used for TPS Volt, ECT Volt, IAT Volt, MAP Volt
  
  return (val*5)/256;
}

float calcValueMinus40(int val) {
  //value minus 40
  //used for ECT Temp, IAT Temp
  
  return val-40;
}

float calcValueDivide10(int val) {
  //value divided by 10
  //used for Batt Volt
  
  return val/10;
}

float calcValueDivide16(int val) {
  //value divided by 16
  //used for TPS%
  
  return val/16;
}

byte bufferECUResponse() {
  
  int buffCount = 0;
  byte buff[PACKET_BUFFER_SIZE];
  byte sum = 0;
  
  while ( (bike.available() > 0 ) && ( buffCount < PACKET_BUFFER_SIZE)) {
    buff[buffCount++] = bike.read();
  }

  for(int i=0; i<buffCount; i++) {
    
    debug.print(buff[i], HEX);
    
    if(i != buffCount-1) {
        debug.print(";");  
    }

  }
  debug.println();  
}

byte initHonda() {
  //Honda ecu communication handshake

  int initSuccess = 0;

  //while(initSuccess = 0) {
    debug.println("Starting up...");
    debug.println("Setting line low 70ms, high 120ms");
    initComms();
    
    bike.begin(10400);
    debug.println("Sending ECU Wakeup");
    bike.write(ECU_WAKEUP_MESSAGE, sizeof(ECU_WAKEUP_MESSAGE));
    delay(200);
    debug.println("Sending ECU Init String");
    bike.write(ECU_INIT_MESSAGE, sizeof(ECU_INIT_MESSAGE));
    bike.flush();
    delay(50);

    int initBuffCount = 0;
    byte initBuff[32];
    while ( bike.available() > 0  && initBuffCount < 32 ) {
      initBuff[initBuffCount++] = bike.read();
    }

    int initSum = 0;
    for(int i=0; i<initBuffCount; i++) {
      initSum += initBuff[i];
    }
  
    if(initSum == ECU_SUCCESS_CHECKSUM) {
      debug.println("Successfully opened connection to ECU");
      initSuccess = 1;
      return 1;
    }
    else {
      debug.println("Failed to open connection to ECU, trying again in 2s");
      delay(2000);
    }
  //}
}

int initComms() {
  //Honda ECU Init sequence
  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, LOW); //TX Low for 70ms
  delay(70);
  digitalWrite(TX_PIN, HIGH); //TX High for 120ms
  delay(120);
  return 1;
}

uint8_t calcChecksum(const uint8_t* data, uint8_t len) {
   uint8_t cksum = 0;
   for (uint8_t i = 0; i < len; i++)
      cksum -= data[i];
   return cksum;
}

void readGPS() {
  while (gpsUART.available() > 0) {
    gps.encode(gpsUART.read());
  }

  debug.println(gps.time.value());
  gpsSpeed = gps.speed.mph();
 
}
