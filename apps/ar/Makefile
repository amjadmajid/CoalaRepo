# Source and build directories
PROJECT   := ar
SRC_DIR   := ./
BLD_DIR   := build
MAKER_DIR := ../../maker

# Edit env.mk as needed
include $(MAKER_DIR)/envCom.mk

# Uncomment and edit if the path to msp430-gcc compiler is different
# than "/Developer/ti/msp430-gcc"
# GCC_DIR := /path/to/msp430-gcc

# Uncomment if additional flags, includes or libraries are needed
# (other than the ones defined in maker/apps.mk)
INCLUDES := \
	-I $(LIB_DIR)/mspmath/include \
	-I $(LIB_DIR)/wispbase/include \

# CFLAGS :=

LFLAGS := \
	-L $(LIB_DIR)/mspmath/build \
	-L $(LIB_DIR)/wispbase/build \

LIBS := \
	-lmspmath \
	-lwispbase \

include $(MAKER_DIR)/apps.mk
