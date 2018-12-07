#ifndef CONFIG_H
#define CONFIG_H
#include "Arduino.h"

#define DEBUG

#define HOSTNAME "ESP8266-light-controller"

#define MODULE_ENABLE_PCA9685               // 16 channel PWM driver
#define MODULE_ENABLE_MPR121                // 12 channel touch input
//#define MODULE_ENABLE_LM75                  // I2C temperature sensor
#define MODULE_ENABLE_SERIAL                // Enable serial port for communication to other device

#ifdef MODULE_ENABLE_PCA9685
  #define MODULE_PCA9685_ADDRESS 0x40       // I2C address of PCA9685
#endif

#ifdef MODULE_ENABLE_MPR121
  #define MODULE_MPR121_IRQ D0              // MPR121 IRQ pin
  #define MODULE_MPR121_ADDRESS 0x5A        // I2C address of MMPR121
#endif

#ifdef MODULE_ENABLE_LM75
  #define MODULE_LM75_ADDRESS {0x48}          // I2C address of LM75
  #define MODULE_LM75_UPDATE_TIME 5         // Publish new temperature every x seconds
  #define MODULE_LM75_RESOLUTION 11         // Resolution for LM75A ic, valid values are 9,10,11,12
  #define MODULE_LM75_COUNT 1
#endif

#ifdef MODULE_ENABLE_SERIAL
  #define MODULE_SERIAL_BAUDRATE 115200     // Serial port baud rate
#endif

#define PWM_CHANNELS 3
#define PWM_CHANNEL_NAMES {"desklight", "bedlight", "thirdlight"}
#define PWM_CHANNEL_INDEX {0, 1, 2}
#define PWM_CHANNEL_TYPES {PWM_TYPE_PCA9685,PWM_TYPE_PCA9685,PWM_TYPE_PCA9685}
#define PWM_CHANNEL_MINVAL {0, 0, 0}
#define PWM_CHANNEL_MAXVAL {0xFFFF, 0xFFFF, 0xFFFF}

#define DIN_CHANNELS 3
#define DIN_CHANNEL_NAMES {"button1", "button2", "button3"};
#define DIN_CHANNEL_TYPES {DIN_TYPE_MPR121,DIN_TYPE_MPR121,DIN_TYPE_MPR121};
#define DIN_CHANNEL_INDEX {0,1,2}

#define DOUT_CHANNELS 1
#define DOUT_CHANNEL_NAMES {"digout1"}
#define DOUT_CHANNEL_TYPES {DOUT_TYPE_PCA9685}
#define DOUT_CHANNEL_INDEX {6}

#define TEMPERATURE_MEASURE_INTERVAL 2500

//--------- Helper macros -------------//
#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #ifndef MODULE_ENABLE_SERIAL
    #define MODULE_ENABLE_SERIAL
  #endif
  #ifndef MODULE_SERIAL_BAUDRATE
    #define MODULE_SERIAL_BAUDRATE 115200
  #endif
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

#endif //CONFIG_H
