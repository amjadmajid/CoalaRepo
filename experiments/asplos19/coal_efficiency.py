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

nc	= [ [77.74, 2, 20.26]	, [ 52.81, 0.21, 46.98]	, [ 29.61, 0.15, 70.24]	, [37.66, 0.15, 62.19]	, [74.26, 21.81, 3.93]	, [ 38.37, 0.1, 61.53] ]
sc	= [ [ 85,14.43,0.57]	, [74.42, 25.09, 0.49]	, [ 85.08,13.24, 1.68]	, [77.97, 20.47, 1.56]	, [67.39, 31.22, 1.38]	, [79.46, 19.64, 0.91] ]
ea	= [ [93.27, 3.02, 3.71 ], [91.91, 1.38, 6.71 ]	, [89.65, 0.3, 10.05 ]	, [89.64, 0.56, 9.81]	, [76.66, 21.04, 2.3 ]	, [91.28, 0.31, 8.4 ] ]
eta	= [ [94.23, 2.95, 2.82]	, [94.4,  1.31, 4.29]	, [91.84, 0.75, 7.41]	, [91.29, 1.89, 6.82]	, [85.18, 12.49, 2.33 ]	, [93.59, 0.97, 5.43 ] ]
		


stacklevel = 	[ [0,0,0,0], [0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0] ]

times = ( [nc, sc, ea, eta])
figureSetting()
f = plt.figure(figsize=(8,2.8))

totalGroupBarsWidth = 0.9
numberOfBars        = 4
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
		uc =plt.bar(barWitdth*i + shift, bar[0],
			color=c[0],
			width=barWitdth, 
			linewidth=0.5, 
			align='edge', 
			edgecolor="w")
		rp =plt.bar(barWitdth*i + shift, bar[1],
			color=c[1], 
			width=barWitdth,
			bottom=bar[0], 
			linewidth=0.5, 
			align='edge', 
			edgecolor="w")
		co =plt.bar(barWitdth*i + shift, bar[2], 
			color=c[2],
			width=barWitdth,
			bottom=bar[0]+bar[1], 
			linewidth=0.5, 
			align='edge', 
			edgecolor="w")
		shift+=1
	# plt.show()
	# exit()

# add text at the bottom of the bars 
pos=np.array([0,1,2,3])*barWitdth
lbl = ['NC', 'EO','HG', 'WHG' ]

d=barWitdth/5
for h in range(len(names)):
    for i in range( numberOfBars ):

        plt.text( pos[i]+d-0.05 , -2 , lbl[i] , fontsize=11, color=c[0], verticalalignment='top', rotation=60)
    d+=1


ax = plt.axes()
ax.tick_params(axis='x', pad=25, bottom=False)
ax.xaxis.set_major_locator(ticker.FixedLocator( np.arange(shift)+ (barWitdth*numberOfBars)/2 ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( names ))

ax.set_facecolor('white')

plt.legend([uc, rp, co], ["task code", "re-execution", "commit"], loc='lower center',  bbox_to_anchor=(0.5, -0.025), ncol=3,   fontsize= 14, framealpha=1, facecolor='white')
plt.ylabel("MCU utilization [%]")
plt.xlim([0,5.9])
plt.tight_layout()
f.savefig("../../paper/asplos19/figures/coalEfficiency.pdf",format="pdf", dpi=1200)
f.savefig("../../paper/asplos19/figures/coalEfficiency.eps",format="eps", dpi=1200)
plt.show()
exit()

