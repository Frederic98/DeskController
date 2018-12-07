from typing import Union, Tuple, List
import paho.mqtt.client as mqtt
import pigpio
import radioStreamer
import smokesignal
import logging

logger = logging.Logger(__name__)


class Amplifier:
    _amp_mute = False
    _amp_power = False
    _amp_channel = 0
    _radio_power = False
    _radio_station = ''
    _radio_volume = 0

    def __init__(self, server: str, io_pwr: int, io_mute: int, io_switch: Union[List[int, ...], Tuple[int, ...]]):
        self.server = server
        self.client = mqtt.Client()
        self.client.connect(self.server)
        self.client.loop_start()

        self.io_pwr = io_pwr
        self.io_mute = io_mute
        self.io_switch = io_switch
        self.gpio = pigpio.pi()
        for io in [*self.io_switch, self.io_pwr, self.io_mute]:
            self.gpio.set_mode(io, pigpio.OUTPUT)
            self.gpio.write(io, pigpio.LOW)

        smokesignal.on('radio/title', self.radio_pub_title)
        self.radio = radioStreamer.Radio()
        self.radio_stations = sorted(self.radio.stations)
        self._radio_station = self.radio_stations[0]

        self.amp_mute = False
        self.amp_power = False
        self.amp_channel = 0

        self.subscribe('amplifier/mute/set', self.mqtt_amp_mute)
        self.subscribe('amplifier/power/set', self.mqtt_amp_power)
        self.subscribe('amplifier/channel/set', self.mqtt_amp_channel)

        self.subscribe('radio/power/set', self.mqtt_radio_active)
        self.subscribe('radio/power/toggle', self.mqtt_radio_toggle)
        self.subscribe('radio/station/set', self.mqtt_radio_station)
        self.subscribe('radio/station/next', self.mqtt_radio_next)
        self.subscribe('radio/station/previous', self.mqtt_radio_prev)
        self.subscribe('radio/volume/set', self.mqtt_radio_volume)

    def subscribe(self, topic, callback=None, *args, **kwargs):
        self.client.subscribe(topic, *args, **kwargs)
        if callback is not None:
            self.client.message_callback_add(topic, callback)

    # MQTT callbacks
    def mqtt_amp_mute(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        if message.payload in [b'0', b'1']:
            self.amp_mute = message.payload == b'1'

    def mqtt_amp_power(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        if message.payload in [b'0', b'1']:
            self.amp_power = message.payload == b'1'

    def mqtt_amp_channel(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        ch = int(message.payload.decode())
        self.amp_channel = ch

    def mqtt_radio_station(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        self.radio_station = message.payload.decode()

    def mqtt_radio_active(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        if message.payload in [b'0', b'1']:
            self.radio_power = message.payload == b'1'

    def mqtt_radio_toggle(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        self.radio_power = not self.radio_power

    def mqtt_radio_volume(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        self._radio_volume = int(message.payload.decode())

    def mqtt_radio_next(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        self.radio_station = self.radio_stations[(self.radio_stations.index(self.radio_station) + 1) % len(self.radio_stations)]

    def mqtt_radio_prev(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        self.radio_station = self.radio_stations[(self.radio_stations.index(self.radio_station) - 1) % len(self.radio_stations)]

    # IO control
    @property
    def amp_mute(self):
        return self._amp_mute
    @amp_mute.setter
    def amp_mute(self, mute):
        self._amp_mute = mute
        self.gpio.write(self.io_mute, pigpio.LOW if mute else pigpio.HIGH)
        self.client.publish('amplifier/mute', b'1' if mute else b'0')

    @property
    def amp_power(self):
        return self._amp_power
    @amp_power.setter
    def amp_power(self, power: bool):
        self._amp_power = power
        self.gpio.write(self.io_pwr, pigpio.HIGH if power else pigpio.LOW)
        self.client.publish('amplifier/power', b'1' if power else b'0')

    @property
    def amp_channel(self):
        return self._amp_channel
    @amp_channel.setter
    def amp_channel(self, ch: int):
        for io in self.io_switch:
            self.gpio.write(io, pigpio.LOW)
        self.gpio.write(self.io_switch[ch], pigpio.HIGH)
        self.client.publish('amplifier/channel', str(ch).encode())

    @property
    def radio_power(self):
        return self._radio_power
    @radio_power.setter
    def radio_power(self, power: bool):
        self._radio_power = power
        if power: self.radio.play(self.radio_station)
        else: self.radio.stop()
        self.client.publish('radio/active', b'1' if power else b'0')

    @property
    def radio_station(self):
        return self._radio_station
    @radio_station.setter
    def radio_station(self, station):
        self._radio_station = station

    @property
    def radio_volume(self):
        return self._radio_volume
    @radio_volume.setter
    def radio_volume(self, volume):
        self.radio.set_volume(volume)
        self.client.publish('radio/volume', str(volume))

    def radio_pub_title(self, title: str):
        self.client.publish('radio/title', title.encode())


if __name__ == '__main__':
    import signal
    logging.basicConfig()
    amp = Amplifier('127.0.0.1', 18, 17, (27, 22, 23, 24))
    signal.pause()
