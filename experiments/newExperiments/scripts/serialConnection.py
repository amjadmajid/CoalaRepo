#! /Users/amjad/anaconda/bin/python3.6
import serial 
import os
import sys 
import time 

# get app name 
appName=None
if( sys.version_info[0] ) < 3 :
    appName =  raw_input('enter app name: ')
else:
    appName =  input('enter app name: ')


# check if the required directory exist
basePath= '../data/user/'
tailPath = 'simulatedPwrInter_1500/CoalescedTaskSize/camelPowerPattern/'

dirPath = os.path.join(os.path.join(basePath,appName), tailPath)

# print(dirPath)
# exit()

if not os.path.isdir(dirPath) :
	#make the directory path
	os.makedirs( dirPath )


cp = os.getcwd()

fname =  open( os.path.join(dirPath,'slowChange_2.txt') , 'w')

portName = "/dev/cu.usbmodem1413"
baudRate = 115200

ser = serial.Serial(portName, baudRate, timeout=5)



Togo = 1000
while True:
	if Togo :
		try:
			np = str(ser.read(4), 'utf-8') 
			print(np, end="")
			fname.write(np )
		except e :
			print("Reception error: ", str(e) )
			continue
	else:
		ser.close()
		exit()
	Togo -=1
