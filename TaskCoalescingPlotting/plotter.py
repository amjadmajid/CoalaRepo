#
# @Author: Amjad Majid
# @Date  : March 14, 2019
#
import logicAnalyzerData as lad
import numpy as np
import matplotlib.pyplot as plt

class Plotter:

    def __init__(self, data):
        self.data = data
        self.interpolatedStates=[]
        self.interpolatedTimestamps=[] 
        self.__dataInterpolation()

    def __dataInterpolation(self):
        """This method interpolates the logic analyzer data to enable
        square-like graph plotting."""
        clustersTimestamps=[]
        timestamps=self.data[0]
        states=self.data[1]
        for idx in range(len(timestamps)-1):
            self.interpolatedTimestamps.append(timestamps[idx])
            self.interpolatedTimestamps.append(timestamps[idx+1])
            self.interpolatedStates.append(states[idx])
            self.interpolatedStates.append(states[idx])
        self.interpolatedTimestamps.append(timestamps[-1])
        self.interpolatedStates.append(states[-1])

    def getData(self):
        return np.array(self.interpolatedTimestamps), np.array(self.interpolatedStates)

    def plotOnTime(self, lineWidth=1, posY=0, scaleY=1):
        y = np.array(self.interpolatedStates) * scaleY+ posY
        # print(y[:20])
        # print(y[:10])
        # plt.figure()
        #print(np.sum(self.interpolatedStates, axis=1))
        plt.plot(self.interpolatedTimestamps,y, lw = lineWidth)


