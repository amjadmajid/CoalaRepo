#! /usr/bin/python

import matplotlib.pyplot as plt
from csf import *
import scipy.stats as stats
import numpy as np
import math

def dataMiner( paths ):
	"""
	@input: relative paths to data files extracted from 
			codeProfiler library
	@output: a list of dictionaries for all the apps. 
			 each dict entry has a task id and its size 
	"""
	data = []
	for path in paths:
		counter =  dict();
		size =  dict();
		file = "../data/user/"+path[0]  # first path
		for line in open(file):
			try:
				(taskId, value) = line.split(" ")
			except:
				continue

			counter[taskId] =  counter.get(taskId, 0) + 1
			size[taskId] = size.get(taskId,0) + int(value[:-1], 16)

		codeBlockDis= dict()
		for key in counter:
			codeBlockDis[key] = (size[key] / counter[key])
		data.append(codeBlockDis)
	return (data)


def main():

	apps, paths = filesFinder('realTaskSize/*.txt')

	codeBlockDis = dataMiner(paths)
	All_values = []
	for i in range(len(apps)):
		normPlotting(codeBlockDis[i].values())

		All_values+=codeBlockDis[i].values()

	f = plt.figure()
	plt.hist(All_values, bins=25)
	plt.xlabel("Task size in MCU cycles")
	plt.tight_layout()
	f.savefig("../figures/realTaskSize.pdf",format="pdf", dpi=1200)
	plt.show()  
	# print(codeBlockDis)



if __name__ == '__main__':
	main()
