#include "Arduino.h"
#include "mqtt.h"
#include "wifi.h"
#include "config.h"
#include "pwm.h"
#include "dout.h"

PubSubClient client(espClient);
unsigned long lastReconnectAttempt = 0;

void initMQTT(){
  client.setServer(WiFi.gatewayIP(), 1883);
  DEBUG_PRINT("Server set to port 1883 on host ");
  DEBUG_PRINTLN(WiFi.gatewayIP());
  client.setCallback(callbackMQTT);
}

void loopMQTT(){
  // Handle MQTT server
  if(client.connected()){
    // If MQTT client is connected
    client.loop();
  }else{
    // Else, try to connect
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      // Attempt to reconnect every 5 seconds
      lastReconnectAttempt = now;
      DEBUG_PRINT("->Trying to connect to MQTT server...");
      if (reconnectMQTT()) {
        DEBUG_PRINTLN(" OK");
        lastReconnectAttempt = 0;
      }else{
        DEBUG_PRINTLN(" FAIL");
      }
    }
  }
}

boolean reconnectMQTT() {
  if (client.connect(HOSTNAME)) {
    for(uint8_t channel=0; channel<PWM_CHANNELS; channel++){
      char topic[31] = "bin/pwm/";        //  8         Total: 30+1=31
      strcat(topic, pwm_name[channel]);   // 16 (max)
      strcat(topic, "/+/set");            //  6
      client.subscribe(topic);            // bin/pwm/{topic}/+/set
      DEBUG_PRINT("Subscribed to ");
      DEBUG_PRINTLN(topic);
    }
    for(uint8_t channel=0; channel<DOUT_CHANNELS; channel++){
      char topic[36] = "bin/dout/";       //  9         Total: 35+1=36
      strcat(topic, dout_name[channel]);   // 16 (max)
      strcat(topic, "/state/set");        // 10
      client.subscribe(topic);            // bin/pwm/{topic}/+/set
      DEBUG_PRINT("Subscribed to ");
      DEBUG_PRINTLN(topic);
    }
  }
  return client.connected();
}

void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  DEBUG_PRINT("->MESSAGE\n  ");
  DEBUG_PRINTLN(topic);
  
  if(strncmp(topic, "bin/", 4) != 0) return;        // Topic is invalid
  
  char moduleName[16] = "";
  char* moduleStart = topic + 4;
  char* moduleEnd = strchr(moduleStart, '/');
  if(moduleEnd == NULL) return;                                   // Topic is invalid
  uint8_t moduleLength = min(15, moduleEnd - moduleStart);
  strncpy(moduleName, moduleStart, moduleLength);                 // Copy module name into string
  
  char channelName[16] = "";
  char* channelStart = moduleEnd + 1;
  char* channelEnd = strchr(channelStart, '/');
  if(channelEnd == NULL) return;                                  // Topic is invalid
  uint8_t channelLength = min(15, channelEnd - channelStart);
  strncpy(channelName, channelStart, channelLength);              // Copy channel name into string
  
  if(strcmp(moduleName, "pwm") == 0){
    DEBUG_PRINTLN("  PWM");
    callbackPWM(channelName, channelEnd, payload, length);
  }else if(strcmp(moduleName, "dout") == 0){
    DEBUG_PRINTLN("  DOUT");
    callbackDOUT(channelName, channelEnd, payload, length);
  }
}

void publishUInt8(const char* topic, const uint8_t value){
  uint8_t data[1];
  data[0] = value & 0xff;
  client.publish(topic, data, sizeof(data), true);
}

void publishUInt16(const char* topic, const uint16_t value){
  uint8_t data[2];
  data[0] = value >> 8 & 0xff;
  data[1] = value      & 0xff;
  client.publish(topic, data, sizeof(data), true);
}

void publishUInt32(const char* topic, const uint32_t value){
  uint8_t data[4];
  data[0] = value >> 24 & 0xff;
  data[1] = value >> 16 & 0xff;
  data[2] = value >>  8 & 0xff;
  data[3] = value       & 0xff;
  client.publish(topic, data, sizeof(data), true);
}
