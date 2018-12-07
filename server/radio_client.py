#!/usr/bin/env python3
import paho.mqtt.client as mqtt
import radioStreamer
import logging
import signal
import smokesignal

logging.basicConfig()
logger = logging.Logger(__name__)

client = mqtt.Client()
client.connect('127.0.0.1')

radio = radioStreamer.Radio()
radioActive = False
station = 'qmusic'
stations = sorted(radio.stations)


def mqtt_station(client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
    set_station(message.payload.decode())


def mqtt_active(client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
    msg = message.payload.decode()
    if msg in ['0', '1']:
        set_active(msg == '1')


def mqtt_toggle(client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
    set_active(not(radioActive))


def mqtt_volume(client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
    msg = message.payload.decode()
    value = int(msg)
    radio.set_volume(value)
    client.publish('radio/volume', str(value))


def mqtt_next(client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
    s = stations[(stations.index(station) + 1) % len(stations)]
    set_station(s)


def mqtt_prev(client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
    s = stations[(stations.index(station) - 1) % len(stations)]
    set_station(s)


def set_station(s):
    global station
    if s not in stations:
        return
    station = s
    set_active(True)
    client.publish('radio/station', station.encode())


def set_active(active):
    global radioActive
    radioActive = active
    if active:
        radio.play(station)
        client.publish('radio/active', b'1')
    else:
        radio.stop()
        client.publish('radio/active', b'0')


@smokesignal.on('radio/title')
def pub_title(title: str):
    client.publish('radio/title', title.encode())


client.subscribe('radio/station/set')
client.subscribe('radio/active/set')
client.subscribe('radio/active/toggle')
client.subscribe('radio/volume/set')
client.subscribe('radio/previous')
client.subscribe('radio/next')
client.message_callback_add('radio/station/set', mqtt_station)
client.message_callback_add('radio/active/set', mqtt_active)
client.message_callback_add('radio/active/toggle', mqtt_toggle)
client.message_callback_add('radio/volume/set', mqtt_volume)
client.message_callback_add('radio/previous', mqtt_prev)
client.message_callback_add('radio/next', mqtt_next)

client.loop_start()
client.publish('radio/station/list', '\n'.join(stations))
signal.pause()
