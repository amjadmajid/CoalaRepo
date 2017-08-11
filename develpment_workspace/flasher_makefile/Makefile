EXEC = cem2

# GCC_DIR = /opt/ti/gcc/
GCC_DIR = ~/ti/gcc/
TI_FILES = $(GCC_DIR)include/
GCC_MSP = $(GCC_DIR)/bin/msp430-elf-
GCC = $(GCC_MSP)gcc
GDB = $(GCC_MSP)gdb
OBJCOPY = $(GCC_MSP)objcopy

FLASHER = ~/MSPFlasher_1.3.8/MSP430Flasher


# CFLAGS = -I $(TI_FILES) -I$(LIBs_INC)  -mmcu=$(DEVICE) -std=c99 -o1 -g -static -Wall
# LFLAGS = -L $(TI_FILES)  -L$(IPOSLIB) -L$(LIBs) -T $(LINKER_SCRIPTS_ROOT)$(DEVICE).ld

all: 
	$(OBJCOPY) -O ihex $(EXEC).out $(EXEC).hex

clean: 
	-@rm *.o*
	-@rm *.hex

flash: all
	$(FLASHER)  -w $(EXEC).hex -v -g -z [VCC]

debug:
	$(GDB)

prog: $(EXEC).out
	LD_LIBRARY_PATH=/opt/ti/gcc/bin mspdebug -v  3300 tilib "prog $(EXEC).out"