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

def autolabel(rects, _):
    """
    Attach a text label above each bar displaying its height
    """

    labels = ["cont", "20cm","40cm","60cm"]
    for i, rect in enumerate(rects):
        height = rect.get_height()
        ax.text(rect.get_x()+0.001 + rect.get_width()/2.,height+0.1,'%s' % (labels[i]),
                ha='center', va='bottom',  rotation=90, color='0.4')


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

apps = ["bc","cuckoo", "sort"]
dataSet=[]
for app in apps:
    data =[]
    for dis in distances:
        row=[]
        f_v = open("coalescing/page_128/"+app+"/"+dis) 
        f_n = open("chain_gcc/"+app+"/"+dis) 
        row.append( executionTime(f_v) )
        row.append( executionTime(f_n) )
        f_v.close()
        f_n.close()
        data.append(row)
    data = np.transpose(data)
    dataSet.append(data)

f = plt.figure(figsize=(8,4))
figureSetting()                        # Set figure layout

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator([1.75, 1.75+4.5, 1.75+4.5+4.5]) )
ax.xaxis.set_major_formatter(ticker.FixedFormatter( ["bc","cuckoo", "sort"] ))


plt.ylabel('Seconds')


num_bars = len(data)
# hatches = ['+', '/']
colors = ['#edf8b1', '#7fcdbb']
labels = ["IPE", "Chain"]

gap = 0.1                         # space each bars group 0.1 from the next group
bar_width = (1- gap) / num_bars   # divide the remaining distance equally between the bars          

bar1=[]

shift=0
for data in dataSet:
    for i, row in enumerate(data):    # enumerate returns a data unit (a row) and its index
        x = np.arange(len(row))   

        bar1.append(plt.bar(x+i *bar_width+shift, row,       # blue starts at x=0, i=0 => 0
                #                          # green starts at x=0, i=1 => 0.2 
                #                          # red starts at   x=0, i=2 => 0.4
                width=bar_width, 
                # hatch = hatches[i % len(hatches)],
                color =  colors[i % len(colors)])) 
    shift+=4.5

shift=0
# to set the labels only for one iteration!!
data = dataSet[0]
for i, row in enumerate(data):    # enumerate returns a data unit (a row) and its index
    x = np.arange(len(row))   

    plt.bar(x+i *bar_width+shift, row,       # blue starts at x=0, i=0 => 0
            #                          # green starts at x=0, i=1 => 0.2 
            #                          # red starts at   x=0, i=2 => 0.4
            width=bar_width, 
            # hatch = hatches[i % len(hatches)],
            color =  colors[i % len(colors)], 
            label = labels [i % len(labels)]) 
    shift+=4.5


# add the page sizes
# for i in range(6):
#     autolabel(bar1[i], i)

plt.legend(loc='upper left')
# plt.ylim(0,3.2) 
f.savefig("../figures/ipeChain.eps",format="eps", dpi=1200)
plt.show()









