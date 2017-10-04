#!/bin/bash

# export LD_LIBRARY_PATH=~/MSPFlasher_1.3.11/

# clear
# #./MSP430Flasher -w "Firmware.txt" -v -g -z [VCC]
# read -p "Press any key to continue..."


echo "Select (1) erasing (2) reading (3) flashing (must be after read) :"
read option
echo "Insert the application name: "
read appName
echo "Insert the distance: "
read dis

cm=cm
cd ~/MSPFlasher_1.3.8/

if [ $option -eq 1 ]; then
	./MSP430Flasher -g -z [VCC] -e ERASE_ALL
elif [ $option -eq 2 ]; then
	./MSP430Flasher -r [FirmwareOutput.hex,0x0000-0xffff]
cd -
	cp ~/MSPFlasher_1.3.8/FirmwareOutput.hex  $(pwd)/../data/$appName/virtualTaskSize/$appName-$dis$cm.hex

elif [ $option -eq 3 ]; then
	./MSP430Flasher -w "FirmwareOutput.hex" -v -g -z [VCC]
else
	echo "invaled option"

cd -
fi 