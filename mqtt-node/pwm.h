#ifndef PWM_H
#define PWM_H
#include "config.h"

#define PWM_TYPE_PIN 0
#define PWM_TYPE_PCA9685 1

extern char pwm_name[PWM_CHANNELS][16];
extern uint8_t pwm_type[PWM_CHANNELS];
extern uint8_t pwm_index[PWM_CHANNELS];
extern bool pwm_enabled[PWM_CHANNELS];
extern uint16_t pwm_duty[PWM_CHANNELS];
extern uint16_t pwm_min[PWM_CHANNELS];
extern uint16_t pwm_max[PWM_CHANNELS];

uint16_t inRange(uint16_t val, uint16_t minimum, uint16_t maximum);
void setPWMDuty(uint8_t channel, uint16_t d);
void updatePWMDuty(uint8_t channel);
void forcePWMDuty(uint8_t channel, uint16_t v);
void initPWM();
void callbackPWM(char* name, char* topic, byte* payload, unsigned int length);

#endif  //PWM_H
