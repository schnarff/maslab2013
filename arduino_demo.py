import sys
sys.path.append("../..")
import time

import arduino

ard = arduino.Arduino()
m0 = arduino.Motor(ard,0,2,11)
ard.run() # Start the thread which communicates with the Arduino

# Make the LED blink once a second
while True:
    m0.setSpeed(100)
    time.sleep(1)
