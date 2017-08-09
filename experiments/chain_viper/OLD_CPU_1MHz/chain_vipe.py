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


apps = ['DataDecomp/decomp_255var.csv',
        'DFT_float/8/dft_float.csv',
        'DFT_float/16/dft_float.csv',
        'Sort/50var/50var.csv']

data =[]
for app in apps:
    row=[]
    f_c = open("Chain/"+app)
    f_v = open("vipe/"+app)
    row.append( executionTime(f_c) )
    row.append( executionTime(f_v) )
    f_c.close()
    f_v.close()
    data.append(row)

# print(data)
data = np.transpose(data)
colors = ['0.2', '0.4', '0.6', '0.8']
labels = ["Chain", "VIPE"]


num_bars = len(data)
gap = 0.1
bar_width = (1- gap) / num_bars

f = plt.figure(figsize=(8,4))
figureSetting()                        # Set figure layout

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator([0,1,2,3]) )
ax.xaxis.set_major_formatter(ticker.FixedFormatter( ('DataDecomp', 'DFT8', 'DFT16', 'Sort') ))

plt.ylabel('Seconds')


for i, row in enumerate(data):    # enumerate returns a data unit (a row) and its index
    print('row', row)
    x = np.arange(len(row))
    plt.bar((x-0.35)+i *bar_width, row,
            width=bar_width,
            color=colors[i % len(colors)],
           label = labels[i% len(labels)],
           align='center')
# plt.bar( data)
# plt.bar(1, chain_dd_time, align='center', color="0.6", hatch='+', label="Chain")
# plt.bar(2, ipos_dd_time, align='center', color="0.85", hatch='/', label="IPOS")

plt.legend(loc='best')
f.savefig("../../figures/chain_vipe.eps",format="eps", dpi=1200)
plt.show()
