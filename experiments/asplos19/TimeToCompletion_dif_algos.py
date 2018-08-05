#! /usr/bin/python


"""
    Coala: performance 
    Metric : time
    Task   : execute apps


@ Author  : Amjad Yousef Majid
@ Gmail ID: amjad.y.majid 
@ Date    : 15/April/2018 
"""
import numpy as np
import matplotlib.pyplot as plt
from csf import *

names = [   'ar'    ,    'bc'    ,  'cuckoo'  , 'dijkstra' ,    'fft'   ,   'sort'  ]
no_coal = 		[ 1.000000, 0.790787, 1.000000, 1.000000, 0.581250, 1.000000] 
coal = 			[ 0.786832 , 0.452975 , 0.268750 , 0.358434 , 0.587500 , 0.360166] 
aware_coal = 	[ 0.786335 , 0.456814 , 0.278125 , 0.349900 , 0.487500 , 0.367956]

times = np.transpose( [no_coal, coal, aware_coal])
figureSetting()
f = plt.figure(figsize=(8,2.8))

totalGroupBarsWidth = 0.9
numberOfBars        = 3
barWitdth           = totalGroupBarsWidth/numberOfBars
shift=0

for i, row in enumerate(times):
        [slw,fst, smt] = plt.bar( np.arange(numberOfBars) * barWitdth + shift, 
            np.array( [row[0], row[1], row[2]] )/float(row[0]), 
            width=barWitdth, 
            color=['#9ecae1','#4292c6','#08519c'], 
             linewidth=0.5, align='edge')
        shift+=1

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator( np.arange(shift)+ (barWitdth*numberOfBars)/2 ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( names ))

plt.xlim([0,5.9])
plt.ylabel("Norm. runtime")
# plt.yticks([0.00, 0.25, 0.50, 0.75,  1.00])
plt.tight_layout()

plt.legend([slw, fst, smt], ['NC', 'HG','WHG' ], loc='lower center',  ncol=3,   fontsize= 14)

f.savefig("../../paper/asplos19/figures/coalStrategies.pdf",format="pdf", dpi=1200)
f.savefig("../../paper/asplos19/figures/coalStrategies.eps",format="eps", dpi=1200)
plt.show()




