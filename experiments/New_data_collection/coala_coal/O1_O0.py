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

    
distances = ["cont.csv"]

data =[]

apps = ["bc","cem","cuckoo","sort"]
for app in apps:
    
    for dis in distances:
        row=[]
        f_v = open("-O0/"+app+"/"+dis) 
        f_n = open("-O1/"+app+"/"+dis) 
        row.append( executionTime(f_v) )
        row.append( executionTime(f_n) )
        f_v.close()
        f_n.close()
        data.append(row)
data = np.transpose(data)
# print(data)
# exit(0);

f = plt.figure(figsize=(8,4))
figureSetting()                        # Set figure layout

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator(np.array([ 0.5,1.5,2.5,3.5,4.5]) ) )
ax.xaxis.set_major_formatter(ticker.FixedFormatter( apps ))


plt.ylabel('Seconds')


num_bars = len(data)

print(num_bars)

# hatches = ['+', '/']
colors = ['#edf8b1', '#7fcdbb']
labels = ["-0O", "-01"]

gap = 0.1                         # space each bars group 0.1 from the next group
bar_width = (1- gap) / num_bars   # divide the remaining distance equally between the bars          


for i, row in enumerate(data):    # enumerate returns a data unit (a row) and its index
    x = np.arange(len(row))      
    plt.bar(x+i *bar_width, row,       # blue starts at x=0, i=0 => 0
            #                          # green starts at x=0, i=1 => 0.2 
            #                          # red starts at   x=0, i=2 => 0.4
            width=bar_width, 
            color = colors[i],
            label = labels[i])



plt.legend(loc='upper left')
# plt.ylim(0,3.2) 
f.savefig("../../figures/O0O1.eps",format="eps", dpi=1200)
plt.show()









