#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Wire.h>
#include "SPI.h"

//FONTS
#include <fonts/DS_DIGI16pt7b.h> //DS_DIGI 16pt
#include <fonts/DS_DIGI6pt7b.h> //DS_DIGI 6pt
#include <fonts/DS_DIGI60pt7b.h> //DS_DIGI 60pt
#include <fonts/Furara8pt7b.h> //Futara 8pt

//GFX
#include "graphics.h"
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

uint16_t RPM_CONST = 310;
uint8_t TEMP_CONST = 10;

//GFX COLOURS
uint16_t DARK_GREEN  = 0x0200;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setupDisplay() {
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(ILI9341_BLACK);
    tft.drawBitmap(0,0, hondaLogo, 320, 240, ILI9341_WHITE);
}

void drawUIElements() {
    tft.fillScreen(ILI9341_BLACK);
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