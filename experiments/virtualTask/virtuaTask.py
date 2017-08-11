#! /usr/bin/python

# get input
filePath_DFT = "DFT16/page_128/mem-dump_60cm.hex"
filePath_bc = "bc/page_128/mem-dump_60cm.hex"
filePath_cem = "CEM/page_128/mem-dump_60cm.hex"



def dataMiner(fileName, memStart, memEnd, wordSize):
	"""
		This function extract data from a file formated in intel hex format 
		@ inputs: 
			filename 
			The start/end of the memory section in hex
			wordSize: The size of word in bytes (2,4)
		@ output:
			an array of data in in int base 10 format
		@Author: Amjad Yousef Majid
	"""
	file = open(fileName)
	data = []
	for line in file:
		if int(line[3:10], 16) >= memStart and int(line[3:10], 16) <= memEnd:
			dataRow = line[9:-3]
			# print(dataRow)
			# each line is 20 bytes of data
			for i in range(0,20,wordSize):
				rawNum = int(dataRow[i:i+2], wordSize * 8) 
				if rawNum <= 64:    #specific check on data [must be removed for other projects]
					data.append(rawNum) 
			# print(data)
			# exit(0)
		
	return(data)


def figureSetting():
    """
    Set a figure params
    """
    #set figure linewidth
    plt.rcParams['axes.linewidth']=2

    #set font size for labels on axes
    plt.rcParams['axes.labelsize'] = 16

    #set size of numbers on x-axis
    plt.rcParams['xtick.labelsize'] = 16
    #set size of numbers on y-axis
    plt.rcParams['ytick.labelsize'] = 16
    #set size of ticks on x-axis
    # plt.rcParams['xtick.major.size'] = 7
    # #set size of ticks on y-axis
    # plt.rcParams['ytick.major.size'] = 7

    #set size of markers, e.g., circles representing points
    #set numpoints for legend
    plt.rcParams['legend.numpoints'] = 1
    plt.rcParams['legend.fontsize'] = 14

################# Ploting ######################


import matplotlib.pyplot as plt
import numpy as np 
import scipy.stats as stats
import matplotlib.ticker as ticker


## Data
data_bc = dataMiner(filePath_bc,  0x8400000, 0x8800000, 2)
data_dft = dataMiner(filePath_DFT, 0xB000000, 0xB400000, 2)
data_cem = dataMiner(filePath_cem, 0xB000000, 0xB400000, 2)

## Plot settings
f = plt.figure(figsize=(8,4))
figureSetting()                        # Set figure layout

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator([1,2,3]) )
ax.xaxis.set_major_formatter(ticker.FixedFormatter( ('BC', 'DFT', 'CEM') ))

# plt.ylabel('Real Task')

plt.bar([1,2,3], [np.mean(data_bc), np.mean(data_dft), np.mean(data_cem)], width=1, align='center', color='g', label='Ave. coalesced task' )
plt.bar([1,2,3], [-657/1000, -9680/1000, -722/1000], align='center', width=1, color='r', label='Ave. task size (MCU cycles)' )

# plt.hist(data_pdf,normed=True)      
# plt.hist(data_bc,normed=True)  
plt.yticks([-10, 0, 10,20,30, 40, 50,60], ['10e4', '0', '10','20','30', '40', '50', '60'])  
plt.legend(loc='best')
f.savefig("../figures/virtualTaskSize.eps",format="eps", dpi=1200)

plt.show()


