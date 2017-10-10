import matplotlib.pyplot as plt
import numpy as np
import matplotlib.mlab as mlab
import matplotlib.ticker as ticker
import os 
import glob
import random
import colors

colorCnts=0

def generateRandomColor():
	global colorCnts
	c  = colors.colors[colorCnts]
	colorCnts+=1
	colorCnts %=len(colors.colors)
	return (c)


dataUser = '../data/user/'

def filesFinder(path):
	filesPaths = []
	os.chdir( dataUser )
	appsMainFolders = glob.glob('*')  # grep all apps folders
	for appFolder in appsMainFolders:
		filesPaths.append( glob.glob( os.path.join(appFolder,path)) )
	os.chdir('../../scripts/')

	validPaths=[]
	validApps=[]
	for i, p in enumerate(filesPaths):
		if p != []:
			validPaths.append(p)
			validApps.append(appsMainFolders[i])

	return (validApps, validPaths)


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

def normPlotting(d, c):
	if len(d) < 1:
		return
	mu = np.mean(d)
	sigma = np.std(d)
	x = np.linspace(mu-4*sigma,mu+4*sigma, 100)
	
	if sigma ==0 :
		return False
	plt.plot(x,mlab.normpdf(x, mu, sigma), color=c, lw=1.5)
	return True

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
