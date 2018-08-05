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
no_coal = 		[ [94.32, 0, 5.68]  , [96.51, 0, 3.49]	, [78.67, 4.26, 17.06]  , [78.66, 7.38, 13.96] , [93.19, 1.57, 5.24], [94.98, 0, 5.02] ] 
coal = 			[ [95.62, 0, 4.38]  , [97.96, 0, 2.04] 	, [87.59, 5.57,	 6.84]  , [84.66, 8.59,  6.75] , [92.81, 1.26, 5.93], [97.47, 0, 2.53] ] 
aware_coal = 	[ [95.58, 0, 4.12]  , [98.09, 0, 1.91] 	, [87.68,  5.6,  6.72]  , [84.98, 8.64,	 6.38] , [92.15, 1.58, 6.27], [97.57, 0, 2.43] ]

stacklevel = 	[ [0,0,0], [0,0,0],[0,0,0],[0,0,0],[0,0,0],[0,0,0] ]

times = ( [no_coal, coal, aware_coal])
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

plt.legend([lp, sp, fp], ["last SRAM page", "other SRAM page", "FRAM page"], loc='lower center',  bbox_to_anchor=(0.5, -0.025),  ncol=3,   fontsize= 14, framealpha=1, facecolor='white')
plt.ylabel("Mem. accesses [%]")
plt.xlim([0,5.9])
plt.tight_layout()
f.savefig("../../paper/asplos19/figures/memAccess.pdf",format="pdf", dpi=1200)
f.savefig("../../paper/asplos19/figures/memAccess.eps",format="eps", dpi=1200)
plt.show()
exit()

