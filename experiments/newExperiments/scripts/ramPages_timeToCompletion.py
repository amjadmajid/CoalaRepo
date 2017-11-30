#! /usr/bin/python


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



cem      = np.array( [ 13251011,5115064,4283901,4220972,4248560,13867514 ]) 
cuckoo   = np.array( [ 454057, 427650, 457850, 552558, 768143, 1222542 ]) 
ar   	 = np.array( [ 483818,452052,469499,421585,471081,542725 ]) 
dft      = np.array( [ 26058854, 26625854, 25964064, 25928743, 26197667, 26763421]) 
rsa      = np.array( [ 2614059, 1829844, 1849674, 1987705, 2248648, 2910148])    ## the fist result of RSA must be re-produced !! 
bc       = np.array( [ 1100054, 974405, 996122, 1181346, 1623207, 2545090 ]) 
sort     = np.array( [ 9565706, 8552088, 7977442, 7512732, 10099092, 15271824 ]) 



cem     = cem / float( cem[0])
cuckoo  = cuckoo / float( cuckoo[0])
ar     = ar /float( ar[0])
dft     = dft/ float( dft[0])
rsa     = rsa / float( rsa[0])
bc      = bc / float( bc[0])
sort    = sort / float( sort[0])




apps = [ cem, cuckoo, ar, dft, rsa, bc, sort]
appsName = [ "cem", "cuckoo", "ar", "dft", "rsa", "bc", "sort"]
numOfApps = len(apps)

basePag = 32
numPags =  ( basePag , 2 * basePag, 4 * basePag, 8 * basePag, 16 * basePag, 32 * basePag  )   


figureSetting()
f = plt.figure(figsize=(8,2.5))

xlabelsPos=[]
s = 0
barsGroupsGap = 0.04
barWidth = (1.0/len(apps[0]))
barsPos = ( np.arange( len(apps[0]) )/float(len(apps[0])) ) # bars position for a single set of bars
for i in range(numOfApps):
    [p32, p64, p128, p256, p512, p1024] = plt.bar(  barsPos + s ,apps[i], width= barWidth, color='g' )
    
    # add text at the bottom of the bars 
    for i in range( len(apps[i]) ):
    	plt.text( barsPos[i] + s + barWidth/4 , 0.05 , numPags[i] , fontsize=8, color='#d8d8d8', rotation=90, verticalalignment='bottom')

    s+=1+ barsGroupsGap
    xlabelsPos.append(s-0.5- (barsGroupsGap) )



# print(xlabelsPos)

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator(  xlabelsPos ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( appsName ))

# plt.legend([p32, p64, p128, p256, p512, p1024]  , ['32', '64', '128', '256', '512', '1024'], loc='lower center' )
plt.ylabel('Norm. Runtime')
plt.xlim([0,7.235])
plt.tight_layout()
f.savefig("../figures/ramPagsSizes.pdf", format="pdf", dpi=1200)
f.savefig("../figures/ramPagsSizes.pdf", format="pdf", dpi=1200)
plt.show()



