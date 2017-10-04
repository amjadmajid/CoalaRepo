#! /usr/bin/python

import os 
import glob
import matplotlib.pyplot as plt
from csf import *
import numpy as np 
import scipy.stats as stats


def filesFinder():
	filesPaths = []
	os.chdir('../data/')
	appsMainFolders = glob.glob('*')  # grep all apps folders

	for appFolder in appsMainFolders:
		filesPaths.append( glob.glob( appFolder+'/CoalescedTaskSize/*60cm.hex') ) # grep all .hex files
	os.chdir('../scripts/')
	return (appsMainFolders, filesPaths)



################# Ploting ######################


def main():

	apps, files = filesFinder()

	data=[]
	appsNames = []
	for app in apps:
		appsNames.append(app.split('_')[0])
		for file in files:
			if file[0].startswith( str(app) ):
				data.append(np.mean( dataMiner('../data/'+file[0],  0xB000000, 0xB400000, 2) ) )


	f = plt.figure(figsize=(8,2.5))
	figureSetting()                        # Set figure layout

	xLocs = range(1, len(data)+1 )

	set_xaxis(plt.axes(), xLocs, appsNames) # set the x axis labels

	plt.bar( xLocs, data, width=1, align='center', color='b', label='Avg. coalesced task' )
	
	plt.ylabel('Real Task')
	plt.legend(loc='best')
	f.savefig("../figures/avgCoalescedTask.pdf",format="pdf", dpi=1200)
	plt.tight_layout()
	plt.show()


if __name__ == '__main__':
	main()
