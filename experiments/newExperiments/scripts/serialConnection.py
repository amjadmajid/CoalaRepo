#! /Users/amjad/anaconda/bin/python3.6
import serial 
import os
import sys 
import time 
from csf import *
# get app name 
appName=unified_input("enter app's name: ")
fileName=unified_input("file's name (fast, slow, smart): ")
fileName +='Change.txt'
pwrPtn = unified_input("Select from (camel, toward, fixed)")

Togo=int( unified_input("Number of lines to be received: ") )
lineLen=int( unified_input("line's length: ") )

# check if the required directory exist
basePath= '../data/'
tailPath = 'simulatedPwrInter/CoalescedTaskSize/'+pwrPtn+'PowerPattern/'

dirPath = os.path.join(os.path.join(basePath,appName), tailPath)

# print(dirPath)
# exit()

if not os.path.isdir(dirPath) :
	#make the directory path
	os.makedirs( dirPath )


cp = os.getcwd()

fname =  open( os.path.join(dirPath,fileName) , 'w')

portName = "/dev/cu.usbmodem1413"
baudRate = 115200

ser = serial.Serial(portName, baudRate, timeout=5)


while Togo:
		try:
			np = str(ser.read(lineLen), 'utf-8') 
			print(np, end="")
			fname.write(np )
		except e :
			print("Reception error: ", str(e) )
			continue
		Togo -=1

ser.close()
exit()
