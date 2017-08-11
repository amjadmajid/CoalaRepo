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

apps        =["cem",    "cuckoo",   "ar",       "rsa",      "bc",       "sort"]   
user        =[0.709372,  0.074199 ,   1.764156,    0.310371,    0.124502,    0.23026]
comp        =[0.632916,  0.078538,    3.286627,    0.511581,    0.143884,    0.245992]
# user_coal   =[0.56662   0.049746    1.555144    0.293449    0.089789    0.137456]
# comp_coal   =[0.475009  0.061497    2.800946    0.49123     0.10746     0.149785]

data = [user,  comp] #  user_coal  comp_coal ]

data =  np.transpose(data)

f = plt.figure(figsize=(8,2.6))
figureSetting()    

s=0
for i, row in enumerate(data):

        [us, co] = plt.bar( np.array([0,0.5]) +i+s, 
            np.array(row)/float(row[0]), 
            width=0.5, 
            color=['#7fcdbb','#2c7fb8'], 
             linewidth=0.5)
        s+=0.3

plt.legend([us, co], ["User", "Compiler"])

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator(  [0.5, 1.8, 3.1, 4.4, 5.7, 7 ] ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( apps ))

plt.xlim(0,7.5)
plt.ylabel("Norma. runtime")
plt.tight_layout()
plt.legend(loc='upper left')
# # plt.ylim(0,3.2) 
f.savefig("../../figures/CompUser.eps",format="eps", dpi=1200)
plt.show()









