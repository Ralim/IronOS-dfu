GIT_VERSION := $(shell git describe --abbrev=8 --dirty --always --tags)

build_type ?= runtime
model ?= TS101

ifeq ($(build_type), runtime)
	ifeq ($(model), TS101)
		VECTOR_TABLE_OFFSET := 0x8000
	else ifeq ($(model), MHP30)
		VECTOR_TABLE_OFFSET := 0x8000
	else ifeq ($(model), S60)
		VECTOR_TABLE_OFFSET := 0x4400
	else
		VECTOR_TABLE_OFFSET := 0x4000
	endif
else ifeq ($(build_type), bootloader)
	VECTOR_TABLE_OFFSET := 0x0000
endif

SRC_LD = src/stm32f103.ld

BIN = ${build_type}_${model}

INC = -I src

SRC_C += $(wildcard src/*.c)
# Defines required by included libraries
DEF = -DSTM32F030x8 -DVERSION=\"$(GIT_VERSION)\" -DVECTOR_TABLE_OFFSET=${VECTOR_TABLE_OFFSET} -DMODEL_${model}=1

# OpenOCD setup

JLINK_DEVICE = STM32F103C8

include cm-makefile/config.mk
include cm-makefile/rules.mk
include cm-makefile/jlink.mk

LDFLAGS += -Wl,--defsym=__BOOTLDR_SIZE__=$(VECTOR_TABLE_OFFSET)

WARNFLAGS += -Wno-undef -Wno-conversion  -Wall -pedantic -Werror

OPTFLAGS =  -Os -flto -finline-small-functions \
-findirect-inlining -fdiagnostics-color \
-ffunction-sections -fdata-sections -Wno-overlength-strings -ggdb -nostartfiles

ARCHFLAGS = -mcpu=cortex-m3 -mthumb -DSTM32F1 -std=c11
DBGFLAGS =

