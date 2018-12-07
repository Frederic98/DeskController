#ifndef MQTT_H
#define MQTT_H
#include <PubSubClient.h>
#include "Arduino.h"

extern PubSubClient client;
extern unsigned long lastReconnectAttempt;

void publishUInt8(const char* topic, const uint8_t value);
void publishUInt16(const char* topic, const uint16_t value);
void publishUInt32(const char* topic, const uint32_t value);

void callbackMQTT(char* topic, byte* payload, unsigned int length);
boolean reconnectMQTT();
void initMQTT();
void loopMQTT();

#endif //MQTT_H
