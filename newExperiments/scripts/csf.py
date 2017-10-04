import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

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

def set_xaxis(ax, xLocs, labels):
	ax.xaxis.set_major_locator(ticker.FixedLocator( xLocs ))
	ax.xaxis.set_major_formatter(ticker.FixedFormatter( labels ))
