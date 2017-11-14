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

############### page size of 128 bytes ##############

# cem      = np.array( [ 3691384,       2081138,    2658959 ]) 
# cuckoo   = np.array( [ 443089,        513159,     622667  ]) 
# crc      = np.array( [ 82567,         84605,      86641   ]) 
# dft      = np.array( [ 26267755,      26010302,   26254522]) 
# rsa      = np.array( [ 2196414,       1771543,    2260314 ]) 
# bc       = np.array( [ 831246,        849276,     868791  ]) 
# sort     = np.array( [ 12242871,      7765337,    7920915 ]) 

############### page size of 64 bytes ##############

# cem      = np.array( [ 3249640,       1869253,    2205335 ]) 
# cuckoo   = np.array( [ 382723,        440148,     552546  ]) 
# crc      = np.array( [ 70087,         72061,      74033   ]) 
# dft      = np.array( [ 26372167,      26306501,   25931295]) 
# rsa      = np.array( [ 2277448,       2070869,    1815520 ]) 
# bc       = np.array( [ 713362,        726396,     745847  ]) 
# sort     = np.array( [ 13915418,      9599409,    8834494 ]) 



# cem      = np.array( [ 3691384,       2081138,    2658959 ]) 
cuckoo   = np.array( [ 454057, 427650, 457850, 552558, 768143, 1222542 ]) 
dft      = np.array( [ 26058854, 26625854, 25964064, 25928743, 26197667, 26763421]) 
rsa      = np.array( [ 1929844, 1829844, 1849674, 1987705, 2248648, 2910148])    ## the fist result of RSA must be re-produced !! 
bc       = np.array( [ 1100054, 974405, 996122, 1181346, 1623207, 2545090 ]) 
sort     = np.array( [ 9565706, 8552088, 7977442, 7512732, 10099092, 15271824 ]) 



# cem     = cem / float( cem[0])
cuckoo  = cuckoo / float( cuckoo[0])
# crc     = crc /float( crc[0])
dft     = dft/ float( dft[0])
rsa     = rsa / float( rsa[0])
bc      = bc / float( bc[0])
sort    = sort / float( sort[0])




apps = [ cuckoo,  dft, rsa, bc, sort]
appsName = [ "cuckoo",  "dft", "rsa", "bc", "sort"]
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
    	plt.text( barsPos[i] + s + barWidth/4 , 0.1 , numPags[i] , fontsize=10, color='w', rotation=90, verticalalignment='bottom')

    s+=1+ barsGroupsGap
    xlabelsPos.append(s-0.5- (barsGroupsGap) )



# print(xlabelsPos)

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator(  xlabelsPos ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( appsName ))

# plt.legend([p32, p64, p128, p256, p512, p1024]  , ['32', '64', '128', '256', '512', '1024'], loc='lower center' )
plt.ylabel('Norm. rantime')
plt.xlim([0,5.2])
plt.tight_layout()
f.savefig("../figures/ramPagsSizes.pdf", format="pdf", dpi=1200)
# f.savefig("../figures/PagingPerformance_64page.pdf", format="pdf", dpi=1200)
plt.show()



