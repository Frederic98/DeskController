#include "pwm.h"
#include "din.h"
#include "mqtt.h"
#include "config.h"
#include "wifi.h"
#include "modules.h"

void setup() {
#ifdef MODULE_ENABLE_SERIAL
  Serial.begin(MODULE_SERIAL_BAUDRATE);
#endif
  initWIFI();
  initMQTT();

  initModules();
#if PWM_CHANNELS > 0
  initPWM();
#endif
#if DIN_CHANNELS > 0
  initDIN();
#endif
}

void loop() {
  loopWIFI();
  loopMQTT();

  // Check digital inputs
#if DIN_CHANNELS > 0
  checkDIN();
#endif
}
