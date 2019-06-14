#! /Users/amjad/anaconda/bin/python3.6
import serial
import os
import sys
import time
from csf import *
# get app name
appName=unified_input("enter app's name: ")
fileName=unified_input("file's name ( (f)ast, (s)low, smar(t) ): ")

if fileName == 'f':
	fileName = 'fast'
elif fileName == 's':
	fileName = 'slow'
elif fileName == 't':
	fileName = 'smart'


fileName +='Change.txt'
pwrPtn = unified_input("Select from ( (c)amel, (t)oward, (f)ixed)")

if pwrPtn == 'c':
	pwrPtn = 'camel'
elif pwrPtn == 't':
	pwrPtn = 'toward'
elif pwrPtn == 'f':
	pwrPtn = 'fixed'

Togo=int( unified_input("Number of lines to be received: ") )
lineLen=int( unified_input("line's length: ") )

# check if the required directory exist
basePath= '../data/'
# tailPath = 'simulatedPwrInter/CoalescedTaskSize/'+pwrPtn+'PowerPattern/'
tailPath = 'realTaskSize/'


dirPath = os.path.join(os.path.join(basePath,appName), tailPath)

# print(dirPath)
# exit()

if not os.path.isdir(dirPath) :
	#make the directory path
	os.makedirs( dirPath )


cp = os.getcwd()

fname =  open( os.path.join(dirPath,fileName) , 'w')

portsName = ["/dev/cu.usbmodem14103","/dev/cu.usbmodem14203","/dev/cu.usbmodem14303" ]
baudRate = 115200
# port=None
# e=None
# ser=None
# while port in portsName:
# 	try:
ser = serial.Serial(portsName[1], baudRate, timeout=5)
	# except e:
	# 		print("Port error: ", str(e) )
	# 		continue

while Togo:
#		try:
                np = str(ser.read(lineLen), 'utf-8')
                print(np, end="")
                fname.write(np )
#		except IOEerror e :
#			print("Reception error: ", str(e) )
#			continue
#		Togo -=1

ser.close()
exit()
