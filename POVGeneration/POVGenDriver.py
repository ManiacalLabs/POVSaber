from bibliopixel.drivers.driver_base import DriverBase
from bibliopixel.colors.gamma import APA102
import time
import struct
import os


class POVGen(DriverBase):
    """For Testing: Provides no ouput, just a valid interface"""

    def __init__(self, num, filename=None):
        super().__init__(num, gamma=APA102)
        self.filename = filename
        self.file = open(self.filename, 'rb+')
        self.rows = 0

    def cleanup(self):
        print('Writing ' + self.filename)
        # truncate last line and rewrite
        self.file.seek(0)
        data = self.file.read(self.numLEDs * (self.rows - 1) * 3)
        self.file.close()
        with open(self.filename, 'wb') as self.file:
            self.file.write(data)

    def _compute_packet(self):
        self._render()

    def _send_packet(self):
        self.file.write(struct.pack('<{}B'.format(len(self._buf)), *self._buf))
        self.rows += 1
        # print(self._buf)
