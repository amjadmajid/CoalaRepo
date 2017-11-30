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

apps =  appsSelector()
filesContainer = filesFinder(apps, 'simulatedPwrInter/difCap/*/*.csv')

print(apps)
print('---------------')
print(filesContainer)

# exit()


names=[]
times=[]
for appFiles in filesContainer:
    appTimes=[]
    for file in appFiles:
        # l = labelMaker(file)
        # name = nameMaker(file)
        time = executionTime( open( '../data/'+file) )
        appTimes.append(time)
    # names.append(name)
    times.append(appTimes)

# print(times)

# exit()

coal = [0.8210784916666664, 0.8251448288043478,  0.8292796825,  0.8634430216346155]
noCoal = [1.2018005568181822,  1.204097878472222, 1.2085183641304347,  1.2322385520833326]


data = np.transpose([coal, noCoal])


# cem     = cem / float( cem[0])
# cuckoo  = cuckoo / float( cuckoo[0])
# ar     = ar /float( ar[0])
# dft     = dft/ float( dft[0])
# rsa     = rsa / float( rsa[0])
# bc      = bc / float( bc[0])
# sort    = sort / float( sort[0])




# apps = [ cuckoo, ar, dft, rsa, bc, sort]
# appsName = [ "cuckoo", "ar", "dft", "rsa", "bc", "sort"]
# numOfApps = len(apps)

# basePag = 32
# numPags =  ( basePag , 2 * basePag, 4 * basePag, 8 * basePag, 16 * basePag, 32 * basePag  )   


figureSetting()
f = plt.figure(figsize=(8,2.5))

# xlabelsPos=[]
# s = 0
# barsGroupsGap = 0.04
# barWidth = (1.0/len(apps[0]))
# barsPos = ( np.arange( len(apps[0]) )/float(len(apps[0])) ) # bars position for a single set of bars
# for i in range(len(data)):
plt.plot( coal,'-o', color='r')
plt.plot( noCoal, '->', color='g' )

    
    # add text at the bottom of the bars 
    # for i in range( len(apps[i]) ):
    #     plt.text( barsPos[i] + s + barWidth/4 , 0.1 , numPags[i] , fontsize=10, color='w', rotation=90, verticalalignment='bottom')

    # s+=1+ barsGroupsGap
    # xlabelsPos.append(s-0.5- (barsGroupsGap) )



# print(xlabelsPos)

# ax = plt.axes()
# ax.xaxis.set_major_locator(ticker.FixedLocator(  xlabelsPos ))
# ax.xaxis.set_major_formatter(ticker.FixedFormatter( appsName ))

# plt.legend([p32, p64, p128, p256, p512, p1024]  , ['32', '64', '128', '256', '512', '1024'], loc='lower center' )
plt.ylabel('Norm. runtime')
# plt.xlim([0,6.21])
plt.tight_layout()
f.savefig("../figures/difCap.pdf", format="pdf", dpi=1200)
# f.savefig("../figures/PagingPerformance_64page.pdf", format="pdf", dpi=1200)
plt.show()




