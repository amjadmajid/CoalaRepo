"""
IPOS versions: performance
    Metric : time
    Task   : manipulate and commit a certain number of variables

@ Author  : Amjad Yousef Majid
@ Gmail ID: amjad.y.majid 
@ Date    : 2/July/2017 
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

data_points =7

dma_diffs=[]
ipos_ml_diffs =[]
ipos_pl_diffs =[]

indx = [1, 2, 4, 8, 16, 32, 64, 128]

for f_idx in range(data_points):
    fh = open('../DMA_128words/{}var.csv'.format(indx[f_idx]) , 'r')
    executionTime(fh, dma_diffs)
    fh = open('../ipos-ml/{}var.csv'.format(indx[f_idx]) , 'r')
    executionTime(fh, ipos_ml_diffs)
    if f_idx != 7: 
        fh = open('../ipos-pl/{}var.csv'.format(indx[f_idx]) , 'r')
        executionTime(fh, ipos_pl_diffs)
        

# ipos_pl_diffs.append(0)
    
#set figure linewidth
# plt.rcParams['axes.linewidth']=2

#set graph line width
# plt.rcParams['lines.linewidth'] = 2

#set font size for titles 
#plt.rcParams['axes.titlesize'] = 16

#set font size for labels on axes
plt.rcParams['axes.labelsize'] = 16

#set size of numbers on x-axis
plt.rcParams['xtick.labelsize'] = 14
#set size of numbers on y-axis
plt.rcParams['ytick.labelsize'] = 14

#set size of ticks on x-axis
# plt.rcParams['xtick.major.size'] = 7
#set size of ticks on y-axis
# plt.rcParams['ytick.major.size'] = 7

#set size of markers, e.g., circles representing points
#set numpoints for legend
plt.rcParams['legend.numpoints'] = 1


f= plt.figure(figsize=(8,4))
plt.plot(range(data_points),dma_diffs, 'g-o', linewidth=2,
         markeredgewidth = 1.5, markerfacecolor = '0.75',
         markeredgecolor = 'g', markersize = 7, label='DMA')

plt.plot(range(data_points),ipos_ml_diffs, 'b-.v', linewidth=2,
         markeredgewidth = 1.5, markerfacecolor = '0.75',
         markeredgecolor = 'b', markersize = 7, label='ML')

plt.plot(range(data_points),ipos_pl_diffs, 'r:D', linewidth=2,
         markeredgewidth = 1.5, markerfacecolor = '0.75',
         markeredgecolor = 'r', markersize = 7, label='PL')

plt.xlabel('Number of variables')
plt.ylabel('milliseconds')
plt.tight_layout()
plt.legend(loc='best')

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator(( range(data_points) ) ) )
ax.xaxis.set_major_formatter(ticker.FixedFormatter(indx) )



sub_axes = plt.axes([0.15,0.37,0.4, 0.2])
xVal = [1,2,4]
sub_axes.plot(xVal,dma_diffs[0:3], 'g-o', linewidth=1.5,
         markeredgewidth = 1.5, markerfacecolor = '0.75',
         markeredgecolor = 'g', markersize = 5, label='DMA')
sub_axes.plot(xVal,ipos_ml_diffs[0:3], 'b:v', linewidth=1.5,
         markeredgewidth = 1.5, markerfacecolor = '0.75',
         markeredgecolor = 'b', markersize = 5, label='ML')
sub_axes.plot(xVal,ipos_pl_diffs[0:3], 'r-.D', linewidth=1.5,
         markeredgewidth = 1.5, markerfacecolor = '0.75',
         markeredgecolor = 'r', markersize = 5, label='PL')

sub_axes.xaxis.set_major_locator(ticker.FixedLocator( xVal  ) )
sub_axes.xaxis.set_major_formatter(ticker.FixedFormatter([1,2,4]) )
sub_axes.yaxis.set_major_locator(ticker.FixedLocator( []  ) )
sub_axes.yaxis.set_major_formatter(ticker.FixedFormatter([]) )


f.savefig("../figures/iposCommitSize.eps",format="eps", dpi=1200)
plt.show()


















