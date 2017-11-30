#!/usr/bin/python


"""
    IPOS: performance 
    Metric : time
    Task   : execute apps


@ Author  : Amjad Yousef Majid
@ Gmail ID: amjad.y.majid 
@ Date    : 17/October/2017 
"""
import numpy as np
import matplotlib.pyplot as plt
from csf import *









cem      = np.array( [ 77,39,21,16-1,197-6,5286-1992 ]) 
cuckoo   = np.array( [ 9, 5, 3, 2, 1, 0 ]) 
ar   	 = np.array( [ 4,2,1,0,0,0 ]) 
dft      = np.array( [ 8, 4, 2, 1, 0, 0]) 
rsa      = np.array( [ 8, 4, 2, 1, 0, 0])   
bc       = np.array( [ 0,0,0,0,0,0 ]) 
sort     = np.array( [ 6, 3, 1, 0, 0, 0 ]) 

# cem      = np.array( [ 0,0,0,1,6,1992  ]) 



# cem     = cem / float( cem[0])
# cuckoo  = cuckoo / float( cuckoo[0])
# ar     = ar /float( ar[0])
# dft     = dft/ float( dft[0])
# rsa     = rsa / float( rsa[0])
# bc      = bc / float( bc[0])
# sort    = sort / float( sort[0])




apps = [ cem, cuckoo, ar, dft, rsa, bc, sort]
appsName = [ "cem", "cuckoo", "ar", "dft", "rsa", "bc", "sort"]
numOfApps = len(apps)

basePag = 32
numPags =  ( basePag , 2 * basePag, 4 * basePag, 8 * basePag, 16 * basePag, 32 * basePag  )   

# zeroPagFalt = ['0', '0', ]

figureSetting()
f = plt.figure(figsize=(8,2.5))

xlabelsPos=[]
s = 0
barsGroupsGap = 0.04
barWidth = (1.0/len(apps[0]))
barsPos = ( np.arange( len(apps[0]) )/float(len(apps[0])) ) # bars position for a single set of bars
zeros = []
for i in range(numOfApps):
    [p32, p64, p128, p256, p512, p1024] = plt.bar(  barsPos + s ,apps[i], width= barWidth, color='r', bottom=0.01 )

    for j, ap in enumerate(barsPos):
    	if apps[i][j] == 0:
    		plt.text(ap+s+0.025, 0.015, 'X')

    s+=1+ barsGroupsGap
    xlabelsPos.append(s-0.5- (barsGroupsGap) )

# Cem full page swap
plt.bar(  barsPos ,np.array( [ 0,0,0,1,6,1992  ]) , width= barWidth, color='b', bottom=cem )



# print(xlabelsPos)

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator(  xlabelsPos ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( appsName ))

plt.yscale('log')
# plt.legend([p32, p64, p128, p256, p512, p1024]  , ['32', '64', '128', '256', '512', '1024'], loc='lower center' )
plt.ylabel('# Page Faults')
plt.xlim([0,7.235])
# ax.set_ylim(ymin=0)
plt.tight_layout()
f.savefig("../figures/pagefault.pdf", format="pdf", dpi=1200)
plt.show()



