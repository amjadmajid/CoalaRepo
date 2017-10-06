#! /usr/bin/python

import matplotlib.pyplot as plt
from csf import *
import numpy as np 
import scipy.stats as stats
import sys

def main():

	user_app = None
	apps, files = filesFinder('CoalescedTaskSize/*-60cm.hex')
	disp_apps = apps[:]
	user_choice=[]

	while(1):
		if( sys.version_info[0] ) < 3 :
			print('to choose from '+str(disp_apps)+' or (done):')
			user_app = raw_input()
		else:
			user_app = input()

		if(user_app == 'done'):
			break;
		disp_apps.remove(user_app)
		user_choice.append(user_app)

	if user_choice !=[]:
		apps = user_choice[:]


	dataMean=[]
	data=[]

	for app in apps:
		for file in files:
			if file[0].split('/')[0] == app:
				rawData = dataMiner(dataUser+file[0],  0xB000000, 0xB400000, 2)
				data.append(rawData)
				dataMean.append(np.mean( rawData ) )
	# print(data)
	# exit()

	f_avg = plt.figure(figsize=(8,2.5))
	figureSetting()                        # Set figure layout

	xLocs = range(1, len(dataMean)+1 )
	set_xaxis(plt.axes(), xLocs, apps) # set the x axis labels

	plt.bar( xLocs, dataMean, width=1, align='center', color='b', label='Avg. coalesced task' )
	
	plt.ylabel('Real Task')
	plt.legend(loc='best')
	plt.tight_layout()


	f_time = plt.figure(figsize=(8,2.5))  # new figure

	for i, raw in enumerate(data):
		plt.plot(raw , '-o', label=apps[i])
	

	plt.legend(loc='best')
	plt.tight_layout()
	# f_avg.savefig("../../figures/avgCoalescedTask.pdf",format="pdf", dpi=1200)
	f_time.savefig("../../figures/coalescedTask.pdf",format="pdf", dpi=1200)

	plt.show()


if __name__ == '__main__':
	main()
