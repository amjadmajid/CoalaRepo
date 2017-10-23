#! /usr/bin/python

from csf import *
import re
import datetime 
import numpy as np
import matplotlib.pyplot as plt

apps = appsSelector()
paths = filesFinder(apps, 'powerInterruptRate/*.txt')

rates=[]

for anApp in paths:
	for apath in anApp:
		f = open('../data/user/'+apath)
		text = f.read()
		f.close()
		timeStamp = re.findall(r'\d{2}:\d{2}:\d{2},\d{3}', text)
		pwrIner = int( re.findall(r'\d+', str( re.findall(r'number[a-z\s]*:\s\d{1,5}',  text)))[0])

		TimeFormat = '%H:%M:%S,%f'
		x =  datetime.datetime.strptime(timeStamp[-1],TimeFormat ) - datetime.datetime.strptime(timeStamp[0],TimeFormat )
		rates.append( pwrIner / x.total_seconds() )


f= plt.figure() 
figureSetting()
c = generateRandomColor()
normPlotting(rates,c)
# plt.bar(rates, rates)
# plt.boxplot(rates)
plt.ylabel("Power interrupt rate (frequency)")
plt.tight_layout()
f.savefig("../figures/powerInterruptRate.pdf",format="pdf", dpi=1200)
plt.show()  