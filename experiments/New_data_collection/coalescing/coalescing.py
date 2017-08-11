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

    
distances = ["cont.csv",'20cm.csv', '40cm.csv']
apps = ["bc","cem", "sort", 'ar', 'rsa', 'dft']

f = plt.figure(figsize=(8,2.6))
figureSetting()    










ar = [1.213208,  1.422938, 1.70852173913043, 1.78106060606061, 2.46094117647059 ,3.50057894736842]
rsa = [0.235423, 0.252849, 0.345225, 0.352205882352941, 0.785947368421053, 0.579828571428571]   
dft = [0.7795645673, 0.87543120, 0.78435398, 0.92709883,  4.063, 3.967868623 ]


s=0
for j, app in enumerate(apps[0:3]):

    for i, dis in enumerate(distances):
        row=[]
        f_v = open("coalescing/page_128/"+app+"/"+dis) 
        f_n = open("nocoalescing/page_128/"+app+"/"+dis) 
        row.append( executionTime(f_v) )
        row.append( executionTime(f_n) )
        f_v.close()
        f_n.close()
        [CA, NO] = plt.bar( np.array([0,0.5])+i+j*len(distances)+s, 
            np.array(row)/float(row[0]), 
            width=0.5, 
            color=['#f7fcb9', '#addd8e'], 
             linewidth=0.5)
    s+=0.5;

for i in [0,2,4]:
    row=[]
    row.append(ar[i])
    row.append(ar[i+1])
    [CA, NO] = plt.bar( np.array([0,0.5])+i/2+9+s, 
    np.array(row)/float(row[0]), 
    width=0.5, 
    color=['#f7fcb9', '#addd8e'], 
     linewidth=0.5)

s+=0.5;

for i in [0,2,4]:
    row=[]
    row.append(rsa[i])
    row.append(rsa[i+1])
    [CA, NO] = plt.bar( np.array([0,0.5])+i/2+12+s, 
    np.array(row)/float(row[0]), 
    width=0.5, 
    color=['#f7fcb9', '#addd8e'], 
     linewidth=0.5)

s+=0.5;

for i in [0,2,4]:
    row=[]
    row.append(dft[i])
    row.append(dft[i+1])
    [CA, NO] = plt.bar( np.array([0,0.5])+i/2+15+s, 
    np.array(row)/float(row[0]), 
    width=0.5, 
    color=['#f7fcb9', '#addd8e'], 
     linewidth=0.5)

s+=0.5;

plt.legend([CA, NO], ["Coalescing", "No coalescing"])


ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator(  (np.arange(len(apps)) * len(distances)) + np.arange(len(distances) , len(apps)+len(distances) ) * 0.5  ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( apps ))


plt.xlim(0,20.5)
plt.ylabel("Norm. runtime")
plt.tight_layout()
plt.legend(loc='upper left')

f.savefig("../../figures/coalescing.pdf",format="pdf", dpi=1200)
f.savefig("../../figures/coalescing.eps",format="eps", dpi=1200)
plt.show()



exit()


