#!/home/amjed/anaconda3/bin/python
import numpy as np 
import matplotlib.pyplot as plt 
from matplotlib import style
import copy
style.use('seaborn-muted')

def remOutLiers(voltagesIn):
	voltages = copy.deepcopy( voltagesIn )
	voltages = np.array(voltages).astype(np.float) 
	stdVolt_3 = np.std( voltages ) * 2
	meanVolt = np.mean( voltages )
	voltages = list(voltages)
	# print('3 * std = ', stdVolt_3)
	for i, volt in enumerate(voltages):
		if np.abs(volt - meanVolt) > stdVolt_3:
			print('del', voltages[i] )
			del voltages[i] 

	return voltages


def voltList(file ):
	"""
		@input: file name 
		@output: list of voltages

		voltList extract the voltages values 
		from a the EDB output
	 """
	voltages=[]
	for line in open(file):
		# get the voltages and remove the `\n` char
		voltages.append( (line.split(',')[-1])[:-1] ) 
	
	return(voltages)

def statistics(voltages, numLines):
	""" 
		@input: voltages list, number instructions between each voltage probes
		@output: mean and standard deviation 
	"""
	diffs=[]
	list_len = len(voltages)
	for i in range(0,list_len, 2):
		diffs.append( (( (float(voltages[i])/numLines - float(voltages[i+1])/numLines ) \
								 **2) * 10e-6 * 0.5 ) )
	diffsClean = remOutLiers(diffs)
	return(np.mean(diffsClean), np.std(diffsClean))

def plot(xAxis, voltMeans, symb, labels):
	plt.rcParams['axes.linewidth'] =  2
	plt.rcParams['axes.labelsize'] =  16
	plt.rcParams['xtick.labelsize'] =  16
	plt.rcParams['ytick.labelsize'] =  16
	plt.rcParams['lines.linewidth'] =  2
	f = plt.figure(figsize=(8,4))
	i=0
	for val in voltMeans:
		plt.plot( xAxis ,val, symb[i], label=labels[i], markeredgewidth=1.5, markerfacecolor='0.75', markeredgecolor='k')
		i+=1
	plt.legend(loc='best')
	plt.xlabel("Words")
	plt.ylabel("pico joule")
	plt.grid(True)
	plt.tight_layout()
	f.savefig("../../../../figures/energyConsumptionDMA_SW.eps", format="eps", dpi=1200)
	plt.show()


def main():
	_5_dma = [  "../outputs/15_dma16.txt","../outputs/15_dma32.txt", "../outputs/15_dma48.txt", "../outputs/15_dma64.txt", 
				"../outputs/15_dma80.txt" , "../outputs/15_dma96.txt" ]

	_5_pt =[  "../outputs/15_pt16.txt",  "../outputs/15_pt32.txt", "../outputs/15_pt48.txt", "../outputs/15_pt64.txt",
			 "../outputs/15_pt80.txt" , "../outputs/15_pt96.txt" ]
	xAxis = [16, 32, 48, 64,80, 96]
	dma_volt_mean=[]
	pt_volt_mean=[]

	for i in range(len(_5_dma)):
		numInstructions=[15] * len(_5_dma) # -1) +[5]
		dma_volt_mean.append(  statistics( voltList(_5_dma[i]), numInstructions[i] )[1] )
		pt_volt_mean.append( statistics( voltList(_5_pt[i]), numInstructions[i] )[1] )

		dma_volt_mean_scaled = np.array(dma_volt_mean) * 10**12
		pt_volt_mean_scaled  = np.array(pt_volt_mean) * 10**12

	plot(xAxis ,[dma_volt_mean_scaled, pt_volt_mean_scaled] , ['k-o','k:>'], ['DMA','SW'] )

if __name__ == '__main__':
	main()



























