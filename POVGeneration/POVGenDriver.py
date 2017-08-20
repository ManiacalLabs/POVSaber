from bibliopixel.drivers.driver_base import DriverBase
from bibliopixel.colors.gamma import APA102
import time
import struct


class POVGen(DriverBase):
    """For Testing: Provides no ouput, just a valid interface"""

    def __init__(self, num, filename=None):
        super().__init__(num, gamma=APA102)
        self.filename = filename
        self.file = open(self.filename, 'wb')

    def cleanup(self):
        print('Writing ' + self.filename)
        self.file.close()

    def _compute_packet(self):
        self._render()

    def _send_packet(self):
        self.file.write(struct.pack('<{}B'.format(len(self._buf)), *self._buf))
        # print(self._buf)
