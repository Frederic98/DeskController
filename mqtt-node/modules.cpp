#include "Arduino.h"
#include "config.h"

#ifdef MODULE_ENABLE_PCA9685
  #include <Adafruit_PWMServoDriver.h>
  Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(MODULE_PCA9685_ADDRESS);
#endif

#ifdef MODULE_ENABLE_MPR121
  #include "Adafruit_MPR121.h"
  Adafruit_MPR121 mpr121 = Adafruit_MPR121();
#endif

void initModules(){
#ifdef MODULE_ENABLE_MPR121
  mpr121.begin(MODULE_MPR121_ADDRESS);
  pinMode(MODULE_MPR121_IRQ, INPUT_PULLUP);
#endif

#ifdef MODULE_ENABLE_PCA9685
  pca9685.begin();
  pca9685.setPWMFreq(1600);
#endif
}
