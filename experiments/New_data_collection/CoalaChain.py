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

def executionTimeCuckoo(fh):
    """
    Input: file handler 
    return: the average execution time of a program
    """
    lim=0
    numbers = []
    for num in fh:
        if num[-2:-1] == '1' and lim < 10:
            lim+=1;
            #print(num)
            numbers.append( float(num.split(',')[0] ) )
    aDiff = 0
    for i in range(0, len(numbers)-1, 2):
        aDiff += (numbers[i+1] - numbers[i])
#     aDiff *=1000  # make the diff in milliseconds

    time = (aDiff / int(len(numbers)/2) )
    fh.close()
    return time

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

    
distances = ["cont.csv",'20cm.csv', '40cm.csv']

apps = ["bc", "sort", "cuckoo", "dd", "cem", "dft"]
ol = ["-O0", "-O0", "-O1", "-O0", "-O0", "-O0"]

f = plt.figure(figsize=(8,2.6))
figureSetting()    

s=0
for j, app in enumerate(apps):

    for i, dis in enumerate(distances):
        row=[]
        f_v = open("coala_coal/"+ol[j]+"/"+app+"/"+dis) 
        f_n = open("chain/"+ol[j]+"/"+app+"/"+dis) 
        row.append( executionTime(f_v) )
        if(app =="cuckoo"):
            row.append(executionTimeCuckoo(f_n))
        else:
            row.append( executionTime(f_n) )
        f_v.close()
        f_n.close()
        [CA, CH] = plt.bar( np.array([0,0.5])+i+j*len(distances)+s, 
            np.array(row)/float(row[0]), 
            width=0.5, 
            color=['#f7fcb9','#2c7fb8'], 
             linewidth=0.5)

    s+=0.5;
plt.legend([CA, CH], ["Coala", "Chain"])

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator(  (np.arange(len(apps)) * len(distances)) + np.arange(len(distances) , len(apps)+len(distances) ) * 0.5  ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( apps ))

plt.xlim(0,20.5)
plt.ylabel("Norm. runtime")
plt.tight_layout()
plt.legend(loc='upper left')
# # plt.ylim(0,3.2) 
f.savefig("../figures/CoalaChain.eps",format="eps", dpi=1200)
plt.show()









