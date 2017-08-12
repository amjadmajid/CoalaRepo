"""
    VIPER: Page size overhead 
    Metric : relative MCU cycles and relative number of page faults
    Task   : execute certain apps


@ Author  : Amjad Yousef Majid
@ Gmail ID: amjad.y.majid 
@ Date    : 2/Aug/2017 
"""
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


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

    #set size of markers, e.g., circles representing points
    #set numpoints for legend
    plt.rcParams['legend.numpoints'] = 1




apps_names=[
    "cem",
    "cuckoo",
    'ar',
    "dft",
    'rsa',
    "bc",
    "sort", 

]


data_page = [
    np.array([8498,7354,5367,5379,5344,5295,5237,5236])/(8498/3),
    np.array([759, 137, 130,49,39,36,0,0]) /(759/3)  ,
    np.array([15528, 9388,  8825,  8549,  5141,  0, 0, 0])/(15528/3) ,
    np.array([3249,3225,3201,3177,1953,100,0,0])/(3249/3),
    np.array([4671,  4575,  4288,  4486,  3488,  2895,  0, 0])/(4671/3),
    np.array([602, 0, 0,0,0,0,0,0])/(602/3),
    np.array([5350,4430,3510,2782,0,0,0,0])/(5350/3) 
]



f = plt.figure(figsize=(8,2))
figureSetting()   


s=0
for d in data_page:
  plt.bar( np.arange(len(d)) + s, d, color='r')
  s+=8.5 

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator([4,12.5,21,29.5,38, 47, 57]) )
ax.xaxis.set_major_formatter(ticker.FixedFormatter(apps_names ) )


plt.yticks([0, 2, 4])  

plt.ylabel('Norm. page faults')

plt.tight_layout()
f.savefig("../figures/pagSizeOverhead.eps",format="eps", dpi=1200)
f.savefig("../figures/pagSizeOverhead.pdf",format="pdf", dpi=1200)
plt.show()




