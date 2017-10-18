#! /usr/bin/python

"""
This script plots the coalesced tasks' sizes in time
expected input is a two hex digits in a line in a txt file
"""

from csf import *
import os
import matplotlib.pyplot as plt
import numpy as np


apps, filesContainer = filesFinder('simulatedPwrInter_1500/CoalescedTaskSize/camelPowerPattern/*.txt')
fifNUm = 0

maxCoalSize = 64 # this is used to check for the max coalesced task size
				 # and filter based on it

figureSetting()

for files in filesContainer:
	for file in files:
		name = nameMaker(file)
		l = labelMaker(file)
		print(l)
		data=[]
		for line in open('../data/user/'+file):
			if len(line) < 2:
				continue
			num = int(line, 16)
			if(num > maxCoalSize):
				continue 
			data.append(num)
		meanData = [np.mean(data)] * len(data) 
		
		plt.figure(apps[fifNUm], figsize=(8,3) )
		plt.plot(data, '-o', label=l)
		plt.legend(loc='best')
		plt.plot(meanData,  lw =2.5)
	plt.ylabel("Coal. Task Size")
	plt.tight_layout()
	plt.savefig("../figures/"+name+".pdf", format="pdf", dpi=1200)
	fifNUm+=1

plt.show()