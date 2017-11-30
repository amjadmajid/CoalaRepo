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
    # plt.rcParams['xtick.major.size'] = 7
    # #set size of ticks on y-axis
    # plt.rcParams['ytick.major.size'] = 7

    #set size of markers, e.g., circles representing points
    #set numpoints for legend
    # plt.rcParams['legend.numpoints'] = 1


chain_cont   = [0.355809,    0.091599,    1.651908,   5.771600664062499  , 0.309126,    0.278821,    0.600697]
coal_cont    = [0.56662,     0.049746,    1.213208,    3.5161449166666663 , 0.235423,    0.089789,   0.137456]
              
chain_20   = [0.499513513513513,   0.129282608695652,  2.3233125,  5.772032160714283 , 0.43731914893617,    0.394844444444445,   0.839388888888889]
coal_20    = [0.805625,            0.0685652173913044,  1.70852173913043,    3.517230833333335  , 0.345225,   0.121641025641026,   0.193191489361702]
                        
chain_40   = [1.37189473684211,    0.289142857142857,   4.58725 ,  8.892858390624998 ,             0.976735294117647 ,  0.914209302325582 ,  1.55934146341463]
coal_40    = [1.68121621621622,   0.172515873015873,   2.46094117647059 , 7.607710203125,   0.785947368421053   , 0.31355       ,      0.42868085106383]

    
distances = ["cont.csv",'20cm.csv', '40cm.csv']

apps    =   ['cem', 'cuckoo', 'ar',  'dft', 'rsa', 'bc', 'sort']


data = [ np.transpose([chain_cont, coal_cont]), 
         np.transpose([chain_20, coal_20    ]), 
         np.transpose([chain_40, coal_40 ]) ]

# print(data)
# exit()         

f = plt.figure(figsize=(8,2.5))
figureSetting()    

s=0
for i in range(1,8):

    data = [ coal_cont[i-1]/coal_cont[i-1], chain_cont[i-1]/coal_cont[i-1], 
            coal_20[i-1]/coal_20[i-1], chain_20[i-1]/coal_20[i-1], 
            coal_40[i-1]/coal_40[i-1],  chain_40[i-1]/coal_40[i-1]]
    # print(np.arange(6) + s)
    # exit()

    [us, co, _,__,___,____]= plt.bar( np.arange(6)+0.5 + s, data , 
            width=1, 
            color=['#addd8e','#31a354', '#addd8e','#31a354', '#addd8e','#31a354'], 
             linewidth=0.5)
    s+=6.5

# add text at the bottom of the bars 

pos=np.array([1,3,5])-0.3
lbl = ['0m', '.2m','.4m' ]
d=0
for h in range(7):
    for i in range( 3 ):
        plt.text( pos[i]+d , 0.1 , lbl[i] , fontsize=9, color='#404040', rotation=90, verticalalignment='bottom')
    d+=6.5

plt.legend([us, co], ["Coala", "Chain"], loc='upper left')

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator( np.arange(7)+ 3 + np.arange(7) * 5.5 ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( apps ))


# exit()
plt.yticks([1,2,3,4])
plt.xlim(0,45)
plt.ylabel("Norm. runtime")
plt.tight_layout()
# plt.legend(loc='best')
# # plt.ylim(0,3.2) 
f.savefig("../../figures/coala_chain_clang.pdf",format="pdf", dpi=1200)
f.savefig("../../figures/coala_chain_clang.eps",format="eps", dpi=1200)
plt.show()









