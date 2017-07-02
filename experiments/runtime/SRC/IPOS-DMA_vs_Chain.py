"""
Performance: IPOS versus Chain
    Metric : time
    Task   : Decompressing 100 bytes of data
    App    : data_decompression


@ Author  : Amjad Yousef Majid
@ Gmail ID: amjad.y.majid 
@ Date    : 20/May/2017 
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

    
    
chain_dd = open('../experiments_data/data_decomp/chain/data_decomp_100bytes.csv')
ipos_dd = open('../experiments_data/data_decomp/ipos-dma16/VT_1RT.csv')

chain_dft4 = open('../experiments_data/dft/chain_dft4.csv')

print(chain_dft4)

ipos_dft4 = open('../experiments_data/dft/IPOS_DFT4.csv')

chain_dft8 = open('../experiments_data/dft/chain_dft8.csv')
ipos_dft8 = open('../experiments_data/dft/IPOS_DFT8.csv')



chain_dd_time = executionTime(chain_dd)  # Execution time
ipos_dd_time = executionTime(ipos_dd) 

chain_dft4_time = executionTime(chain_dft4) 
ipos_dft4_time = executionTime(ipos_dft4)

chain_dft8_time = executionTime(chain_dft8)  
ipos_dft8_time = executionTime(ipos_dft8)
#print(chain_time, ipos_time)
# print(ipos_dft8_time,chain_dft8_time )



data = [ [chain_dd_time, ipos_dd_time],
         [chain_dft4_time,ipos_dft4_time ],
         [chain_dft8_time,ipos_dft8_time]
       ]

data = np.transpose(data)
hatches = ['+', '/']
colors = ['0.6', '0.8']
# colors = ['c', 'm']
labels = ["Chain", "IPOS"]


num_bars = len(data)
gap = 0.1   
bar_width = (1- gap) / num_bars            

f = plt.figure(figsize=(8,4))
figureSetting()                        # Set figure layout

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator([0,1, 2]) )
ax.xaxis.set_major_formatter(ticker.FixedFormatter(("Decomp.", "DFT4", "DFT8") ) )

plt.ylabel('Seconds')


for i, row in enumerate(data):    # enumerate returns a data unit (a row) and its index
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
f.savefig("../../figures/ipos_chain.eps",format="eps", dpi=1200)
plt.show()
