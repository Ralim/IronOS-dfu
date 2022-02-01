##
##  This file is part of CM-Makefile.
##
##  Copyright (C) 2015 Adam Heinrich <adam@adamh.cz>
##
##  CM-Makefile is free software: you can redistribute it and/or modify
##  it under the terms of the GNU Lesser General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  CM-Makefile is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU Lesser General Public License for more details.
##
##  You should have received a copy of the GNU Lesser General Public License
##  along with CM-Makefile.  If not, see <http://www.gnu.org/licenses/>.
##

# Project name and build directory
BIN ?= template
BUILD_DIR ?= ./build

# Verbosity: Set V=1 to display command echo
V ?= 0

ifeq ($(V), 0)
	CMD_ECHO = @
else
	CMD_ECHO =
endif

# Include directories
INC += -I.

# ASM and C (e.g. startup) source files and linker script outside src directory
SRC_ASM ?=
SRC_C ?=
SRC_LD ?=

# Defines (e.g. required by included libraries)
DEF ?=

# Debugger configuration
CDTDEBUG ?= cdtdebug

# Compiler and linker flags
ARCHFLAGS ?= -mcpu=cortex-m0
FPFLAGS ?= -mfloat-abi=soft
ARCHFLAGS += -mthumb -mabi=aapcs $(FPFLAGS)
DBGFLAGS ?= -ggdb
OPTFLAGS ?= -O3
WARNFLAGS ?= -Wall -Wextra -Wundef -Wshadow -Wimplicit-function-declaration \
             -Wredundant-decls -Wstrict-prototypes -Wmissing-prototypes \
             -Wconversion -Wdouble-promotion -Wfloat-conversion -pedantic
PREPFLAGS ?= -MD -MP

# Use newlib-nano, include syscalls stubs (nosys)
SPECSFLAGS ?= --specs=nano.specs --specs=nosys.specs

# This should be only enabled during development:
#WARNFLAGS += -Werror

ASFLAGS = $(ARCHFLAGS)

# CC: Place functions and data into separate sections to allow dead code removal
# by the linker (-f*-sections)
# TODO: Add -fno-builtin?
CFLAGS = $(ARCHFLAGS) $(OPTFLAGS) $(DBGFLAGS) $(WARNFLAGS) $(PREPFLAGS) \
         -std=gnu99 \
         -ffunction-sections -fdata-sections -fno-strict-aliasing \
         $(SPECSFLAGS)

# CXX: Similar configuration as CC. Remove C-only warning flags, omit
# exceptions and run-time type information
CXXWARNFLAGS = $(filter-out -Wimplicit-function-declaration \
                            -Wstrict-prototypes \
                            -Wmissing-prototypes,$(WARNFLAGS))

CXXFLAGS = $(ARCHFLAGS) $(OPTFLAGS) $(DBGFLAGS) $(CXXWARNFLAGS) $(PREPFLAGS) \
         -std=gnu++11 \
         -ffunction-sections -fdata-sections -fno-strict-aliasing \
         -fno-rtti -fno-exceptions \
         $(SPECSFLAGS)

# LD: Remove unused sections, link with newlib-nano implementation, generate map
LDFLAGS = $(ARCHFLAGS) $(OPTFLAGS) $(DBGFLAGS) \
          -Wl,-Map=$(BUILD_DIR)/$(BIN).map -Wl,--gc-sections \
          $(SPECSFLAGS)

# Generate object list from source files and add their dirs to search path
SRC_ASM += $(wildcard *.s)
SRC_ASM += $(wildcard *.S)
FILENAMES_ASM = $(notdir $(SRC_ASM))
OBJFILENAMES_ASM = $(FILENAMES_ASM:.s=.o)
OBJS_ASM = $(addprefix $(BUILD_DIR)/, $(OBJFILENAMES_ASM:.S=.o))
vpath %.s $(dir $(SRC_ASM))
vpath %.S $(dir $(SRC_ASM))

SRC_C += $(wildcard *.c)
FILENAMES_C = $(notdir $(SRC_C))
OBJS_C = $(addprefix $(BUILD_DIR)/, $(FILENAMES_C:.c=.o))
vpath %.c $(dir $(SRC_C))

SRC_CXX += $(wildcard *.cpp)
FILENAMES_CXX = $(notdir $(SRC_CXX))
OBJS_CXX = $(addprefix $(BUILD_DIR)/, $(FILENAMES_CXX:.cpp=.o))
vpath %.cpp $(dir $(SRC_CXX))

# Tools selection
CROSS_COMPILE ?= arm-none-eabi-

CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
AS := $(CROSS_COMPILE)gcc
NM := $(CROSS_COMPILE)nm
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump
SIZE := $(CROSS_COMPILE)size
GDB := $(CROSS_COMPILE)gdb
DFUSEPACK := python3 tools/dfuse-pack.py

# Use g++ as a linker if there are any C++ object files:
ifeq ($(OBJS_CXX),)
LD := $(CROSS_COMPILE)gcc
else
LD := $(CROSS_COMPILE)g++
endif

# Help message:
HELP_TEXT = Available targets:\n\
  all - Build with default configuration\n\
  deps - Build dependencies (if specified)\n\
  clean - Remove build outputs\n\
  distclean - Remove build outputs as well as outputs of dependencies

# Desired outputs:
OUTPUTS = $(BUILD_DIR)/$(BIN).elf \
	  $(BUILD_DIR)/$(BIN).hex \
          $(BUILD_DIR)/$(BIN).bin \
          $(BUILD_DIR)/$(BIN).sym \
          $(BUILD_DIR)/$(BIN).disasm \
          $(BUILD_DIR)/$(BIN).dfu 
          

# Dependencies:
DEPS ?=
DEPS_CLEAN ?=

.PHONY: default
default: deps all
