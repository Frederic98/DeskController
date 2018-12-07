#ifndef MODULES_H
#define MODULES_H
#include "Arduino.h"
#include "config.h"

#ifdef MODULE_ENABLE_PCA9685
  #include <Adafruit_PWMServoDriver.h>
  extern Adafruit_PWMServoDriver pca9685;
#endif

#ifdef MODULE_ENABLE_MPR121
  #include "Adafruit_MPR121.h"
  extern Adafruit_MPR121 mpr121;
#endif

void initModules();

#endif //MODULES_H
