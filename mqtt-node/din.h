#ifndef DIN_H
#define DIN_H
#include "Arduino.h"
#include "config.h"

#define DIN_TYPE_PIN 0
#define DIN_TYPE_MPR121 1

#define DIN_ENABLE_MPR121
#ifdef DIN_ENABLE_MPR121
  extern uint16_t din_mpr121_state;
  extern uint16_t din_mpr121_readcount;
#endif

extern char din_name[DIN_CHANNELS][16];
extern uint8_t din_type[DIN_CHANNELS];
extern uint8_t din_index[DIN_CHANNELS];
extern bool din_state[DIN_CHANNELS];

void initDIN();
void checkDIN();

#endif  //DIN_H
