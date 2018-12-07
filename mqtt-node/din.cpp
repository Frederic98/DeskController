#include "din.h"
#include "Arduino.h"
#include "mqtt.h"
#include "config.h"
#include "modules.h"

#ifdef MODULE_ENABLE_MPR121
uint16_t din_mpr121_state;
uint16_t din_mpr121_readcount;
#endif //MODULE_ENABLE_MPR121

char din_name[DIN_CHANNELS][16] = DIN_CHANNEL_NAMES;
uint8_t din_type[DIN_CHANNELS] = DIN_CHANNEL_TYPES;
uint8_t din_index[DIN_CHANNELS] = DIN_CHANNEL_INDEX;
bool din_state[DIN_CHANNELS];

void initDIN(){
  for(uint8_t channel=0; channel<DIN_CHANNELS; channel++){
    if(din_type[channel] == DIN_TYPE_PIN){
      pinMode(din_index[channel], INPUT_PULLUP);
    }
  }
}

void checkDIN(){
#ifdef MODULE_ENABLE_MPR121
  if(digitalRead(MODULE_MPR121_IRQ) == LOW){
    DEBUG_PRINT("->Reading MPR212... #");
    DEBUG_PRINT(din_mpr121_readcount);
    din_mpr121_readcount++;
    din_mpr121_state = mpr121.touched();
    DEBUG_PRINTLN(" done!");
  }
#endif //MODULE_ENABLE_MPR121
  for(uint8_t channel=0; channel<DIN_CHANNELS; channel++){
    bool state;
    switch(din_type[channel]){
      case DIN_TYPE_PIN:
        state = digitalRead(din_index[channel]);
        break;
#ifdef MODULE_ENABLE_MPR121
      case DIN_TYPE_MPR121:
        state = din_mpr121_state & (1 << din_index[channel]);
        break;
#endif //MODULE_ENABLE_MPR121
      default:
        // Unknown type, skip this one...
        continue;
    }
    if(state != din_state[channel]){
      din_state[channel] = state;
      char topicOut[31] = "bin/din/";        //  8          Total: 30+1=31
      strcat(topicOut, din_name[channel]);   // 16 (max)
      strcat(topicOut, "/state");            //  6
      publishUInt8(topicOut, din_state[channel] ? 1:0);
    }
  }
}
