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
filesContainer = filesFinder(apps, 'simulatedPwrInter/TimeToCompletion/*/*.csv')

# print(apps)
# print('---------------')
# print(filesContainer)

# exit()

names=[]
times=[]
for appFiles in filesContainer:
    appTimes=[]
    for file in appFiles:
        l = labelMaker(file)
        name = nameMaker(file)
        time = executionTime( open( '../data/'+file) )
        appTimes.append(time)
    names.append(name)
    times.append(appTimes)

print(times)

# exit()

figureSetting()
f = plt.figure(figsize=(8,2.5))

s=0
for i, row in enumerate(times):

        [slw,fst, smt] = plt.bar( np.array([0,0.3,0.6]) +s, 
            np.array( [row[1], row[0], row[2]] )/float(row[1]), 
            width=0.3, 
            color=['#addd8e','#31a354','#61a654'], 
             linewidth=0.5)
        s+=1

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator( np.arange(s)+0.5 ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( names ))

# plt.xlim(0,s-0.1)
plt.ylabel("Norm. Runtime")
plt.yticks([0.0, 0.4, 0.8, 1.2])
plt.tight_layout()
plt.legend([smt, fst, slw], ["TSHC", "HC", "NONE"], loc='lower left',     fontsize= 12)
# plt.ylim(0,3.2) 
f.savefig("../figures/coalStrategies.pdf",format="pdf", dpi=1200)
f.savefig("../figures/coalStrategies.eps",format="eps", dpi=1200)
plt.show()




