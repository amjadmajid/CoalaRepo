#### Intro
This is a basic example used to measure the energy consumption of writing to FRAM and compares it to SRAM writing operation.

I used the EDB debugger (http://brandonlucia.com/pubs/edb.pdf) to measure the enery consumption. The instructions to use the EDB can be found here (https://github.com/CMUAbstract/edb-rat). 

#### The methodology of conducting the experiment: 

0- Initial steps:
0.1- Connect the WISP to EDB
0.2- Run the EDB console
0.3- Attach the device 

1- watchpoints are enabled

2-The capacitor is charged to 2.45V* 
2.1 It is recommended charge the capacitor and to stream the watchpoints value on the same line as follows
charge 2.45; stream watchpoints**
 
3- I was manully updating the index of my for loops after the cap is discharged. 

#### How to interpret the results
I started with 100 FRAM writes and zero SRAM write, and the first dot in the graph is the output of this scenario. In the second Loop I reduced the number of writing to FRAM to 50 and increased the SRAM writes to 50, The second dot shows the energy consumption of this scenario. For the third iteration there were 35 FRAM write and 65 SRAM writes and so on changing. 

--------------------------------------------------------

*  2.45V is just enough to activate WISP and have some time to run the second instruction.

** Executing the commands in this way ensure that there is enough energy in the cap. or wisp to run the second commands 

