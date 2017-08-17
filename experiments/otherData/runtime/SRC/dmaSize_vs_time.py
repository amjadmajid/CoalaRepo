"""
DMA: Block size
    Metric : time
    Task   : Transfer block of data
    App    : TI APP


@ Author  : Amjad Yousef Majid
@ Gmail ID: amjad.y.majid 
@ Date    : 20/May/2017 
"""

import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

def executionTime(fh, diffs):
    numbers = []
    for num in fh:
        if num[-2:-1] == '1':
            #print(num)
            numbers.append( float(num.split(',')[0] ) )
    aDiff = 0
    for i in range(0, len(numbers)-1, 2):
        aDiff += (numbers[i+1] - numbers[i])
    aDiff *=1000  # make the diff in milliseconds
    
    
    diffs.append(aDiff / int(len(numbers)/2) )
#     print(diffs[:10])
#     exit()
    fh.close()

data_points =8

dma_diffs=[]
pt_diffs =[]

dmaSizes = [16, 32, 48, 64, 80, 96, 112, 128]
for f_idx in range(data_points):
    fh = open('../experiments_data/ipos-dma-size/dma{}.csv'.format(dmaSizes[f_idx]) , 'r')
    executionTime(fh, dma_diffs)
    
for f_idx in range(data_points):
    fh = open('../experiments_data/ipos-dma-size/whileBased/byte{}.csv'.format(dmaSizes[f_idx]) , 'r')
    executionTime(fh, pt_diffs)
    
    
#set figure linewidth
plt.rcParams['axes.linewidth']=2

#set graph line width
plt.rcParams['lines.linewidth'] = 2

#set font size for titles 
#plt.rcParams['axes.titlesize'] = 16

#set font size for labels on axes
plt.rcParams['axes.labelsize'] = 16

#set size of numbers on x-axis
plt.rcParams['xtick.labelsize'] = 16
#set size of numbers on y-axis
plt.rcParams['ytick.labelsize'] = 16

#set size of ticks on x-axis
plt.rcParams['xtick.major.size'] = 7
#set size of ticks on y-axis
plt.rcParams['ytick.major.size'] = 7

#set size of markers, e.g., circles representing points
#set numpoints for legend
plt.rcParams['legend.numpoints'] = 1


f= plt.figure(figsize=(8,4) )
plt.plot(range(data_points),dma_diffs, 'g-o', 
         markeredgewidth = 1.5, markerfacecolor = '0.75',
         markeredgecolor = 'g', markersize = 10, label='DMA')

plt.plot(range(data_points),pt_diffs, 'b:v', 
         markeredgewidth = 1.5, markerfacecolor = '0.75',
         markeredgecolor = 'b', markersize = 10, label='SW')

plt.xlabel('Data block size')
plt.ylabel('milliseconds')
plt.tight_layout()
plt.legend(loc='upper left')

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator(( range(9) ) ) )
ax.xaxis.set_major_formatter(ticker.FixedFormatter(dmaSizes) )
plt.grid(True)
f.savefig("../../figures/dmaSize_time.eps",format="eps", dpi=1200)
plt.show()