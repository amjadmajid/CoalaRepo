import numpy as np 
import matplotlib.pyplot as plt 
import matplotlib.patches as patches 

from logicAnalyzerData import LogicAnalyzerData
from plotter import Plotter
from csf import *

lad_47 = LogicAnalyzerData('sort_47uf_duty_cycle_12-9_extra-long.csv')
lad_470 = LogicAnalyzerData('sort_470uf_duty_cycle_12-79_extra-long.csv')
plt.figure(figsize=(8,2.5))

base = 600.9
signal_length = 1.6
zoom_in_controll = 0.055
shift = .9+zoom_in_controll  # for alignment 
coal_tasks_470 = lad_470.getData(base,base+signal_length,[0])
lad_470.intervalDataInterpolation(0.001) # in-place data manipulation 

coal_tasks = lad_47.getData(base-shift,base-shift+signal_length,[0])
lad_47.intervalDataInterpolation(0.001) # in-place data manipulation 
static_tasks =    lad_47.getData(base-shift+zoom_in_controll+.192452,base-shift+zoom_in_controll+.23,[2])
lad_47.intervalDataInterpolation(0.001) # in-place data manipulation 
coalTask_detail = lad_47.getData(base-shift+zoom_in_controll+.192452,base-shift+zoom_in_controll+.23,[0])
lad_47.intervalDataInterpolation(0.001) # in-place data manipulation 

coal_tasks_470  = Plotter(coal_tasks_470 )

coal_tasks = Plotter(coal_tasks)
static_tasks = Plotter(static_tasks)
coalTask_detail = Plotter(coalTask_detail)

coal_tasks_470_x,coal_tasks_470_y = coal_tasks_470.getData()

coal_tasks_x,coal_tasks_y = coal_tasks.getData()
static_tasks_x,static_tasks_y = static_tasks.getData()
coalTask_detail_x,coalTask_detail_y = coalTask_detail.getData()

# plt.plot(coal_tasks_470_x,coal_tasks_470_y * .2)

# plt.plot(coal_tasks_x+shift,coal_tasks_y * .2 + 0.21)

figureSetting()


plt.ylim([-0.1,.7])

# sub_axes = plt.axes([.14,.6,.74,.3])
# sub_axes.get_xaxis().set_visible(False)
# sub_axes.get_yaxis().set_visible(False)
plt.plot(static_tasks_x-base+shift-.245,static_tasks_y* .5, color='#4292c6')
plt.plot(coalTask_detail_x-base+shift-.245,coalTask_detail_y* .51, lw=2.5, color='darkblue')

plt.annotate("", xytext = (0.0089, .598),  xy=(.01225,.3), 
	arrowprops={'color':'.4','arrowstyle':"->", 'linewidth':2, "connectionstyle": "arc,angleA=-60,angleB=90,armA=0,armB=20,rad=5"}, )
plt.annotate('static tasks', 
             xytext = (0.0034, .56),  # text location
             fontsize = 14,
             xy=(.01,.3),        # arrows points to 
             ha = 'left',    # horizontal alignment 
             va = 'bottom',    # vertical alignment
            )

plt.annotate("", xytext = (0.0157, .598),  xy=(.0195,.4), 
	arrowprops={'color':'.4', 'arrowstyle':"->", 'linewidth':2, "connectionstyle": "arc,angleA=-10,angleB=96,armA=0,armB=20,rad=5"}, )
plt.annotate('commit', 
             xytext = (0.012, .56),  # text location
             fontsize = 14,
             xy=(.01,.3),        # arrows points to 
             ha = 'left',    # horizontal alignment 
             va = 'bottom',    # vertical alignment
            )


plt.annotate("", xytext = (0.027, .598),  xy=(.0306,.488), 
	arrowprops={'color':'.4', 'arrowstyle':"->", 'linewidth':2, "connectionstyle": "arc,angleA=10,angleB=140,armA=0,armB=20,rad=5"}, )
plt.annotate('coalesced task', 
             xytext = (0.02, .56),  # text location
             fontsize = 14,
             xy=(.01,.3),        # arrows points to 
             ha = 'left',    # horizontal alignment 
             va = 'bottom',    # vertical alignment
            )

plt.annotate("", xytext = (0.0355, .598),  xy=(.0375,.55), 
	arrowprops={'color':".4" ,'arrowstyle':"->", 'linewidth':2}, )
plt.annotate('lost tasks', 
             xytext = (0.0308, .56),  # text location
             fontsize = 14,
             xy=(.01,.3),        # arrows points to 
             ha = 'left',    # horizontal alignment 
             va = 'bottom',    # vertical alignment
            )

#Ellipse
shape = patches.Ellipse((.03785,.26), width=.0015,height=.7 ,
	fill=False, color='r', lw=1.5)

plt.xlabel('time (second)')
plt.yticks(np.array([0, 0.5]), ('C', 'T') ) 
from matplotlib import rc
#rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
#rc('text', usetex=True)
plt.gca().add_patch(shape)
plt.tight_layout()
plt.savefig("../paper/TOSN/figures/dif_cap.eps",format="eps")
plt.savefig("../paper/TOSN/figures/dif_cap.pdf",format="pdf")
plt.show()



