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

# Black Magic Probe target configuration
BLACKMAGIC_PATTERN ?= usb-Black_Sphere_Technologies_Black_Magic_Probe_*-if00
BLACKMAGIC_PORT ?= $(wildcard /dev/serial/by-id/$(BLACKMAGIC_PATTERN))
BLACKMAGIC_AUTO_TPWR ?= 0

BLACKMAGIC_GDBINIT ?= \
	set mi-async on\n\
	target extended-remote $(BLACKMAGIC_PORT)\n\
	set confirm off\n\
	set mem inaccessible-by-default off\n\
	monitor version\n\
	$(if $(filter 1,$(BLACKMAGIC_AUTO_TPWR)),,\#)monitor tpwr enable\n\
	monitor swdp_scan\n\
	attach 1\n\
	file $(realpath $(BUILD_DIR)/$(BIN).elf)\n\
	load

HELP_TEXT += \n\
  flash - Flash using Black Magic Probe\n\
  reset - Reset the target MCU using Black Magic Probe\n\
  erase - Erase flash (STM32 only) using Black Magic Probe\n\
  power_<on/off> - Supply power to the target from Black Magic Probe\n\
  cdtdebug - Start debugger (cdtdebug) and connect to the GDB server\n\
  debug - Start debugger (gdb -tui) and connect to the GDB server

OUTPUTS += $(BUILD_DIR)/$(BIN).bmp.gdb $(BUILD_DIR)/$(BIN).bmp.cdt

$(BUILD_DIR)/$(BIN).bmp.gdb:
	@echo "  ECHO    $(notdir $@)"
	$(CMD_ECHO) echo "$(BLACKMAGIC_GDBINIT)" > $@

$(BUILD_DIR)/$(BIN).bmp.cdt: $(BUILD_DIR)/$(BIN).bmp.gdb
	@echo "  ECHO    $(notdir $@)"
	$(CMD_ECHO) echo "org.eclipse.cdt.dsf.gdb/defaultGdbCommand=$(GDB)\n"\
	"org.eclipse.cdt.dsf.gdb/defaultGdbInit=$(realpath $^)\n" > $@

.PHONY: flash
flash: $(BUILD_DIR)/$(BIN).hex
	$(CMD_ECHO) $(GDB) -nx --batch \
	-ex 'target extended-remote $(BLACKMAGIC_PORT)' \
	-ex 'set confirm off' \
	-ex 'monitor version' \
	$(if $(filter 1,$(BLACKMAGIC_AUTO_TPWR)),-ex 'monitor tpwr enable') \
	-ex 'monitor swdp_scan' \
	-ex 'attach 1' \
	-ex 'load' \
	-ex 'compare-sections' \
	-ex 'kill' \
	$^

.PHONY: reset
reset:
	$(CMD_ECHO) $(GDB) -nx --batch \
	-ex 'target extended-remote $(BLACKMAGIC_PORT)' \
	-ex 'set confirm off' \
	-ex 'monitor version' \
	$(if $(filter 1,$(BLACKMAGIC_AUTO_TPWR)),-ex 'monitor tpwr enable') \
	-ex 'monitor swdp_scan' \
	-ex 'attach 1' \
	-ex 'kill'

.PHONY: erase
erase:
	$(CMD_ECHO) $(GDB) -nx --batch \
	-ex 'target extended-remote $(BLACKMAGIC_PORT)' \
	-ex 'set confirm off' \
	-ex 'monitor version' \
	$(if $(filter 1,$(BLACKMAGIC_AUTO_TPWR)),-ex 'monitor tpwr enable') \
	-ex 'monitor swdp_scan' \
	-ex 'attach 1' \
	-ex 'monitor erase' \
	-ex 'kill'

.PHONY: power_on
power_on:
	$(CMD_ECHO) $(GDB) -nx --batch \
	-ex 'target extended-remote $(BLACKMAGIC_PORT)' \
	-ex 'monitor tpwr enable'

.PHONY: power_off
power_off:
	$(CMD_ECHO) $(GDB) -nx --batch \
	-ex 'target extended-remote $(BLACKMAGIC_PORT)' \
	-ex 'monitor tpwr disable'

.PHONY: cdtdebug
cdtdebug: $(BUILD_DIR)/$(BIN).elf | $(BUILD_DIR)/$(BIN).bmp.cdt
	@echo "Starting cdtdebug with $(GDB)..."
	$(CMD_ECHO) $(CDTDEBUG) -pluginCustomization $| -e $(realpath $^) &

.PHONY: debug
debug: $(BUILD_DIR)/$(BIN).elf | $(BUILD_DIR)/$(BIN).bmp.gdb
	$(CMD_ECHO) $(GDB) -tui -x $| $^
