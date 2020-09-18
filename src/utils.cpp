#include <Arduino.h>
#include "utils.h"

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
  //value divided by 16 and times 10
  //used for TPS%
  return (val/16)*10;
}

uint8_t calcChecksum(const uint8_t* data, uint8_t len) {
   uint8_t cksum = 0;
   for (uint8_t i = 0; i < len; i++)
      cksum -= data[i];
   return cksum;
}

float calcKPHtoMPH(int val) {
  return val / 1.609344;
}
