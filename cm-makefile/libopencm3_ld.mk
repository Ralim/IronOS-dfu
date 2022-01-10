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

# Setup linker script generator and add the script to outputs (so it can be
# automatically removed by `make clean`):

OPENCM3_DIR ?= ./libopencm3
OPENCM3_LIB ?= libopencm3_stm32f0.a
OPENCM3_DEVICE ?= stm32f030r8

ifneq ($(wildcard $(OPENCM3_DIR)/mk),)

DEVICE = $(OPENCM3_DEVICE)

-include $(OPENCM3_DIR)/mk/genlink-config.mk

LDSCRIPT = $(SRC_LD)
Q ?= $(CMD_ECHO)
OUTPUTS += $(LDSCRIPT)

-include $(OPENCM3_DIR)/mk/genlink-rules.mk

else

$(error Error: libopencm3 is not available. Run `git submodule update --init` \
	to initialize submodules)

endif