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
filesContainer = filesFinder(apps, 'simulatedPwrInter_1500/TimeToCompletion/*/*.csv')

# print(apps)
# print('---------------')
print(filesContainer)
names=[]
times=[]
for appFiles in filesContainer:
    appTimes=[]
    for file in appFiles:
        l = labelMaker(file)
        name = nameMaker(file)
        time = executionTime( open( '../data/user/'+file) )
        appTimes.append(time)
    names.append(name)
    times.append(appTimes)

# print(times)

figureSetting()
f = plt.figure(figsize=(8,2.5))

s=0
for i, row in enumerate(times):

        [fst, slw] = plt.bar( np.array([0,0.5]) +i+s, 
            np.array(row)/float(row[0]), 
            width=0.5, 
            color=['#addd8e','#31a354'], 
             linewidth=0.5)
        s+=0.3

plt.legend([fst, slw], ["Fast Changing", "Slow Changing"])

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator(  [0.5, 1.8, 3.1, 4.4, 5.7, 7 ] ))
ax.xaxis.set_major_formatter(ticker.FixedFormatter( names ))

# plt.xlim(0,s-0.1)
plt.ylabel("Norm. runtime")
plt.tight_layout()
plt.legend(loc='best')
# plt.ylim(0,3.2) 
f.savefig("../figures/fast_slow_algos.pdf",format="pdf", dpi=1200)
plt.show()




