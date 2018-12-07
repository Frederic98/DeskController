#include "Arduino.h"
#include "pwm.h"
#include "config.h"
#include "modules.h"
#include "mqtt.h"

char pwm_name[PWM_CHANNELS][16] = PWM_CHANNEL_NAMES;
uint8_t pwm_type[PWM_CHANNELS] = PWM_CHANNEL_TYPES;
uint8_t pwm_index[PWM_CHANNELS] = PWM_CHANNEL_INDEX;
bool pwm_enabled[PWM_CHANNELS];
uint16_t pwm_duty[PWM_CHANNELS];
uint16_t pwm_min[PWM_CHANNELS] = PWM_CHANNEL_MINVAL;
uint16_t pwm_max[PWM_CHANNELS] = PWM_CHANNEL_MAXVAL;

uint16_t inRange(uint16_t val, uint16_t minimum, uint16_t maximum){
  return max(minimum, min(val, maximum));
}
void setPWMDuty(uint8_t channel, uint16_t d){
  pwm_duty[channel] = inRange(d, pwm_min[channel], pwm_max[channel]);
  updatePWMDuty(channel);
}
void updatePWMDuty(uint8_t channel){
  forcePWMDuty(channel, pwm_enabled[channel] ? pwm_duty[channel] : 0);
}
void forcePWMDuty(uint8_t channel, uint16_t v){
  switch(pwm_type[channel]){
    case PWM_TYPE_PIN:
      analogWrite(pwm_index[channel], v >> 6);
      break;
#ifdef MODULE_ENABLE_PCA9685
    case PWM_TYPE_PCA9685:
      pca9685.setPWM(pwm_index[channel], 0, v >> 4);
      break;
#endif //MODULE_ENABLE_PCA9685
  }
}

void initPWM(){
  for(uint8_t channel=0; channel<PWM_CHANNELS; channel++){
    switch(pwm_type[channel]){
      case PWM_TYPE_PIN:
        pinMode(pwm_index[channel], OUTPUT);
        break;
#ifdef MODULE_ENABLE_PCA9685
      case PWM_TYPE_PCA9685:
        break;
#endif //MODULE_ENABLE_PCA9685
    }
  }
}

void callbackPWM(char* name, char* topic, byte* payload, unsigned int length){
  uint8_t channel = 0;
  for(; channel<PWM_CHANNELS; channel++){
    if(strcmp(name, pwm_name[channel]) == 0) break;
  }
  if(channel >= PWM_CHANNELS) return;
  DEBUG_PRINT("  ");
  DEBUG_PRINTLN(pwm_name[channel]);
  
  if(strcmp(topic, "/brightness/set") == 0){
    if(length == 0){
      DEBUG_PRINTLN("  REQUEST");  // Don't set anything, only publish the current brightness
    }else{
      uint16_t brightness = payload[0] << 8;
      if(length > 1) brightness |= payload[1];
      setPWMDuty(channel, brightness);
      DEBUG_PRINT("  ");
      DEBUG_PRINTLN(brightness);
    }
    char topicOut[36] = "bin/pwm/";        //  4          Total: 35+1=36
    strcat(topicOut, pwm_name[channel]);   // 16 (max)
    strcat(topicOut, "/brightness");       // 11
    publishUInt16(topicOut, pwm_duty[channel]);
    DEBUG_PRINT("  ");
    DEBUG_PRINT(topicOut);
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(pwm_duty[channel]);
  }else if(strcmp(topic, "/power/set")==0){
    DEBUG_PRINT("  power -> ");
    if(length == 0){
      DEBUG_PRINTLN("REQUEST");  // Don't set anything, only publish the current state
    }else{
      int8_t powerstate = payload[0];
      if(powerstate == -1){
        pwm_enabled[channel] = !pwm_enabled[channel];
        DEBUG_PRINT("TOGGLE -> ");
      }
      else if(powerstate == 0) pwm_enabled[channel] = false;
      else if(powerstate == 1) pwm_enabled[channel] = true;
      DEBUG_PRINTLN(pwm_enabled[channel] ? "ON":"OFF");
      updatePWMDuty(channel);
    }
    char topicOut[31] = "bin/pwm/";        //  8          Total: 30+1=31
    strcat(topicOut, pwm_name[channel]);   // 16 (max)
    strcat(topicOut, "/power");            //  6
    publishUInt8(topicOut, pwm_enabled[channel] ? 1 : 0);
  }
}
