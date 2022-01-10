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

OPENCM3_DIR ?= ./libopencm3
OPENCM3_TARGET ?= stm32/f0
OPENCM3_PREFIX ?= $(CROSS_COMPILE)

OPENCM3_LIB = libopencm3_$(subst /,,$(OPENCM3_TARGET)).a

OPENCM3_FLAGS  = V=$(V)
OPENCM3_FLAGS += PREFIX=$(OPENCM3_PREFIX)
OPENCM3_FLAGS += FP_FLAGS="$(FPFLAGS)"
OPENCM3_FLAGS += DEBUG_FLAGS="$(DBGFLAGS)"

HELP_TEXT += \n\
  libopencm3 - Build the libopencm3 library\n\
  libopencm3_clean - Remove files built in the libopencm3 folder

# Setup linker flags required by libopencm3:
LDFLAGS += -l$(subst lib,,$(subst .a,,$(OPENCM3_LIB)))
LDFLAGS += -nostartfiles

# Setup dependencies (to build libopencm3 automatically):
DEPS += libopencm3
DEPS_CLEAN += libopencm3_clean

$(OPENCM3_DIR)/Makefile:
	$(error Error: libopencm3 is not available. Run \
		`git submodule update --init` to initialize submodules)

$(OPENCM3_DIR)/lib/$(OPENCM3_LIB): $(OPENCM3_DIR)/Makefile
	$(CMD_ECHO) $(MAKE) -C $(OPENCM3_DIR) $(OPENCM3_FLAGS) \
	lib/$(OPENCM3_TARGET)

.PHONY: libopencm3
libopencm3: $(OPENCM3_DIR)/lib/$(OPENCM3_LIB)

.PHONY: libopencm3_clean
libopencm3_clean:
	$(CMD_ECHO) $(MAKE) -C $(OPENCM3_DIR) clean
