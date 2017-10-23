#! /usr/bin/python
import random
import math
import matplotlib.pyplot as plt
import numpy as np
from csf import *

class task( object):
	def __init__(self,id, size, name):
		""" Create a task"""
		self.id = id
		self.size = size
		self.name = name
	
	def getId(self):
		return int(self.id)

	def getSize(self):
		return int(self.size)

	def setSizeLabel(self, sl):
		self.sLabel = sl

	def getSizeLabel(self):
		return int(self.sLabel)

	def getName(self):
		return self.name


class tasksCont(object):
	"""Expects a sorted list of tasks based on their size"""
	def __init__(self, tsks):
		self.tsks = tsks
		self.idx =0
		
	def setNumCatags(self, nCa):
		self.nCa = nCa
		
	def getNumCatags(self):
		return self.nCa

	def getCatagMetric(self):
		return( int(math.ceil( float(self.tsks[-1].getSize()) / self.getNumCatags() ) ) )

	def tsksLabel(self):
		mtrc = self.getCatagMetric()
		# print '->>>', mtrc
		for tsk in self.tsks:
			i = 1  
			while tsk.getSize() > mtrc * i:
				i+=1
				print('i - ', i)
			tsk.setSizeLabel( i )


def clustersSizes(cls):
	sizes=[0]
	elm = cls[0]
	for c in cls:
		if elm == c:
			sizes[-1]+=1
		else:
			elm = c
			sizes.append(1)
	return sizes


tasks = []
riskLevels = 5

apps = appsSelector()
print(apps)
paths = filesFinder(apps, 'realTaskSize/*.txt')
print(paths)

d = mspProfiler_dataMiner(apps, paths) 

apps_names = d.keys() 

for app in apps_names:
	tasks_names = d[app].keys()
	print(tasks_names)
	
	for i, task_name in enumerate(tasks_names):
		print( max( d[app][task_name]) )
		tasks.append( task(i, max( d[app][task_name]), task_name ) )


numOfTasks = len(tasks_names)


# for i in range(numOfTasks):
# 	tasks.append( task(i, random.randint(100, 20000)) )


tasks.sort(key=lambda x:x.size)

tasksObj = tasksCont(tasks)
tasksObj.setNumCatags(riskLevels)
tasksObj.tsksLabel()

clusters = []
sizes = []

for i, tsk in enumerate(tasks):
	clusters.append(tsk.getSizeLabel() )
	sizes.append(tsk.getSize() / float(tasks[-1].getSize() ) * riskLevels )

riskLabels=set()
for i, tsk in enumerate(tasks):
	print("{:40s}{:2d}{:6d}{:2d}".format(tsk.getName(), tsk.getId(), tsk.getSize(), tsk.getSizeLabel()) )
	riskLabels.add( tsk.getSizeLabel() )


riskLabels = list(riskLabels)
clusWidth = clustersSizes(clusters) 

plt.figure()

colWd = 0.5

for i, wd in enumerate( clusWidth):
	plt.bar( colWd , riskLabels[i], width=wd, align="edge", color='0.85', lw=2)
	colWd += wd
plt.bar( colWd , i+1 , width=wd, align="edge", color='0.85' , label='risk level') # to add the label

plt.plot( range(1, len(sizes)+1), sizes , 'ro', label='norm. task Size')

ax = plt.gca()
ax.set_xlim([0.5,numOfTasks+.5])
ax.set_ylim([0,riskLevels+0.05])
plt.legend(loc='best')

plt.show()































