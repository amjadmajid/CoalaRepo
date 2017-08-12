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
    "dft",
    "bc",
    "sort"
]

row_cuc = np.array([1203109, 420241, 380692,147507, 148331, 513967, 805221,1257204])
row_cuc_norm = row_cuc / np.min(row_cuc)

row_cem = np.array([8549451,5388765,3626039,3814352,4608503, 6421565, 10096537,17589788])
row_cem_norm = row_cem / np.min(row_cem)

row_bc = np.array([788926, 615698, 638402, 660397,732470, 911575, 1274839, 1961663])
row_bc_norm = row_bc / np.min(row_bc)

row_sort = np.array( [2937186,2734324,2495083,2317925,1243372,1566328,2219896,3460253])
row_sort_norm = row_sort / np.min(row_sort)

row_dft = np.array( [28438580,28458614,28511341,28572634,28464076,28075348,28308658,28880930])
row_dft_norm = row_dft / np.min(row_dft)


data_muc = [row_cem_norm, row_cuc_norm, row_dft_norm, row_bc_norm,row_sort_norm]



f = plt.figure(figsize=(8,2))
figureSetting()   


s=0
for d in data_muc:
  plt.bar( np.arange(len(d)) + s, d, color='y')
  s+=8.5 

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator([4,12.5,21,29.5,38]) )
ax.xaxis.set_major_formatter(ticker.FixedFormatter(apps_names ) )


plt.yticks([0, 2, 4,6,8])  

plt.ylabel('Norm. runtime')

plt.tight_layout()
f.savefig("../figures/pagSizeOverhead.eps",format="eps", dpi=1200)
f.savefig("../figures/pagSizeOverhead.pdf",format="pdf", dpi=1200)
plt.show()
