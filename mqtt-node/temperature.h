#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#include "Arduino.h"
#include <lm75.h>                 //https://github.com/jlz3008/lm75

#ifdef MODULE_ENABLE_LM75
//  extern TempI2C_LM75 lm75Sensor;
  extern uint8_t lm75Address[MODULE_LM75_COUNT];
  extern TempI2C_LM75 lm75Sensors[MODULE_LM75_COUNT];
#endif

int16_t temperature;                // Temperature in 100*°C
unsigned long previousMeasurement;
inline void loopTemperature();
void measureTemperature();

#endif //TEMPERATURE_H
