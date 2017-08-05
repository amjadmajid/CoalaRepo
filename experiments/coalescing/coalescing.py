"""
    IPOS: performance 
    Metric : time
    Task   : execute certain apps


@ Author  : Amjad Yousef Majid
@ Gmail ID: amjad.y.majid 
@ Date    : 30/June/2017 
"""
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


def executionTime(fh):
    """
    Input: file handler 
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
    plt.rcParams['xtick.major.size'] = 7
    #set size of ticks on y-axis
    plt.rcParams['ytick.major.size'] = 7

    #set size of markers, e.g., circles representing points
    #set numpoints for legend
    plt.rcParams['legend.numpoints'] = 1

    
distances = ["cont.csv",'20cm.csv', '40cm.csv', "60cm.csv"]

data =[]
for dis in distances:
    row=[]
    f_c = open("coalescing/page_128/bc/"+dis) 
    f_n = open("nocoalescing/page_128/bc/"+dis) 
    row.append( executionTime(f_c) )
    row.append( executionTime(f_n) )
    f_c.close()
    f_n.close()
    data.append(row)

# print(data)
# data = np.transpose(data)
colors = ['0.2', '0.4', '0.6', '0']
labels = ['20cm', '40cm', '60cm', 'cpwr']
          
print(data) 

f = plt.figure(figsize=(8,4))
figureSetting()                        # Set figure layout

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator([1,2,3,4]) )
ax.xaxis.set_major_formatter(ticker.FixedFormatter( ("COAL", "NOCOAL") ))

plt.ylabel('Seconds')

plt.bar(np.array([1,2,3,4]) , data)

plt.legend(loc='best')
f.savefig("../figures/coalescing.eps",format="eps", dpi=1200)
plt.show()