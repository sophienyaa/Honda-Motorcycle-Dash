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

void setupUI();
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
void setupDisplay();
void drawIconBox();