from JVSE import JVSE
import time, threading

class Template:
    def __init__(self):
        self.emulator = JVSE()

    def run(self):
        self.emulator.start()
	self.emulator.set_key_player1(3, 1);
        while 1:
            time.sleep(.001)

if __name__ == "__main__" :
    template = Template()
    template.run()
