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

names = 		[   'ar'    		,    'bc'    		,  'cuckoo'  			, 'dijkstra' 		   , 'fft'   				,   'sort'  ]
nc = [[63.54,19.84,16.62],  [38.25,46.11,15.64] , [21.41,66.12,12.47],  [27.74,57.83,14.42], [68.15,3.62,28.24], [28.63,53.03,18.34]]
sc = [[74.6,	6.22, 19.18] ,  [63.24, 11.2, 25.57], [54.07, 17.95, 27.97], [57.19, 14.99, 27.82], [68.44, 2.77, 28.79], [53.8, 12.64, 33.57]]
ea = [[75.47, 5, 19.53],  [63.32, 11.07, 25.61] , [54.57, 17.13, 28.29],  [58.22, 13.54, 28.23], [69.92, 2.11, 27.97], [54.37, 11.75, 33.89]]


stacklevel = 	[ [0,0,0], [0,0,0],[0,0,0],[0,0,0],[0,0,0],[0,0,0] ]

times = ( [nc, sc, ea])
figureSetting()
f = plt.figure(figsize=(8,2.8))

totalGroupBarsWidth = 0.9
numberOfBars        = 3
barWitdth           = totalGroupBarsWidth/numberOfBars
shift=0
c=['#08519c', '#4292c6', '#9ecae1']

# print(times)
# exit()
for i, row in enumerate(times):
	# print(row)
	# exit()
	shift = 0
	for bar in  row:
		lp =plt.bar(barWitdth*i + shift, bar[0],
			color=c[0],
			width=barWitdth, 
			linewidth=0.5, 
			align='edge', 
			edgecolor="w")
		sp =plt.bar(barWitdth*i + shift, bar[1],
			color=c[1], 
			width=barWitdth,
			bottom=bar[0], 
			linewidth=0.5, 
			align='edge', 
			edgecolor="w")
		fp =plt.bar(barWitdth*i + shift, bar[2], 
			color=c[2],
			width=barWitdth,
			bottom=bar[0]+bar[1], 
			linewidth=0.5, 
			align='edge', 
			edgecolor="w")
		shift+=1

# add text at the bottom of the bars 
pos=np.array([0,1,2,3])*barWitdth
lbl = ['NC', 'EG','WEG' ]

d=barWitdth/3
for h in range(len(names)):
    for i in range( numberOfBars ):
        plt.text( pos[i]+d-0.05 , -2 , lbl[i] , fontsize=11, color=c[0], verticalalignment='top', rotation=60)
    d+=1

ax = plt.axes()
ax.tick_params(axis='x', pad=25, bottom=False)

ax.xaxis.set_major_locator(ticker.FixedLocator( np.arange(shift)+ (barWitdth*numberOfBars)/2 ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( names ))
ax.set_facecolor('white')
					

plt.legend([lp, sp, fp], ["user code", "transitions/commits", "memory access"], loc='lower center',  bbox_to_anchor=(0.5, -0.025),  ncol=3,   fontsize= 14, framealpha=1, facecolor='white')
plt.ylabel("MCU utilization [%]")
plt.xlim([0,5.9])
plt.tight_layout()
f.savefig("../../paper/asplos19/figures/overallOverhead.pdf",format="pdf", dpi=1200)
f.savefig("../../paper/asplos19/figures/overallOverhead.eps",format="eps", dpi=1200)
plt.show()
exit()

