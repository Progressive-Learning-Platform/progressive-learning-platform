#!/usr/bin/env python

import serial
import sys
import time

print "-----auto_test start-----"

ser = serial.Serial(sys.argv[1],57600,timeout=1)
ser.open()

ser.write("f")

data = 'a'

start = time.time()

while data != chr(0x04) and time.time()-start < 30:
	data = ser.read(1)	
	if data != chr(0x04):
		sys.stdout.write(data)

print "-----auto_test stop-----"
