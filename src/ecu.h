/*
L9637D Wiring

ESP32       L9637D
------------------
17          TX
16          RX
3V3         VCC
GND         GND
*/


struct TABLE11RESPONSE {

    uint16_t rpm;
    uint8_t speed;

    uint8_t tpsPercent;
    uint8_t tpsVolts;

    float ectTemp;
    uint8_t ectVolts;

    float iatTemp;
    uint8_t iatVolts;

    float battVolts;

    float mapPressure;
    uint8_t mapVolts;
};

struct TABLED1RESPONSE {
    uint8_t engState;
    uint8_t switchState;
};

TABLE11RESPONSE showDataTable11();
TABLED1RESPONSE showDataTableD1();
byte bufferECUResponse();
byte setupECU(bool debugMode);
void wakeupECU();