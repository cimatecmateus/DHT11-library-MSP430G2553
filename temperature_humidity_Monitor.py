#!/usr/bin/python 
#-*- coding: iso-8859-1 -*-
import time
import serial
 
# Start serial conection
comport = serial.Serial('/dev/ttyACM0', 9600)
 
# Time between the serial conection and the time to send anything
time.sleep(1.8)

while True: 
	valueSerial = comport.read()
	if valueSerial == 't':
		print 'Temperature: %d' % ord(comport.read())
	elif valueSerial == 'h':
		print 'Humidity: %d\n' % ord(comport.read())

