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

# OpenOCD target configuration
OPENOCD ?= openocd -f board/st_nucleo_f0.cfg

OPENOCD_GDBINIT ?= \
	target remote localhost:3333\n\
	set mem inaccessible-by-default off

HELP_TEXT += \n\
  flash - Flash using OpenOCD\n\
  reset - Reset the target MCU using OpenOCD\n\
  gdb - Start OpenOCD as GDB server\n\
  cdtdebug - Start debugger (cdtdebug) and connect to the GDB server\n\
  debug - Start debugger (gdb -tui) and connect to the GDB server

OUTPUTS += $(BUILD_DIR)/$(BIN).openocd.gdb $(BUILD_DIR)/$(BIN).openocd.cdt

$(BUILD_DIR)/$(BIN).openocd.gdb:
	@echo "  ECHO    $(notdir $@)"
	$(CMD_ECHO) echo "$(OPENOCD_GDBINIT)" > $@

$(BUILD_DIR)/$(BIN).openocd.cdt:
	@echo "  ECHO    $(notdir $@)"
	$(CMD_ECHO) echo "org.eclipse.cdt.dsf.gdb/defaultGdbCommand=$(GDB)" > $@

.PHONY: flash
flash: $(BUILD_DIR)/$(BIN).hex
	$(CMD_ECHO) $(OPENOCD) -c \
	"init; \
	reset halt; \
	sleep 100; \
	flash write_image erase $^; \
	reset run; \
	shutdown"

.PHONY: reset
reset:
	$(CMD_ECHO) $(OPENOCD) -c \
	"init; \
	reset halt; \
	sleep 100; \
	reset run; \
	shutdown"

.PHONY: gdb
gdb: $(BUILD_DIR)/$(BIN).hex
	$(CMD_ECHO) $(OPENOCD) -c \
	"init; \
	reset halt; \
	sleep 100; \
	flash write_image erase $^; \
	reset halt"

.PHONY: cdtdebug
cdtdebug: $(BUILD_DIR)/$(BIN).elf | $(BUILD_DIR)/$(BIN).openocd.cdt
	@echo "Starting cdtdebug with $(GDB)..."
	$(CMD_ECHO) $(CDTDEBUG) -pluginCustomization $| -r localhost:3333 \
				-e $(realpath $^) &

.PHONY: debug
debug: $(BUILD_DIR)/$(BIN).elf | $(BUILD_DIR)/$(BIN).openocd.gdb
	$(CMD_ECHO) $(GDB) -tui -x $| $^
