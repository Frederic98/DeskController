#!/usr/bin/env python3
import re
import traceback
import enum
from typing import Dict, Tuple
import paho.mqtt.client as mqtt
import struct
import logging

log = logging.Logger(__name__)


class MQTTConverter:
    class Direction(enum.Enum):
        ASCII_TO_BIN = 0
        BIN_TO_ASCII = 1

    def __init__(self, host):
        self.topics = {}            # type: Dict[str, Tuple[struct.Struct, function, function]]
        self.client = mqtt.Client(protocol=mqtt.MQTTv311 | 0x80)        # 0x80: Mosquitto bridge
        self.client.connect(host)
        self.client.on_message = self.on_message
        log.debug('Connected to {}'.format(host))

    def on_message(self, client, userdata, message: mqtt.MQTTMessage):
        try:
            topic = message.topic
            payload = message.payload
            if message.topic.startswith('bin/'):
                # binary -> ascii conversion
                topic = '/'.join(topic.split('/')[1:])      # remove 'bin/'
                packer, pubcast, subcast = self.topics[topic]
                data = subcast(*packer.unpack(payload))
                self.client.publish(topic, data, message.qos, retain=True)
            else:
                # ascii -> binary conversion
                packer, pubcast, subcast = self.topics['/'.join(topic.split('/')[:-1])]
                data = packer.pack(*pubcast(payload))
                self.client.publish('bin/'+topic, data, message.qos, retain=True)
        except Exception as e:
            print('***')
            traceback.print_exc()
            print(e)
            print(message.topic)
            print(message.payload)
            print('***')

    def on_binmessage(self, message: mqtt.MQTTMessage, packer, asciicast):
        # binary -> ascii conversion
        topic = '/'.join(message.topic.split('/')[1:])  # remove 'bin/'
        payload = message.payload
        data = asciicast(*packer.unpack(payload))
        self.publish(topic, data, message.qos, retain=True)

    def on_asciimessage(self, message:mqtt.MQTTMessage, packer, bincast):
        # ascii -> binary conversion
        topic = message.topic
        payload = message.payload
        data = packer.pack(*bincast(payload))
        self.publish('bin/' + topic, data, message.qos, retain=True)

    def register_topic(self, topic, packer, bcast, acast):
        if isinstance(packer, str):
            packer = struct.Struct(packer)

        self.subscribe(topic, lambda client, userdata, message: self.on_asciimessage(message, packer, bcast))
        self.subscribe(topic + '/set', lambda client, userdata, message: self.on_asciimessage(message, packer, bcast))
        self.subscribe('bin/' + topic, lambda client, userdata, message: self.on_binmessage(message, packer, acast))
        self.subscribe('bin/' + topic + '/set', lambda client, userdata, message: self.on_binmessage(message, packer, acast))

    def loop_forever(self, *args, **kwargs):
        self.client.loop_forever(*args, **kwargs)

    def subscribe(self, topic, callback=None):
        self.client.subscribe(topic)
        if callback is not None:
            self.client.message_callback_add(topic, callback)
        print('subscribed to topic {}'.format(topic))

    def publish(self, topic, data, *args, **kwargs):
        print('publishing to {}: {}'.format(topic, data))
        self.client.publish(topic, data, *args, **kwargs)

def fstrpint(width: int, v: bytes):
    """ float-string parse integer """
    return [int(float(v.decode())/100 * (2**width-1))]

def fstrfint(width: int, v: int):
    """ float-string from integer """
    return '{:.1f}'.format(v*100/(2**width-1))

def cstrpint(v: bytes):
    """ color-string parse integer """
    v = v.decode()
    if v.startswith('#'):
        c = int(v[1:], 16)
        return [(c >> 16) & 0xff, (c >> 8) & 0xff, c & 0xff]
    rgb = re.match(r'rgba?\((\d+), (\d+), (\d+)(?:, \d.\d+)?\)', v)
    if rgb:
        return int(rgb.group(1)), int(rgb.group(2)), int(rgb.group(3))
    raise ValueError('Unsupported color format: ' + v)

def cstrfint(r: int, g: int, b: int):
    """ color-string from integer """
    return '#{:02X}{:02X}{:02X}'.format(r,g,b)


logging.basicConfig()
log.debug('MQTT converter starting...')

# converter = MQTTConverter("127.0.0.1")
converter = MQTTConverter("raspberrypi")
converter.register_topic('pwm/+/brightness', '>H', lambda v: fstrpint(16, v), lambda v: fstrfint(16, v))
converter.register_topic('pwm/+/power', '>b', lambda v: [int(v)], str)
converter.register_topic('din/+/state', '>b', lambda v: [int(v)], str)
converter.register_topic('dout/+/state', '>b', lambda v: [int(v)], str)
converter.register_topic('ambilight/color', '>BBB', cstrpint, cstrfint)             # ToDo: append topic with "rgb/"
converter.register_topic('ambilight/power', '>b', lambda v: [int(v)], str)

log.debug('Starting event loop...')
converter.loop_forever()
