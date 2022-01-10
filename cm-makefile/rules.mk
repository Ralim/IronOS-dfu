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

.PHONY: all
all: deps $(BUILD_DIR) $(OUTPUTS)
	@echo ""
	$(CMD_ECHO) $(SIZE) $(BUILD_DIR)/$(BIN).elf

.PHONY: deps
deps:
	@if [ ! -z "$(DEPS)" ]; then  \
		$(MAKE) -s $(DEPS); \
	fi

$(BUILD_DIR):
	$(CMD_ECHO) mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.s | deps
	@echo "  AS      $(notdir $@)"
	$(CMD_ECHO) $(AS) $(ASFLAGS) $(DEF) $(INC) -c -o $@ $<

$(BUILD_DIR)/%.o: %.S | deps
	@echo "  AS      $(notdir $@)"
	$(CMD_ECHO) $(AS) $(ASFLAGS) $(DEF) $(INC) -c -o $@ $<

$(BUILD_DIR)/%.o: %.c | deps
	@echo "  CC      $(notdir $@)"
	$(CMD_ECHO) $(CC) $(CFLAGS) $(DEF) $(INC) -c -o $@ $<

$(BUILD_DIR)/%.o: %.cpp | deps
	@echo "  CXX     $(notdir $@)"
	$(CMD_ECHO) $(CXX) $(CXXFLAGS) $(DEF) $(INC) -c -o $@ $<

$(BUILD_DIR)/$(BIN).elf: $(OBJS_ASM) $(OBJS_C) $(OBJS_CXX) | $(SRC_LD)
	@echo "  LD      $(notdir $@)"
	$(CMD_ECHO) $(LD) $^ $(LDFLAGS) $(INC) -T$(SRC_LD) -o $@

$(BUILD_DIR)/$(BIN).sym: $(BUILD_DIR)/$(BIN).elf
	@echo "  NM      $(notdir $@)"
	$(CMD_ECHO) $(NM) -n $< > $@

$(BUILD_DIR)/$(BIN).disasm: $(BUILD_DIR)/$(BIN).elf
	@echo "  OBJDUMP $(notdir $@)"
	$(CMD_ECHO) $(OBJDUMP) -S $< > $@

$(BUILD_DIR)/$(BIN).hex: $(BUILD_DIR)/$(BIN).elf
	@echo "  IHEX    $(notdir $@)"
	$(CMD_ECHO) $(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/$(BIN).bin: $(BUILD_DIR)/$(BIN).elf
	@echo "  BIN     $(notdir $@)"
	$(CMD_ECHO) $(OBJCOPY) -O binary $< $@

-include $(addprefix $(BUILD_DIR)/, $(FILENAMES_C:.c=.d))
-include $(addprefix $(BUILD_DIR)/, $(FILENAMES_CXX:.cpp=.d))

.PHONY: clean
clean:
	rm -f $(OUTPUTS)
	rm -f $(BUILD_DIR)/$(BIN).map $(BUILD_DIR)/*.o

.PHONY: distclean
distclean: $(DEPS_CLEAN) clean

.PHONY: help
help:
	@echo -e '$(HELP_TEXT)'
