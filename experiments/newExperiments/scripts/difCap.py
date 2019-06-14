#! /usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
from csf import *

coal = np.array([0.8210784916666664, 0.8251448288043478,  0.8292796825,  0.8634430216346155])
noCoal = np.array([1.2018005568181822,  1.204097878472222, 1.2085183641304347,  1.2322385520833326])

coal = coal[::-1]
noCoal = noCoal[::-1]
c = noCoal/coal

figureSetting()
f = plt.subplots(figsize=(8,2.5))

plt.plot( c,'-o', color='r')
#plt.plot( noCoal, '->', color='g' )

plt.ylabel('Norm. runtime')
plt.xlabel('On-time (ms)')
plt.xticks(np.array(4), ('5', '10', '25','50') )
# plt.xlim([0,6.21])
plt.tight_layout()
f.savefig("../figures/difCap.pdf", format="pdf", dpi=1200)
# f.savefig("../figures/PagingPerformance_64page.pdf", format="pdf", dpi=1200)
plt.show()




