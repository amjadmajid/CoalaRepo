#! /usr/bin/python

import matplotlib.pyplot as plt
from csf import *
import scipy.stats as stats
import numpy as np
import math



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
	apps = appsSelector()
	paths = filesFinder(apps, 'realTaskSize/*.txt')
	d = mspProfiler_dataMiner(apps, paths)
	apps_names = d.keys() 



	# task based processing/plotting

	# plt.figure("tasks distributions")
	# for app in apps_names:
	# 	tasks_names = d[app].keys()
	# 	print(tasks_names)
	# 	c = generateRandomColor()
	# 	print("task c", c)

	# 	for task_name in tasks_names:
	# 		print( d[app][task_name] )
	# 		# if not normPlotting(d[app][task_name], c):
	# 			# print(task_name)

	# plt.show()

	# exit()


	apps_dict={}
	for app in apps_names :
		appVals = list_flatter( d[app].values() )
		apps_dict[app] = appVals

	# Plotting apps distributions
	figureSetting()
	f = plt.figure("Apps distributions", figsize=(8,3))
	ax = plt.axes()
	ax.set_xscale('log')
	# linSty = ['b--', 'r:', 'g.', 'b:', "m-", ',', '>']
	for i, app in enumerate(apps_names) :
		c = generateRandomColor()
		# if not normPlotting( apps_dict[app], c  ): 
		# 	print(apps)

		plt.plot( np.array(sorted(apps_dict[app])), np.arange(1,len( apps_dict[app])+1) / float(len(apps_dict[app]) ),label=app, lw=1.5 )

	plt.ylabel("Task Size Dist.")
	plt.tight_layout()
	plt.legend(loc='best')
	f.savefig("../figures/TskDist.pdf", format="pdf", dpi=1200)
	plt.show()


if __name__ == '__main__':
	main()
