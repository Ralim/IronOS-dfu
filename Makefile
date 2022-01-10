GIT_VERSION := $(shell git describe --abbrev=8 --dirty --always --tags)



# Project name
BIN = stm32-dfu


INC = -I src

SRC_C += $(wildcard src/*.c)
SRC_LD = src/stm32f103.ld
# Defines required by included libraries
DEF = -DSTM32F030x8 -DVERSION=\"$(GIT_VERSION)\"

# OpenOCD setup

JLINK_DEVICE = STM32f103C8

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

