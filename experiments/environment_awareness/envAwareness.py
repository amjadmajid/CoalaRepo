import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


def main():
    # files = ['10cm-50cm.csv','10cm-80cm.csv','10cm-50cm_2.csv', '10cm-50cm_3.csv'] 
    files = ['10cm-50cm_3.csv'] 
    for file in files:
        f =  open(file)

        x=[]
        y=[]
        s= True
        for line in f:
            s = not s

            if s:
                point=line[:-1].split(',')

                x.append(float(point[0]))
                y.append(float(point[1]))
                # print(x,y)


        f = plt.figure(figsize=(8,4))
        figureSetting()

        frame = plt.stem( x, y,  markerfmt=" " )

        plt.ylim(0,1.1)
        plt.xlabel('Seconds')
        plt.tight_layout()


    ax = plt.gca()
    ax.yaxis.set_visible(False)
    f.savefig("../figures/envAwareness.eps",format="eps", dpi=1200)
    plt.show()







def figureSetting():
    """
    Set a figure params
    """
    #set figure linewidth
    plt.rcParams['axes.linewidth']=2

    #set font size for labels on axes
    plt.rcParams['axes.labelsize'] = 16

    #set size of numbers on x-axis
    plt.rcParams['xtick.labelsize'] = 16

    #set size of ticks on x-axis
    plt.rcParams['xtick.major.size'] = 7


    #set size of markers, e.g., circles representing points
    #set numpoints for legend
    plt.rcParams['legend.numpoints'] = 1

    
if __name__ == '__main__':
        main()