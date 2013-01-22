import arduino # Import the interface library
import time as t

ard = arduino.Arduino() # Create the Arduino object

# Create other actuators, sensors, etc.
#m0 = arduino.Motor(ard, 2, 3, 4) # Motor with pwm output on pin 4, direction pin on pin 3, and current sensing pin on pin A2
#d0 = arduino.DigitalInput(ard, 22) # Digital input on pin 22
s0 = arduino.Servo(ard,9)

ard.run() # Start the thread that interacts with the Arduino itself

s0.setAngle(0)
t.sleep(.5)
s0.setAngle(180)

