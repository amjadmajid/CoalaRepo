include ../maker/envCom.mk
# Source and build directories
PROJECT := coala
SRC_DIR := src
BLD_DIR := build
LIB_DIR := ../libs

# Uncomment if additional flags or includes are needed
# (other than the ones specified in maker/libs.mk)
INCLUDES := \
	-I $(LIB_DIR)/mspDebugger/include \
	-I $(LIB_DIR)/mspProfiler/include \
	-I $(LIB_DIR)/mspReseter/include \
	-I $(LIB_DIR)/mspprintf/include \
	-I $(LIB_DIR)/mspbase/include \

CFLAGS := -DENABLE_PARTIAL_COMMIT=1

include ../maker/libs.mk
