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
    "sort"
]

row_cuc = np.array([1203109, 420241, 380692,147507, 148331, 513967, 805221,1257204])
row_cuc_norm = row_cuc / np.min(row_cuc)

row_cem = np.array([8549451,5388765,3626039,3814352,4608503, 6421565, 10096537,17589788])
row_cem_norm = row_cem / np.min(row_cem)


row_ar = np.array([ 188089,  152107,  139285,  146280,  134494,  162834,  139537,  179792])
row_ar_norm = row_ar / np.min(row_ar)

row_dft = np.array( [28438580,28458614,28511341,28572634,28464076,28075348,28308658,28880930])
row_dft_norm = row_dft / np.min(row_dft)

row_rsa = np.array( [514148, 318774 , 309972,  301252 , 207486 , 1412470 ,1540923 ,1797342])
row_rsa_norm = row_rsa / np.min(row_rsa)



row_bc = np.array([788926, 615698, 638402, 660397,732470, 911575, 1274839, 1961663])
row_bc_norm = row_bc / np.min(row_bc)

row_sort = np.array( [2937186,2734324,2495083,2317925,1243372,1566328,2219896,3460253])
row_sort_norm = row_sort / np.min(row_sort)




data_muc = [row_cem_norm, row_cuc_norm, row_ar_norm, row_dft_norm, row_rsa_norm,  row_bc_norm,row_sort_norm]



f = plt.figure(figsize=(8,2))
figureSetting()   


s=0
b1=[]

for d in data_muc:
  b1.append( plt.bar( np.arange(len(d)) + s, d, color='y') )
  s+=8.5


ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator([4,12.2,21,29,37.5, 46.5, 54.5]) )
ax.xaxis.set_major_formatter(ticker.FixedFormatter(apps_names ) )


def autolabel(rects):
    """
    Attach a text label above each bar displaying its height
    """

    labels = ['16','32','48', '64','128', '256', '512', '1024']
    for i, rect in enumerate(rects):
        height = rect.get_height()
        ax.text(rect.get_x()+0.1 + rect.get_width()/2.,height+0.1,'%s' % (labels[i]), fontsize=10,
                ha='center', va='bottom',  rotation=90, color='0.4')


autolabel(b1[0]);

plt.yticks([0, 2, 4,6,8])  

plt.ylabel('Norm. runtime')

plt.tight_layout()
f.savefig("../figures/pagSizeOverhead.eps",format="eps", dpi=1200)
f.savefig("../figures/pagSizeOverhead.pdf",format="pdf", dpi=1200)
plt.show()
