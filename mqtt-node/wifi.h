#ifndef WIFI_H
#define WIFI_H
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define SSID_LENGTH     32
#define PASSWD_LENGTH   64
#define EEPROM_LENGTH   (SSID_LENGTH + PASSWD_LENGTH)

void initWIFI();
void handleRoot();
void setAP(bool active);
void loopWIFI();

extern WiFiClient espClient;
extern ESP8266WebServer server;

#endif
