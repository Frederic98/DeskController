# DeskController

An MQTT based system to let some ESP8266 chips communicate.

The project consits of a python part (running on a raspberry pi) and a c++ part (running on some ESP8266's with arduino)).  
For the ESP8266, I designed a custom PCB that has a voltage regulator, 3 mosfets and a temperature sensor.  
https://easyeda.com/Frederic98/esp8266-led-controller

One of the python scripts controls a standby and a mute input of an audio amplifier, and plays internet radio streams. Another functions as a wakeup light, constantly sending an increasing brightness value to an ESP8266 when triggered. The third script is a service that converts MQTT messages back and forth between an ascii text, and a binary value.  
All python scripts are run as a systemd service.
