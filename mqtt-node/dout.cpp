#include "Arduino.h"
#include "dout.h"
#include "config.h"
#include "modules.h"
#include "mqtt.h"

char dout_name[DOUT_CHANNELS][16] = DOUT_CHANNEL_NAMES;
uint8_t dout_type[DOUT_CHANNELS] = DOUT_CHANNEL_TYPES;
uint8_t dout_index[DOUT_CHANNELS] = DOUT_CHANNEL_INDEX;
bool dout_state[DOUT_CHANNELS];

void setDOutState(uint8_t channel, bool s){
  if(channel >= DOUT_CHANNELS) return;
  switch(dout_type[channel]){
    case DOUT_TYPE_PIN:
      digitalWrite(dout_index[channel], s);
      break;
#ifdef MODULE_ENABLE_PCA9685
    case DOUT_TYPE_PCA9685:
      if(s == HIGH){
        pca9685.setPWM(dout_index[channel], 4096, 0);
      }else{
        pca9685.setPWM(dout_index[channel], 0, 4096);
      }
      break;
#endif //MODULE_ENABLE_PCA9685
  }
  dout_state[channel] = s == HIGH;
}

void initDOUT(){
  for(uint8_t channel=0; channel<DOUT_CHANNELS; channel++){
    switch(dout_type[channel]){
      case DOUT_TYPE_PIN:
        pinMode(dout_index[channel], OUTPUT);
        break;
#ifdef MODULE_ENABLE_PCA9685
      case DOUT_TYPE_PCA9685:
        break;
#endif //MODULE_ENABLE_PCA9685
    }
  }
}

void callbackDOUT(char* name, char* topic, byte* payload, unsigned int length){
  uint8_t channel = 0;
  for(; channel<DOUT_CHANNELS; channel++){
    if(strcmp(name, dout_name[channel]) == 0) break;
  }
  if(channel >= DOUT_CHANNELS) return;
  DEBUG_PRINT("  ");
  DEBUG_PRINT(dout_name[channel]);
  
  if(strcmp(topic, "/state/set") == 0){
    if(length == 0){
      DEBUG_PRINTLN("  REQUEST");  // Don't set anything, only publish the current brightness
    }else{
      bool state;
      if(payload[0] == 0) state = false;
      else if(payload[0] == 1) state = true;
      else if(payload[0] == 0xff) state = !dout_state[channel];
      else return;
      setDOutState(channel, state);
      DEBUG_PRINTLN(state ? " 1":" 0");
    }
    char topicOut[32] = "bin/dout/";            //  9          Total: 31+1=32
    strcat(topicOut, dout_name[channel]);       // 16 (max)
    strcat(topicOut, "/state");                 //  6
    publishUInt8(topicOut, dout_state[channel] ? 1:0);
    DEBUG_PRINT("  ");
    DEBUG_PRINT(topicOut);
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(dout_state[channel] ? "1":"0");
  }
}
