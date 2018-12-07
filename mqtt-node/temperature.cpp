#include "Arduino.h"
#include <lm75.h>
#include "config.h"
#include "mqtt.h"
#include "temperature.h"

#ifdef MODULE_ENABLE_LM75
  uint8_t lm75Address[MODULE_LM75_COUNT];
  
  #if MODULE_LM75_RESOLUTION == 9
    #define MODULE_LM75_RESOLUTION_SETTING TempI2C_LM75::nine_bits
  #elif MODULE_LM75_RESOLUTION == 10
    #define MODULE_LM75_RESOLUTION_SETTING TempI2C_LM75::ten_bits
  #elif MODULE_LM75_RESOLUTION == 11
    #define MODULE_LM75_RESOLUTION_SETTING TempI2C_LM75::eleven_bits
  #elif MODULE_LM75_RESOLUTION == 12
    #define MODULE_LM75_RESOLUTION_SETTING TempI2C_LM75::twelve_bits
  #endif
//  TempI2C_LM75 lm75Sensor = TempI2C_LM75(0x48, MODULE_LM75_RESOLUTION_SETTING);
  TempI2C_LM75 lm75Sensors[MODULE_LM75_COUNT];
#endif

void loopTemperature(){
  unsigned long now = millis();
  if(now - previousMeasurement >= TEMPERATURE_MEASURE_INTERVAL){
    measureTemperature();
  }
}

void measureTemperature(){
#ifdef MODULE_ENABLE_LM75
  temperature = lm75Sensor.getTemp() * 100;
  publishUInt16("temperature/sensor1/value", temperature);
#endif
}
