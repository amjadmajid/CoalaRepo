#!/bin/bash

# export LD_LIBRARY_PATH=~/MSPFlasher_1.3.11/

# clear
# #./MSP430Flasher -w "Firmware.txt" -v -g -z [VCC]
# read -p "Press any key to continue..."
echo "Select (1) erasing (2) reading (3) flashing (must be after read) :"
read option

if [ $option -eq 1 ]; then
	~/MSPFlasher_1.3.11/MSP430Flasher -g -z [VCC] -e ERASE_ALL
elif [ $option -eq 2 ]; then
	~/MSPFlasher_1.3.11/MSP430Flasher -r [FirmwareOutput.hex,0x0000-0xffff]
elif [ $option -eq 3 ]; then
	~/MSPFlasher_1.3.11/MSP430Flasher -w "FirmwareOutput.hex" -v -g -z [VCC]
else
	echo "invaled option"
fi 