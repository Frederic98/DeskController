#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "Arduino.h"
#include "wifi.h"
#include "config.h"

WiFiClient espClient;
ESP8266WebServer server(80);

void initWIFI() {
  EEPROM.begin(96);       // Start EEPROM, 32 bytes for SSID + 64 bytes for password
  
  server.on("/", handleRoot);
  server.begin();
  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  setAP(true);
  
  char ssid[33];
  char passwd[65];
  for(int i=0; i<SSID_LENGTH; i++){ssid[i] = EEPROM.read(i);}
  for(int i=0; i<PASSWD_LENGTH; i++){passwd[i] = EEPROM.read(i + SSID_LENGTH);}

  // Print WiFi info
  DEBUG_PRINTLN();
  DEBUG_PRINT("Connecting to '");
  DEBUG_PRINT(ssid);
  DEBUG_PRINT("', with password '");
  DEBUG_PRINT(passwd);
  DEBUG_PRINTLN("'");
  
  WiFi.begin(ssid, passwd);

  while (WiFi.status() != WL_CONNECTED) {
    server.handleClient();
    yield();
  }

  DEBUG_PRINTLN();
  DEBUG_PRINTLN("WiFi connected");
  DEBUG_PRINT("IP address: ");
  DEBUG_PRINTLN(WiFi.localIP());
  DEBUG_PRINT("Gateway: ");
  DEBUG_PRINTLN(WiFi.gatewayIP());

  setAP(false);
}

void setAP(bool active){
  if(active){
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(HOSTNAME);
  }else{
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
  }
}

void handleRoot() {
  if(server.arg("ssid") == "" || server.arg("passwd") == ""){
    server.send(200, "text/html", "<html><head></head><body><h1>Configure WiFi</h1><form action=\"/\">SSID <input name=\"ssid\" type=\"text\" /><br>Password <input name=\"passwd\" type=\"text\" /><br><input type=\"submit\" /></form></body></html>");
  }else{
    char ssid[SSID_LENGTH + 1];
    char passwd[PASSWD_LENGTH + 1];
    server.arg("ssid").toCharArray(ssid, sizeof(ssid));
    server.arg("passwd").toCharArray(passwd, sizeof(passwd));
    for(int i=0; i<SSID_LENGTH; i++){EEPROM.write(i, ssid[i]);}
    for(int i=0; i<PASSWD_LENGTH; i++){EEPROM.write(i+SSID_LENGTH, passwd[i]);}
    EEPROM.commit();
    server.send(200, "text/html", "<html><head></head><body><h1>Ok, resetting...</h1></body></html>");
    ESP.reset();
  }
}

void loopWIFI(){
  server.handleClient();
}
