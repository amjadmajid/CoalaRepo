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
from csf import *

figureSetting()


coala_15  = np.array([0.786335 , 0.456814 , 0.278125 , 0.349900 , 0.487500 , 0.367956])
alpaca_15 = np.array([0.693665 , 1.000000 , 0.306250 , 0.555723 , 1.000000 , 0.740847])
     
coala_30  = np.array([0.788232 , 0.471374 , 0.319018 , 0.385218 ,     0.62  , 0.378595])
alpaca_30 = np.array([0.675560 , 1.000000 , 0.300613 , 0.552129 ,     0    , 0.715679])

coala_50  = np.array([0.858665 , 0.471374 , 0.308869 , 0.357457 ,     1.52  , 0.388675]) 
alpaca_50 = np.array([0.672112 , 1.000000 , 0.299694 , 0.556479 ,     0    , 0.733026])
    
apps   = [   'ar'    ,    'bc'    ,  'cuckoo'  , 'dijkstra' ,    'fft'   ,   'sort'  ]


coala_15  = coala_15/alpaca_15
alpaca_15 = alpaca_15/alpaca_15

coala_30[0:4]  = coala_30[0:4]/alpaca_30[0:4]
alpaca_30[0:4] = alpaca_30[0:4]/alpaca_30[0:4]
coala_30[5]  = coala_30[5]/alpaca_30[5]
alpaca_30[5] = alpaca_30[5]/alpaca_30[5]


coala_50[0:4]  = coala_50[0:4]/alpaca_50[0:4]
alpaca_50[0:4] = alpaca_50[0:4]/alpaca_50[0:4]
coala_50[5]  = coala_50[5]/alpaca_50[5]
alpaca_50[5] = alpaca_50[5]/alpaca_50[5]


data = np.transpose( [alpaca_15, coala_15, alpaca_30, coala_30, alpaca_50, coala_50 ]) 


# print(data)
# exit()         

f = plt.figure(figsize=(8,2.8))
figureSetting()    

totalGroupBarsWidth =   0.9
numberOfBars        =   6
barWitdth           =totalGroupBarsWidth/numberOfBars

shift=0
for i in range(1,7):
    [us, co, _,__,___,____]= plt.bar( (np.arange(numberOfBars)/numberOfBars)*totalGroupBarsWidth +shift , data[i-1] ,
            width=barWitdth , 
            color=['#a1d99b','#08519c', '#a1d99b','#08519c', '#a1d99b','#08519c'], 
             linewidth=0.5, align='edge')
    shift+=1


# add text at the bottom of the bars 
pos=np.array([0,2,4])*barWitdth
lbl = ['.15', '.30','.50' ]

d=0
for h in range(numberOfBars):
    for i in range( 3 ):

        plt.text( pos[i]+d , -0.033 , lbl[i] , fontsize=11, color='#08519c', verticalalignment='top', rotation=60)
    d+=1



plt.text( barWitdth  *26+ 0.386 , 0.03 , r'$\infty$' , fontsize=17, color='k', verticalalignment='bottom')
plt.text( barWitdth  *28+ 0.386, 0.03 , r'$\infty$' , fontsize=17, color='k', verticalalignment='bottom')

ax = plt.axes()
ax.tick_params(axis='x', bottom=False)
ax.set_facecolor('white')

plt.legend([us, co], [ "Alpaca", "Coala" ], loc='lower center', bbox_to_anchor=(0.5, -0.025), ncol=2, fontsize=14, framealpha=1, facecolor='white')

ax = plt.axes()
ax.tick_params(axis='x', pad=25)
ax.xaxis.set_major_locator(ticker.FixedLocator( np.arange(shift)+  (barWitdth*numberOfBars)/2 ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( apps ))


plt.xlim(0,5.9)
plt.ylabel("Norm. runtime")
plt.tight_layout()
f.savefig("../../paper/asplos19/figures/coala_alpaca_gcc.pdf",format="pdf", dpi=1200)
f.savefig("../../paper/asplos19/figures/coala_alpaca_gcc.eps",format="eps", dpi=1200)
plt.show()









