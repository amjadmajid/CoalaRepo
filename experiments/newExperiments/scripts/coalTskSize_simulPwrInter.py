#! /usr/bin/python

from csf import *
import os
import matplotlib.pyplot as plt
import numpy as np

apps, files = filesFinder('simulatedPwrInter_1500/CoalescedTaskSize/*.txt')
fifNUm = 0

files = files[0]
for file in files:
	print(file)
	fifNUm+=1
	data=[]
	for line in open('../data/user/'+file):
		if len(line) < 2:
			continue
		num = int(line, 16)
		if(num > 64):
			continue 
		data.append(num)
	meanData = [np.mean(data)] * len(data) 
	
	plt.figure(fifNUm)
	plt.plot(data)
	plt.plot(meanData)

plt.show()