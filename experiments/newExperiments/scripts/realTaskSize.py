#! /usr/bin/python

import matplotlib.pyplot as plt
from csf import *
import scipy.stats as stats
import numpy as np
import math
from collections import defaultdict


def dataMiner( apps, paths ):
	"""
	@input: relative paths to data files extracted from 
			codeProfiler library
	@output: { app:{task:values, ....}, .... } 
	"""
	task_sizes= defaultdict(list)
	apps_size={}
	for app, path in zip(apps, paths):

		file = "../data/user/"+path[0]  # first path
		for line in open(file):
			try:
				(taskId, value) = line.split(" ")
			except:
				continue

			task_sizes[app+'_'+taskId].append( int(value[:-1], 16) ) # lists of a task sizes for repeated executions
		apps_size[app] = task_sizes.copy()
		task_sizes.clear()
	return apps_size



def list_flatter(l):
	"""
	flatter to levels nested lists
	"""
	fl=[]
	for sl in l:
		for el in sl:
			fl.append(el)
	return(fl)


def main():
	
	apps, paths = filesFinder('realTaskSize/*.txt')
	d = dataMiner(apps, paths)
	apps_names = d.keys() 



	# task based processing/plotting

	plt.figure("tasks distributions")
	for app in apps_names:
		tasks_names = d[app].keys()
		# print(tasks_names)
		c = generateRandomColor()
		# print("task c", c)
		for task_name in tasks_names:
			if not normPlotting(d[app][task_name], c):
				print(task_name)

		plt.show()


	apps_dict={}
	for app in apps_names :
		appVals = list_flatter( d[app].values() )
		apps_dict[app] = appVals

	# Plotting apps distributions
	plt.figure("Apps distributions")
	for app in apps_names :
		c = generateRandomColor()
		if not normPlotting( apps_dict[app], c  ): 
			print(apps)
		plt.show()


if __name__ == '__main__':
	main()
