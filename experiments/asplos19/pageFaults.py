#! /usr/bin/python
"""
    Coala: performance
    Metric : time
    Task   : execute apps

@ Author  : Amjad Yousef Majid
@ Gmail ID: amjad.y.majid
@ Date    : 12/April/2018
"""
import numpy as np
import matplotlib.pyplot as plt
from csf import *

appsNames           = [ 'ar', 'bc', 'cuckoo', 'dijkstra', 'fft', 'sort']
pageSizes           = ['32'   ,'64'    ,'128'   , '256'  ]

ar_time             = np.array([4180924, 3830583, 3355085, 3429367])
ar_pagePulls        = np.array([6      ,3       ,2       ,1       ])

bc_time             = np.array([314642, 326114, 350346  , 395894  ])
bc_pagePulls        = np.array([1     ,1      ,1        ,1        ])

cuckoo_time         = np.array([197220, 183920, 185808  , 201444  ])
cuckoo_pagePulls    = np.array([11    ,7      ,4        ,3        ])

dijkstra_time       = np.array([1459916, 1222655, 1065594, 1164196])
dijkstra_pagePulls  = np.array([20     , 10     , 5      , 3      ])

fft_time            = np.array([ 87674 , 79918  , 77066  , 75918  ])
fft_pagePulls       = np.array([ 9     , 5      , 3      , 2      ])

sort_time           = np.array([ 4563329, 3802744, 3196377, 2541895 ])
sort_pagePulls      = np.array([ 7      , 4      , 2      , 1       ])

apps_times     = [ar_time/min(ar_time)  ,bc_time/min(bc_time)  , cuckoo_time/min(cuckoo_time)  , dijkstra_time/min(dijkstra_time)  , fft_time/min(fft_time)  , sort_time /min(sort_time)  ]
apps_pagePulls = [ar_pagePulls  , bc_pagePulls  , cuckoo_pagePulls  , dijkstra_pagePulls, fft_pagePulls , sort_pagePulls    ]

figureSetting()
f = plt.figure(figsize=(8,2.8))

totalGroupBarsWidth = 0.9
numberOfBars        = 4
barWitdth           = totalGroupBarsWidth/numberOfBars
shift=0

for i in range(len(appsNames)):
    app_time = np.array(apps_times[i])/min(apps_times[i])
    plt.bar( np.arange(numberOfBars)*barWitdth +shift , app_time, 
        width=barWitdth, 
        color='#08519c', 
        edgecolor='w', 
        align='edge')

    for i, pagesize in enumerate(pageSizes):
    	plt.text( barWitdth*i+shift+barWitdth/4-0.05, -0.08 , pagesize , fontsize=11, color='#08519c', verticalalignment='top', rotation=60)
    shift+=1

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator( np.arange(shift)+(barWitdth*numberOfBars)/2))
ax.xaxis.set_major_formatter(ticker.FixedFormatter(appsNames))
ax.set_facecolor('white')

plt.ylabel("Normalized runtime")
plt.xlim([0,5.9])
ax.tick_params(axis='x', pad=30, bottom=False)
plt.tight_layout()
f.savefig("../../paper/asplos19/figures/page_exec-time.pdf",format="pdf", dpi=1200)
f.savefig("../../paper/asplos19/figures/page_exec-time.eps",format="eps", dpi=1200)


f = plt.figure(figsize=(8,2.8))

shift=0
for i in range(len(appsNames)):
    
    plt.bar( np.arange(numberOfBars)*barWitdth +shift , 
        apps_pagePulls[i], 
        width=barWitdth, 
        color='#08306b', 
        edgecolor='w', 
        align='edge')

    for i, pagesize in enumerate(pageSizes):
        plt.text( barWitdth*i+shift+barWitdth/4-0.05, -0.8 , pagesize , fontsize=11, color='#08519c', verticalalignment='top', rotation=60)


    shift+=1

ax = plt.axes()
ax.set_facecolor('white')
ax.xaxis.set_major_locator(ticker.FixedLocator( np.arange(shift)+(barWitdth*numberOfBars)/2))
ax.xaxis.set_major_formatter(ticker.FixedFormatter(appsNames))
ax.yaxis.set_major_locator(ticker.FixedLocator( [0,4,8,12,16,20]))
ax.yaxis.set_major_formatter(ticker.FixedFormatter([0,4,8,12,16,20]))
plt.xlim([0,5.9])
plt.ylabel("No. page faults")
ax.tick_params(axis='x', pad=30, bottom=False)
# plt.ylabel([0,4,8,12,16,20])
plt.tight_layout()
f.savefig("../../paper/asplos19/figures/pagePulls.pdf",format="pdf", dpi=1200)
f.savefig("../../paper/asplos19/figures/pagePulls.eps",format="eps", dpi=1200)


plt.show()
