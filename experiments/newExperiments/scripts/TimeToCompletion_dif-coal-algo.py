#! /usr/bin/python

"""
This is unfinished script to measure the average ON time
It expects a .csv file from saleae logic analyzer
"""


from csf import *
import os
import matplotlib.pyplot as plt
import numpy as np

apps, filesContainer = filesFinder('simulatedPwrInter_1500/TimeToCompletion/*/*.csv')

dd=0
for files in filesContainer:
	for file in files:
		print(file)

		x=np.array([])
		y=np.array([])
		i=0
		for line in open('../data/user/'+file):
			i+=1
			if(i > 200):
				break
			tmp_x, tmp_y =line[:-1].split(',')
			x = np.append(x,tmp_x)
			y = np.append(y,tmp_y)

		# print(x)

		# removing the spikes 
		diffs=np.array([])
		for i, v in enumerate(y[:-1]):
			diffs = np.append(diffs, float(x[i+1]) - float(x[i]) )

		#Clean Y values
		y[ diffs < 0.0027]=0

		#combine the similar adjacent fields
		cleanX = []
		cleanY = []

		i = 0

		while i < len(y[:-1]):
			cleanY.append(y[i])
			if y[i] == y[i+1]:
				sigLength = 0
				while y[i] == y[i+1]:
					sigLength+= (x[i] + x[i+1]);
					i+=1
				i+=1
				cleanX.append( sigLength ) 
			else:
				cleanX.append(x[i])
				i+=1

		for i in range(1,len(cleanX)-1):
			print ( float(cleanX[i+1]) - float(cleanX[i]) )

		for i, yVal in enumerate(y[:-1]):
			plt.plot( [x[i], x[i+1]] , [yVal,yVal] , color='b' )	
			plt.plot( [x[i+1], x[i+1]] , [yVal,y[i+1]] , color='b'  )	

		break
	break

plt.margins(0.02)
plt.show()
		
