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

    
    
decomp_dma      = open('../data_decompression/dma_decomp_100B.csv')
decomp_vob_ml   = open('../data_decompression/vob_memLimitted_100B.csv')
decomp_vob_pl   = open('../IPOS-versions-performance/data_decompression/vob_perforLimitted_100B.csv')

sel_dma         = open('../selectionSort/dma_sort_20num.csv')
sel_vob_ml      = open('../selectionSort/vob_memLimitted_sort_20num.csv')
sel_vob_pl      = open('../IPOS-versions-performance/selectionSort/vob_perfLimitted_sort_20num.csv')

bd_dma         = open('../largeDataCommit/DMA_commit_50B.csv')
bd_vob_ml      = open('../largeDataCommit/memLimit_commit_50B.csv')
bd_vob_pl      = open('../largeDataCommit/perfLimit_commit_50B.csv')


decomp_dma_time     = executionTime(decomp_dma) 
decomp_vob_ml_time  = executionTime(decomp_vob_ml) 
decomp_vob_pl_time  = executionTime(decomp_vob_pl) 

sel_dma_time     = executionTime(sel_dma) 
sel_vob_ml_time  = executionTime(sel_vob_ml) 
sel_vob_pl_time  = executionTime(sel_vob_pl) 

bd_dma_time     = executionTime(bd_dma) 
bd_vob_ml_time  = executionTime(bd_vob_ml) 
bd_vob_pl_time  = executionTime(bd_vob_pl) 


data = [ [decomp_dma_time,  decomp_vob_ml_time,     decomp_vob_pl_time],
         [sel_dma_time,     sel_vob_ml_time,        sel_vob_pl_time],
         [bd_dma_time,      bd_vob_ml_time,         bd_vob_pl_time] ]


data = np.transpose(data)
hatches = ['o','+', '/']
colors = ['0.4', '0.6', '0.8']
labels = ["DMA", "VOB_ml", "VOB_pl"]


num_bars = len(data)
gap = 0.1   
bar_width = (1- gap) / num_bars            

f = plt.figure(figsize=(8,4))
figureSetting()                        # Set figure layout

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator([0,1,2]) )
ax.xaxis.set_major_formatter(ticker.FixedFormatter(("Decomp", "Sel", "BData") ) )

plt.ylabel('Seconds')


for i, row in enumerate(data):    # enumerate returns a data unit (a row) and its index
    print('row', row)
    x = np.arange(len(row))      
    plt.bar((x-0.23)+i *bar_width, row,    
            width=bar_width, 
            hatch = hatches[i % len(hatches)],
            color=colors[i % len(colors)], 
           label = labels[i% len(labels)], 
           align='center')

# plt.bar( data)
# plt.bar(1, chain_dd_time, align='center', color="0.6", hatch='+', label="Chain")
# plt.bar(2, ipos_dd_time, align='center', color="0.85", hatch='/', label="IPOS")

plt.legend(loc='upper left')
f.savefig("../figures/executionTime.eps",format="eps", dpi=1200)
plt.show()