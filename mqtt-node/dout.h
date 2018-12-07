#ifndef DOUT_H
#define DOUT_H
#include "config.h"

#define DOUT_TYPE_PIN 0
#define DOUT_TYPE_PCA9685 1

extern char dout_name[DOUT_CHANNELS][16];
extern uint8_t dout_type[DOUT_CHANNELS];
extern uint8_t dout_index[DOUT_CHANNELS];
extern bool dout_state[DOUT_CHANNELS];

void setDOutState(uint8_t channel, bool s);
void initDOUT();
void callbackDOUT(char* name, char* topic, byte* payload, unsigned int length);

#endif  //DOUT_H
