#include <Arduino.h>
#include "utils.h"
#include "ecu.h"

#define bike Serial2
#define debug Serial
#define PACKET_BUFFER_SIZE (128)

#define TX_PIN 17
#define RX_PIN 16

byte ECU_WAKEUP_MESSAGE[] = {0xFE, 0x04, 0x72, 0x8C}; 
byte ECU_INIT_MESSAGE[] = {0x72, 0x05, 0x00, 0xF0, 0x99};
uint16_t ECU_SUCCESS_CHECKSUM = 0x6FB;

void wakeupECU() {
  //Honda ECU Init sequence
  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, LOW); //TX Low for 70ms
  delay(70);
  digitalWrite(TX_PIN, HIGH); //TX High for 120ms
  delay(120);
}

byte setupECU(bool debugMode) {
    //Honda ecu communication handshake
    if (debugMode) {
        debug.begin(115200);
        debug.println("Starting up...");
        debug.println("Setting line low 70ms, high 120ms");
    }

    wakeupECU();
    bike.begin(10400);

    if (debugMode) {
        debug.println("Sending ECU Wakeup");
    }

    bike.write(ECU_WAKEUP_MESSAGE, sizeof(ECU_WAKEUP_MESSAGE));
    delay(200);

    if(debugMode) {
        debug.println("Sending ECU Init String");
    }

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
        if(debugMode) {
            debug.println("Successfully opened connection to ECU");
        }
        return 1;
    }
    else {
        if(debugMode) {
            debug.println("Failed to open connection to ECU, trying again in 2s");
        }
        return 0;
    }
}

//Shows most other info
TABLE11RESPONSE showDataTable11() {

  byte data[] = {0x72, 0x05, 0x71, 0x11};
  byte chk = calcChecksum(data, sizeof(data));
  byte dataWithChk[] = {0x72, 0x05, 0x71, 0x11, chk};

  bike.write(dataWithChk, sizeof(dataWithChk));
  delay(50);
  
  int buffCount = 0;
  byte buff[PACKET_BUFFER_SIZE];  
  while ( (bike.available() > 0 ) && ( buffCount < PACKET_BUFFER_SIZE)) {
    buff[buffCount++] = bike.read();
  }

    TABLE11RESPONSE response;

    response.rpm = (uint16_t) (buff[9] << 8) + buff[10];
    response.tpsPercent = calcValueDivide16(buff[12]);
    response.ectTemp = calcValueMinus40(buff[14]);
    response.iatTemp = calcValueMinus40(buff[16]);
    response.battVolts = calcValueDivide10(buff[21]);
    response.speed = buff[22];
    response.tpsVolts = calcValueDivide256(buff[11]);
    response.mapVolts = calcValueDivide256(buff[17]);
    response.mapPressure = buff[18];
    response.iatVolts = calcValueDivide256(buff[15]);
    response.ectVolts = calcValueDivide256(buff[13]);

    return response;
}

//Shows info like neutral switch, engine on
TABLED1RESPONSE showDataTableD1() { 

    byte data[] = {0x72, 0x05, 0x71, 0xD1};
    byte chk = calcChecksum(data, sizeof(data));
    byte dataWithChk[] = {0x72, 0x05, 0x71, 0xD1, chk};

    bike.write(dataWithChk, sizeof(dataWithChk));

    delay(50);

    int buffCount = 0;
    byte buff[PACKET_BUFFER_SIZE];  
    while ( (bike.available() > 0 ) && ( buffCount < PACKET_BUFFER_SIZE)) {
    buff[buffCount++] = bike.read();
    } 

    TABLED1RESPONSE response;

    response.engState = buff[13];
    response.switchState = buff[9];
    return response;
  
}

byte bufferECUResponse() {
  
  int buffCount = 0;
  byte buff[PACKET_BUFFER_SIZE];
  
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
  return 0;
}