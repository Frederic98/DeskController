#!/usr/bin/env python3
import os
import subprocess
import threading
import logging
import time
import re
import signal
import sys
import smokesignal

logger = logging.Logger(__name__)


class Radio:
    re_icyinfo = re.compile(r"(\w+)='(.*?)';")

    def __init__(self):
        self.mplayer = None     # type: subprocess.Popen
        self._mplayer_writelock = threading.Lock()
        threading.Thread(target=self._mplayer_reader, daemon=True).start()

        self.stations = {}
        playlists_dir = os.path.join(os.path.dirname(__file__), 'playlists')
        for file in [file for file in os.listdir(playlists_dir) if file.endswith('.m3u')]:
            name = os.path.splitext(file)[0]
            with open(os.path.join(playlists_dir, file), 'r') as f:
                url = f.read().strip()
            self.stations[name] = url

    def _mplayer_reader(self):
        while True:
            try:
                if self._mplayer_active:
                    line = self.mplayer.stdout.readline().strip()
                    if line.startswith('ICY Info'):
                        fields = dict(self.re_icyinfo.findall(line))
                        if 'StreamTitle' in fields:
                            smokesignal.emit('radio/title', fields['StreamTitle'])
                else:
                    time.sleep(1)
            except Exception as e:
                logger.error(e)

    def _mplayer_write(self, cmd: str):
        with self._mplayer_writelock:
            self.mplayer.stdin.write(cmd + '\n')

    @property
    def _mplayer_active(self):
        return (self.mplayer is not None) and (self.mplayer.poll() is None)

    def _play_url(self, url):
        if self._mplayer_active:
            self._mplayer_write('loadfile {}'.format(url))
        else:
            self.mplayer = subprocess.Popen(['mplayer', '-slave', '-quiet', url],
                                            stdin=subprocess.PIPE,
                                            stdout=subprocess.PIPE,
                                            stderr=subprocess.STDOUT,
                                            universal_newlines=True,
                                            bufsize=1)

    def play(self, station):
        self._play_url(self.stations[station])

    def stop(self):
        if self._mplayer_active:
            self._mplayer_write('stop')

    def set_volume(self, value: int):
        assert 0 <= value <= 100
        self._mplayer_write('volume {:d} 1'.format(value))


if __name__ == '__main__':
    @smokesignal.on('radio/title')
    def print_title(title):
        print(title)

    r = Radio()
    if len(sys.argv) > 1:
        r.play(sys.argv[-1])
        signal.pause()
    else:
        r.play('qmusic')
        time.sleep(5)
        r.play('npo1')
        time.sleep(3)
        r.stop()
