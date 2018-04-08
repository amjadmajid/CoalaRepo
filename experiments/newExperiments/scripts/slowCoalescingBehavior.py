#! /usr/bin/python


"""
    IPOS: performance 
    Metric : time
    Task   : execute apps


@ Author  : Amjad Yousef Majid
@ Gmail ID: amjad.y.majid 
@ Date    : 8/April/2017 
"""
import numpy as np
import matplotlib.pyplot as plt
from csf import *

coalTaskSize = [1,2,3,4,5, 6,5,4,3, 2,3,4]

figureSetting()
f = plt.figure(figsize=(8,2.5))



plt.plot(coalTaskSize)

data_len = len(coalTaskSize)
for i in range(data_len-1):
    if coalTaskSize[i+1] > coalTaskSize[i]:
        plt.plot(i, coalTaskSize[i], 'o', color='g')
    else:
        plt.plot(i, coalTaskSize[i], 'X', color='r')



plt.annotate('Repeated failures',
             fontsize=14,
             xytext = (5,1.2),  # text location
             xy=(5,5.8), 
                    # arrows points to 
             ha = 'center',    # horizontal alignment 
             va = 'bottom',    # vertical alignment
             arrowprops={'facecolor': 'k', 'width': 2, "headwidth":7, 'shrink' : 0.01}, 
            )

plt.annotate("", 
             fontsize=14,
             xytext = (5,2),  # text location
             xy=(5.9,4.8), 
                    # arrows points to 
             ha = 'center',    # horizontal alignment 
             va = 'bottom',    # vertical alignment
             arrowprops={'facecolor': 'k', 'width': 2, "headwidth":7, 'shrink' : 0.01}, 
            )

plt.annotate("", xytext = (5,2),  # text location
             fontsize=14,
             xy=(6.9,3.8), 
                    # arrows points to 
             ha = 'center',    # horizontal alignment 
             va = 'bottom',    # vertical alignment
             arrowprops={'facecolor': 'k', 'width': 2, "headwidth":7, 'shrink' : 0.01}, 
            )

plt.text(0.7,5 , 'successful commit', fontsize=14, color='k', rotation=26) 

plt.ylabel("Coal. Task Size")
plt.xlabel("Time")
plt.tight_layout()
f.savefig("../figures/slowCoal.pdf",format="pdf", dpi=1200)
f.savefig("../figures/slowCoal.eps",format="eps", dpi=1200)
plt.show()




