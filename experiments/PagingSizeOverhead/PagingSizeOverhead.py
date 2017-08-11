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


apps_names=[
    "Cuckoo",
    "CEM",
    "BC",
    "Sort"
]

row_cuc = np.array([1203109, 420241, 380692,147507, 148331, 513967, 805221,1257204])
row_cuc_norm = row_cuc / np.min(row_cuc)

row_cem = np.array([8549451,5388765,3626039,3814352,4608503, 6421565, 10096537,17589788])
row_cem_norm = row_cem / np.min(row_cem)

row_bc = np.array([788926, 615698, 638402, 660397,732470, 911575, 1274839, 1961663])
row_bc_norm = row_bc / np.min(row_bc)

row_sort = np.array( [2937186,2734324,2495083,2317925,1243372,1566328,2219896,3460253])
row_sort_norm = row_sort / np.min(row_sort)

data_muc = [row_cuc_norm, row_cem_norm,row_bc_norm,row_sort_norm]

data_page = [
    np.array([759, 137, 130,49,39,36,0,0]) /(759/3)  ,
    np.array([8498,7354,5367,5379,5344,5295,5237,5236])/(8498/3),
    np.array([602, 0, 0,0,0,0,0,0])/(602/3),
    np.array([5350,4430,3510,2782,0,0,0,0])/(5350/3)
]

dataSet = [data_muc, -np.array(data_page)]

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
    
def autolabel(rects, i):
    """
    Attach a text label above each bar displaying its height
    """

    labels = ["16", "32","48","64","128","256","512","1024"]
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+0.01 + rect.get_width()/2.,height+0.1,'%d' % int(labels[i]),
                ha='center', va='bottom',  rotation=90, color='0.2')

          

f = plt.figure(figsize=(8,3.5))
figureSetting()                        # Set figure layout

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator([0,1,2,3]) )
ax.xaxis.set_major_formatter(ticker.FixedFormatter(apps_names ) )

dataSet = [np.transpose(data_muc), -np.transpose(data_page) ]

num_bars = 8
gap = 0.1   
bar_width = (1- gap) / num_bars 

colors=['y', 'r']
cIdx=0
bar1 = []
for data in dataSet:
    for i, row in enumerate(data):    # enumerate returns a data unit (a row) and its index
        x = np.arange(len(row))  
        bar1.append( plt.bar((x-0.35)+i *bar_width, row,    
                              width=bar_width,
                              color = colors[cIdx], align='center', edgecolor = "w") )
    cIdx+=1;

# add the page sizes
for i in range(8):
    autolabel(bar1[i], i)

    
plt.annotate('norm. overhead', fontsize=14,
             xytext = (2,6),  # text location
             xy=(1.5,3.5),        # arrows points to 
             ha = 'left',    # horizontal alignment 
             va = 'bottom',    # vertical alignment
             arrowprops={'facecolor': '0.8', 'shrink' : 0.1}, 
            )

plt.annotate('norm. page faults', fontsize=14,
             xytext = (1.9,-2.9),  # text location
             xy=(1.7,-0.9),        # arrows points to 
             ha = 'left',    # horizontal alignment 
             va = 'top',    # vertical alignment
             arrowprops={'facecolor': '0.8', 'shrink' : 0.1}, 
            )
plt.tight_layout()
# plt.ylim(-4,3)    
plt.yticks([-4, -2, 1, 4,7,10], ['10e4', '10e2', '0','5','10', '15',  '20'])  
f.savefig("../figures/pagSizeOverhead.eps",format="eps", dpi=1200)
f.savefig("../figures/pagSizeOverhead.pdf",format="pdf", dpi=1200)
plt.tight_layout()
plt.show()