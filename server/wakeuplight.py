#!/usr/bin/env python3
import signal
import threading
import time
import paho.mqtt.client as mqtt


class WakeupLight:
    def __init__(self, server):
        self.server = server
        self.client = mqtt.Client()
        self.client.connect(self.server)

        self.subscribe('wakeuplight/active/set', self.mqtt_wakeup_set)
        self.wakeup_active = threading.Event()

        self.wakeup_duration = 30*60
        self.wakeup_stepsize = 0.05
        self.wakeup_endvalue = 100
        self.wakeup_stepdt = self.wakeup_duration / (self.wakeup_endvalue / self.wakeup_stepsize)
        self.wakeup_stepnum = 0

        threading.Thread(target=self.light_loop, daemon=True).start()

    def subscribe(self, topic, callback=None, *args, **kwargs):
        self.client.subscribe(topic, *args, **kwargs)
        if callback is not None:
            self.client.message_callback_add(topic, callback)

    def mqtt_wakeup_set(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        if message.payload == b'0':
            # deactivate wakeup light
            self.wakeup_active.clear()
        elif message.payload == b'1':
            # start wakeup light
            self.wakeup_stepnum = 0
            self.wakeup_active.set()

    def light_loop(self):
        while True:
            if self.wakeup_active.is_set():
                brightness = self.wakeup_stepnum * self.wakeup_stepsize
                self.wakeup_stepnum += 1
                self.client.publish('pwm/bedlight/brightness/set', '{:.4f}'.format(brightness))
                if brightness >= 100:
                    self.wakeup_active.clear()
                    continue
                time.sleep(self.wakeup_stepdt)
            else:
                self.wakeup_active.wait()
                self.client.publish('pwm/bedlight/brightness/set', '0')
                self.client.publish('pwm/bedlight/power/set', '1')

    def loop_forever(self):
        self.client.loop_forever()


if __name__ == '__main__':
    light = WakeupLight('raspberrypi')
    light.loop_forever()
