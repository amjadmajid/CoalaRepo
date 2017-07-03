#!/usr/python3
# this script is used to generate an array for the experiment of 
# FRAm energy cost.

import random 
import numpy as np 

arraySize = 400
listLen =  1600

indices = np.random.choice(arraySize,arraySize)

nv_num_f = open("nv_num.txt", "w")
vo_num_f = open("vo_num.txt", "w")

read_nv_num_f = open("read_nv_num.txt", "w")
read_vo_num_f = open("read_vo_num.txt", "w")

for i in range(arraySize):
	nv_num_f.write("nv_num[{}] = {};\n".format(i, random.randrange(65000) ))
	vo_num_f.write("vo_num[{}] = {};\n".format(i, random.randrange(65000) ))

for i in range(listLen):
	read_nv_num_f.write ("vo_num2[{}] = nv_num[{}];\n".format(indices[i%arraySize], indices[ indices[i%arraySize] ]) )
	read_vo_num_f.write ("vo_num2[{}] = vo_num[{}];\n".format(indices[i%arraySize], indices[ indices[i%arraySize] ]) )