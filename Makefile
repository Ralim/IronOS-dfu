GIT_VERSION := $(shell git describe --abbrev=8 --dirty --always --tags)

build_type ?= runtime

ifeq ($(build_type), runtime)
    VECTOR_TABLE_OFFSET := 0x4000
	SRC_LD = src/stm32f103_runtime.ld

BIN = runtime
else 
	VECTOR_TABLE_OFFSET := 0x0000
	SRC_LD = src/stm32f103.ld
BIN = bootloader

endif



INC = -I src

SRC_C += $(wildcard src/*.c)
# Defines required by included libraries
DEF = -DSTM32F030x8 -DVERSION=\"$(GIT_VERSION)\" -DVECTOR_TABLE_OFFSET=${VECTOR_TABLE_OFFSET}

# OpenOCD setup

JLINK_DEVICE = STM32F103C8

include cm-makefile/config.mk
include cm-makefile/rules.mk
include cm-makefile/jlink.mk

# Disable GCC warnings which are triggered by the STM32CubeF0 SDK
WARNFLAGS += -Wno-undef -Wno-conversion  -Wall -pedantic -Werror 

OPTFLAGS =  -Os -flto -finline-small-functions \
-findirect-inlining -fdiagnostics-color \
-ffunction-sections -fdata-sections -Wno-overlength-strings -ggdb -nostartfiles


ARCHFLAGS = -mcpu=cortex-m3 -mthumb -DSTM32F1 -std=c11 
DBGFLAGS = 

