# ##########################
#	Environment variables
# ##########################

# Path to toolchain
GCC_DIR   := /Developer/ti/msp430-gcc

# Path to MSP430 libraries
LIB_DIR   := ../../libs

# Path to Coala
COALA_DIR := ../../coala

# Path to modified linker script
LSC_DIR   := $(COALA_DIR)/linkerScripts/8KB_pages

# #########################
# 	Common variabls
# #########################

# Compiler and tools
GCC_MSP := $(GCC_DIR)/bin/msp430-elf-
GCC     := $(GCC_MSP)gcc
AR      := $(GCC_MSP)ar
DEVICE  := msp430fr5969

# control showing the debugging info
VERBOSE ?= 1
ifeq ($(VERBOSE),1)
V :=
VECHO := @echo
else
V := @
VECHO := @true
endif


