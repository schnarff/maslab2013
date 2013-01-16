from ctypes import cdll
from bitarray import bitarray
libImgProc = cdll.LoadLibrary('./libimgproc.so')

class ImageProcessing(object):
    def __init__(self):
        self.obj = libImgProc.ImageProcessing_new()
	self.ballTemp = str(bytearray(16*10))
	self.ballData = ""
	self.ballCount = 0
	self.ballList = []

    def classify(self,data):
        libImgProc.ImageProcessing_classify(self.obj,data)

    def findBalls(self,data):
        self.ballCount = libImgProc.ImageProcessing_findBalls(self.obj,data,self.ballTemp)
	self.ballData = bytearray(self.ballTemp)
	self.ballList = []
	for i in range(self.ballCount):
		base = 16*i;
		xi = self.ballData[3+base]<<24 | self.ballData[2+base]<<16 | self.ballData[1+base]<<8 | self.ballData[0+base]
		eta = self.ballData[7+base]<<24 | self.ballData[6+base]<<16 | self.ballData[5+base]<<8 | self.ballData[4+base]
		diam = self.ballData[11+base]<<24 | self.ballData[10+base]<<16 | self.ballData[9+base]<<8 | self.ballData[8+base]
		color = self.ballData[15+base]<<24 | self.ballData[14+base]<<16 | self.ballData[13+base]<<8 | self.ballData[12+base]
		ball = Ball(xi,eta,diam,color)
		self.ballList.append(ball)
		

class Ball(object):
    def __init__(self,xi,eta,diam,color):
	self.xi = xi
	self.eta = eta
	self.diam = diam
	self.color = color

#Initialize Pygame
import pygame.camera
import pygame.image
pygame.camera.init()
cams = pygame.camera.list_cameras()
cam = pygame.camera.Camera(cams[1],(320,240))
cam.start()
cam.get_image()
imgProc = ImageProcessing()
screen = pygame.display.set_mode((320,240))


#Initialize ARDUINO
import sys
import time
sys.path.append("../..")
import arduino

ard = arduino.Arduino()
m0 = arduino.Motor(ard,0,2,11)
m1 = arduino.Motor(ard,0,3,12)
ard.run()


#Game timer
ENDTIME = time.time()+2*60

#Begin Core Loop
prev = time.time()
frames = 0
data = 0
P = -3.0
I = -1.3
D = -0.00
xprev = 160
Sx = 0
Dx = 0
dt = 0.04

missing = 0

while ( time.time()<ENDTIME ):
	#Image Capture
	img = cam.get_image()
	data = pygame.image.tostring(img,"RGBX")

	#Image processing	
	imgProc.classify(data)
	imgProc.findBalls(data)

	#Motor Control Basics
	if ( imgProc.ballCount>0 ):
		closest = 0
		ball = imgProc.ballList[0]
		for B in imgProc.ballList:
			if ( B.diam>closest ):
				closest = B.diam
				ball = B
		#Track the closest ball
		x = (ball.xi-160)*float(ball.diam)/70.0
		Sx += dt*x
		Dx = (x-xprev)/dt		
		M = (x*P + Sx*I + Dx*D*ball.diam)*0.8
		xprev = x
		if ( Sx > 60 ):
			Sx = 60
		if ( Sx < -60 ):
			Sx = -60
		if ( M > 255 ):
			M = 255
		if ( M < -255 ):
			M = 255
		if ( abs(ball.xi-160)<20 or ball.diam>40 ):
			m0.setSpeed(200-M/6)
			m1.setSpeed(200+M/6)
			print "GO GO GO"
		else:
			m0.setSpeed(int(-M))
			m1.setSpeed(int(M))
			print "Turning: " + str(x) + " , " + str(Sx)
		missing = time.time()

	if ( time.time()-missing>1 ):
		m0.setSpeed(200)
		m1.setSpeed(-200)

	#Pygame output
	img = pygame.image.fromstring(data,(320,240),"RGBX")
	rect = img.get_rect()
	screen.blit(img,rect)

	#FPS
	pygame.display.update()
	frames+=1

	if ( time.time()-prev > 1 ):
		print "FPS: " + str(frames)
		frames = 0
		prev = time.time()

m0.setSpeed(0)
m1.setSpeed(0)
cam.stop()
pygame.quit()
