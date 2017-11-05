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

numPags =  ("single",  "double",  "triple" )   

############### page size of 128 bytes ##############

cem      = np.array( [ 3691384,       2081138,    2658959 ]) 
cuckoo   = np.array( [ 443089,        513159,     622667  ]) 
crc      = np.array( [ 82567,         84605,      86641   ]) 
dft      = np.array( [ 26267755,      26010302,   26254522]) 
rsa      = np.array( [ 2196414,       1771543,    2260314 ]) 
bc       = np.array( [ 831246,        849276,     868791  ]) 
sort     = np.array( [ 12242871,      7765337,    7920915 ]) 

############### page size of 64 bytes ##############

# cem      = np.array( [ 3249640,       1869253,    2205335 ]) 
# cuckoo   = np.array( [ 382723,        440148,     552546  ]) 
# crc      = np.array( [ 70087,         72061,      74033   ]) 
# dft      = np.array( [ 26372167,      26306501,   25931295]) 
# rsa      = np.array( [ 2277448,       2070869,    1815520 ]) 
# bc       = np.array( [ 713362,        726396,     745847  ]) 
# sort     = np.array( [ 13915418,      9599409,    8834494 ]) 


cem     = cem / float( cem[0])
cuckoo  = cuckoo / float( cuckoo[0])
crc     = crc /float( crc[0])
dft     = dft/ float( dft[0])
rsa     = rsa / float( rsa[0])
bc      = bc / float( bc[0])
sort    = sort / float( sort[0])




apps = [cem, cuckoo, crc, dft, rsa, bc, sort]
appsName = ["cem", "cuckoo", "crc", "dft", "rsa", "bc", "sort"]

figureSetting()
f = plt.figure(figsize=(8,2.5))

xlabelsPos=[]
s = 0
for i in range(7):
    [single, double, triple] = plt.bar(np.array([0, 0.3, 0.6])+s,apps[i], width=0.3, color=['#addd8e','#31a354','#61a654'], )
    s+=0.1
    s+= 0.3*3
    xlabelsPos.append(s-0.5 )

# print(xlabelsPos)

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator(  xlabelsPos ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( appsName ))

plt.legend([single, double, triple]  , ['single', 'double', 'triple'], loc='lower center' )
plt.ylabel('Norm. rantime')
plt.tight_layout()
f.savefig("../figures/PagingPerformance_128Page.pdf", format="pdf", dpi=1200)
# f.savefig("../figures/PagingPerformance_64page.pdf", format="pdf", dpi=1200)
plt.show()



