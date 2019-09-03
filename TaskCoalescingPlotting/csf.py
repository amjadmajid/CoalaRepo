"""
This module is supporting library for other scripts
"""

import matplotlib.pyplot as plt
import numpy as np
import matplotlib.mlab as mlab
import matplotlib.ticker as ticker
import os
import glob
import random
# import colors
from collections import defaultdict
import sys

#plt.style.use('seaborn-darkgrid')

# to reset the original style use `matplotlib.rcdefaults()` after the plot

colorCnts=0

def generateRandomColor():
	global colorCnts
	c  = colors.colors[colorCnts]
	colorCnts+=1
	colorCnts %=len(colors.colors)
	return (c)

def unified_input(aStr=""):
	user_input =None
	if( sys.version_info[0] ) < 3 :
		user_input = raw_input(aStr)
	else:
		user_input = input(aStr)
	return user_input


dataUser = '../data/'
scriptsDir = '../scripts/'

def appsRootDirs():
	os.chdir(dataUser)
	return glob.glob('*')  # grep all apps folders


def appsSelector():
	apps = appsRootDirs()

	disp_apps = apps[:]
	# print(disp_apps)
	user_choice=[]

	while(1):

		user_input = unified_input('to choose from '+str(disp_apps)+' or (done):')

		if(user_input == 'done'):
			break;
		disp_apps.remove(user_input)
		user_choice.append(user_input)

	if user_choice !=[]:
		apps = user_choice[:]
	os.chdir(scriptsDir)

	return apps

def filesFinder(apps, path):
	filesPaths = []
	appsMainFolders = apps
	os.chdir(dataUser)
	for appFolder in appsMainFolders:
		filesPaths.append( glob.glob( os.path.join(appFolder,path)) )

	os.chdir(scriptsDir)
	validPaths=[]
	validApps=[]
	for i, p in enumerate(filesPaths):
		if p != []:
			validPaths.append(p)
			validApps.append(appsMainFolders[i])

	return (validPaths)


def dataMiner(fileName, memStart, memEnd, wordSize):
	"""
		This function extract data from a file formated in intel hex format
		@ inputs:
			filename
			The start/end of the memory section in hex
			wordSize: The size of word in bytes (2,4)
		@ output:
			an array of data in in int base 10 format
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

def mspProfiler_dataMiner( apps, paths ):
	"""
	@input: relative paths to data files extracted from
			mspProfiler library
	@output: { app:{task:values, ....}, .... }
	"""
	task_sizes= defaultdict(list)
	apps_size={}
	for app, path in zip(apps, paths):

		file = dataUser+path[0]  # first path
		for line in open(file):
			try:
				(taskId, value) = line.split(" ")
			except:
				continue

			task_sizes[app+'_'+taskId].append( int(value[:-1], 16) ) # lists of a task sizes for repeated executions
		apps_size[app] = task_sizes.copy()
		task_sizes.clear()
	return apps_size

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
    #plt.rc('text', usetex=True)
    #plt.rcParams['font.family'] = [ 'Tahoma','DejaVu Sans','Lucida Grande', 'Verdana']
    #set figure linewidth
    plt.rcParams['axes.linewidth']=1

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
    plt.rcParams['legend.fontsize'] = 16

    gray = "444444"
    plt.rcParams['axes.facecolor'] = 'white'
    plt.rcParams['axes.edgecolor'] = gray


def set_xaxis(ax, xLocs, labels):
	ax.xaxis.set_major_locator(ticker.FixedLocator( xLocs ))
	ax.xaxis.set_major_formatter(ticker.FixedFormatter( labels ))


def executionTime(fh):
    """
    Input: .csv file handler of saleae.com logic analyzer
    return: the average execution time of a program
    """
    numbers = []
    for num in fh:
        if num[-2:-1] == '1':
            #print(num)
            numbers.append( float(num.split(',')[0] ) )
    aDiff = 0
    for i in range(0, len(numbers)-1, 2):
        aDiff += (numbers[i+1] - numbers[i])
#     aDiff *=1000  # make the diff in milliseconds

    time = (aDiff / int(len(numbers)/2) )
    fh.close()
    return time


def labelMaker(s):
	if 'fast' in s:
		return 'fast'
	elif 'slow' in s:
		return 'slow'
	elif 'smart' in s:
		return 'smart'
	else:
		return "????"


def nameMaker(s):
	name = s.split("/")[0]
	return name


def remove_outliers(arr):
	m  = np.mean(arr)
	sd = np.std(arr)
	narr = np.array(arr)
	narr = narr[ np.abs(narr-m) < (3*sd) ]
	return narr











