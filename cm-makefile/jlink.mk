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

# Segger J-Link configuration
JLINK_DEVICE ?= Cortex-M0
JLINK_SPEED ?= 1000

JLINK_FLAGS = -device $(JLINK_DEVICE) -if swd -speed $(JLINK_SPEED)
JLINK_EXE = JLinkExe -nogui 1
JLINK_GDB = JLinkGDBServer -nogui 1

JLINK_GDBINIT ?= \
	target remote localhost:2331\n\
	set mem inaccessible-by-default off

HELP_TEXT += \n\
  flash - Flash using J-Link\n\
  reset - Reset the target MCU using J-Link\n\
  erase - Erase flash using J-Link\n\
  gdb - Start J-Link GDB server\n\
  cdtdebug - Start debugger (cdtdebug) and connect to the GDB server\n\
  debug - Start debugger (gdb -tui) and connect to the GDB server\n\
  rtt - Start J-Link RTT server

$(BUILD_DIR)/$(BIN).jlink.gdb:
	@echo "  ECHO    $(notdir $@)"
	$(CMD_ECHO) echo "$(JLINK_GDBINIT)" > $@

$(BUILD_DIR)/$(BIN).jlink.cdt:
	@echo "  ECHO    $(notdir $@)"
	$(CMD_ECHO) echo "org.eclipse.cdt.dsf.gdb/defaultGdbCommand=$(GDB)" > $@

.PHONY: flash
flash: $(BUILD_DIR)/$(BIN).hex
	$(CMD_ECHO) echo -e "loadfile $^\n\
	r\n\
	h\n\
	q\n" | $(JLINK_EXE) $(JLINK_FLAGS)
	$(CMD_ECHO) echo ""

.PHONY: reset
reset:
	$(CMD_ECHO) echo -e "r\n\
	g\n\
	q\n" | $(JLINK_EXE) $(JLINK_FLAGS)
	$(CMD_ECHO) echo ""

.PHONY: erase
erase:
	$(CMD_ECHO) echo -e "erase\n\
	r\n\
	q\n" | $(JLINK_EXE) $(JLINK_FLAGS)
	$(CMD_ECHO) echo ""

.PHONY: gdb
gdb: $(BUILD_DIR)/$(BIN).hex
	@echo "Starting GDB server"
	$(CMD_ECHO) $(JLINK_GDB) $(JLINK_FLAGS)

.PHONY: cdtdebug
cdtdebug: $(BUILD_DIR)/$(BIN).elf | $(BUILD_DIR)/$(BIN).jlink.cdt
	@echo "Starting cdtdebug with $(GDB)..."
	$(CMD_ECHO) $(CDTDEBUG) -pluginCustomization $| -r localhost:2331 \
				-e $(realpath $^) &

.PHONY: debug
debug: $(BUILD_DIR)/$(BIN).elf | $(BUILD_DIR)/$(BIN).jlink.gdb
	$(CMD_ECHO) $(GDB) -tui -x $| $^

.PHONY: rtt
rtt:
	$(JLINK_EXE) $(JLINK_FLAGS) -autoconnect 1
	$(CMD_ECHO) echo ""
