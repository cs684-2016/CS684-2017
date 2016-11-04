import serial
import numpy
import matplotlib.pyplot as plt
from drawnow import *

leftrightA=[]
updownA=[]
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=2, xonxoff=False, rtscts=False, dsrdtr=False) 
ser.flushInput()
ser.flushOutput()
plt.ion()

def dataPlot():
	plt.title("X-Y Co-ordinate data from Joystick")
	plt.grid(True)
	plt.ylabel("ASCII Values")
	plt.plot(leftrightA, 'ro-', label="ASCII")
	plt.legend(loc='upper left')
	plt2=plt.twinx()
	plt2.plot(updownA)

while True:
	while (ser.inWaiting()==0):
		pass
	dataStream = ser.readline();
	#ser.read(dataStream)
	#print dataStream
	dataArray = dataStream.split(',')
	#print dataArray[0]
	#print dataArray[1]
	leftright = float ( dataArray[0] )
	updown = float( dataArray[1] )
	#print leftright,",",updown
	leftrightA.append(leftright)
	updownA.append(updown)
	drawnow(dataPlot)
