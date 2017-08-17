#!/home/amjed/anaconda3/bin/python
import numpy as np 
import matplotlib.pyplot as plt 
from matplotlib import style

style.use('seaborn-muted')

numberOfReadLines = 1600;

#----Get the volatge values of the cap-----#
fram_voltages=[]
sram_voltages=[]
fram_cntr=0
sram_cntr=0
for line in open("../outputs/fram.txt"):
	# get the voltages and remove the `\n` char
	fram_voltages.append( (line.split(',')[-1])[:-1] ) 
	fram_cntr+=1

for line in open("../outputs/sram.txt"):
	# get the voltages and remove the `\n` char
	sram_voltages.append( (line.split(',')[-1])[:-1] )
	sram_cntr+=1

#print(fram_voltages)	 
#print(sram_voltages)
#exit()


#-----SRAM/FRAM write costs------#
v_list_len= min(fram_cntr, sram_cntr)
fram_diffs=[]
sram_diffs=[]
for i in range(0,v_list_len, 2):
	fram_diffs.append( ( ( (float(fram_voltages[i])/numberOfReadLines )**2 - (float(fram_voltages[i+1])/numberOfReadLines ) **2 ) * 10e-6 * 0.5 ) ) 
	sram_diffs.append( ( ( (float(sram_voltages[i])/numberOfReadLines )**2 - (float(sram_voltages[i+1])/numberOfReadLines ) **2 ) * 10e-6 * 0.5 ) ) 

# print(fram_diffs[:5])
# print(sram_diffs[:5])
# exit()

# print (sum(fram_diffs) /len(fram_diffs))
# print (sum(sram_diffs) /len(sram_diffs))


#------------Line fitting------------#
fit_order=3
xVals = range(len(fram_diffs))
fram_z =  np.polyfit( xVals, fram_diffs, fit_order)
sram_z =  np.polyfit( xVals, sram_diffs, fit_order)
fram_estYVals = np.polyval(fram_z, xVals)
sram_estYVals = np.polyval(sram_z, xVals)


#--------------Plotting--------------#

plt.rcParams['axes.labelsize'] =  12
plt.rcParams['lines.linewidth'] =  1.5
plt.figure(figsize=(6,3))
plt.plot(xVals, fram_estYVals, 'b-', label="FRAM")
plt.plot(fram_diffs, 'bo')

plt.plot(xVals, sram_estYVals, 'r-', linewidth=1, label="SRAM")
plt.plot(sram_diffs, 'r*')

plt.xlabel("Number of measurements of the capacitor")

plt.figure('box', figsize=(4,2))
plt.boxplot((fram_diffs , sram_diffs) )

plt.xticks([1,2], ['FRAM', 'SRAM'])
plt.yticks([0e-13,2e-13,4e-13,6e-13], [0.0,0.2,0.4,0.6])


plt.ylabel("pico joule")
plt.legend(loc='best')



plt.tight_layout()

plt.savefig("../../../figures/fram_write.eps", format="eps", dpi=1200)


plt.show()


