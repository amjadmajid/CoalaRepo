"""
Task Merging: 
    Metric  : time
    Task    : Merge Tasks
    App    : Data decompression


@ Author  : Amjad Yousef Majid
@ Gmail ID: amjad.y.majid 
@ Date    : 20/May/2017 
"""

import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

data_points =11

diffs=[]
for f_idx in range(1,data_points):

    fh = open('../experiments_data/task_merging/emptyTasks/VT_{}RT.csv'.format(f_idx) , 'r')
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


f= plt.figure(figsize=(8,4)  )
plt.plot(range(1,data_points),diffs, 'g-o', 
         markeredgewidth = 1.5, markerfacecolor = '0.75',
         markeredgecolor = 'g', markersize = 10, label='IPOS')

plt.xlabel('Virtual Task size')
plt.ylabel('Milliseconds')
plt.tight_layout()
plt.legend()

f.savefig("../../figures/virtualTaskSize.eps",format="eps", dpi=1200)
plt.show()