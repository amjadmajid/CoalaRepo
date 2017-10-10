#! /usr/bin/python

from csf import *
import os
import matplotlib.pyplot as plt
import numpy as np

apps, files = filesFinder('simulatedPwrInter_1500/CoalescedTaskSize/*.txt')

for file in files:
	data=[]
	for line in open('../data/user/'+file[0]):
		num = int(line, 16)
		if(num > 64):
			continue 
		data.append(num)
	meanData = [np.mean(data)] * len(data) 
	
	plt.figure()
	plt.plot(data)
	plt.plot(meanData)
	
plt.show()