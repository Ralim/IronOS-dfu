# CM-Makefile: Makefile template for Cortex-M projects

Makefile template for ARM Cortex-M projects using the [GNU Arm Embedded][1]
toolchain. The template is meant to be included in project-specific Makefile.

See repository [cm-makefile-examples][5] for usage examples.

## Dependencies

To successfully compile and debug the project, you need:

- Linux desktop (OS X should work as well)
- [GCC ARM Embedded][1] toolchain
- MCU vendor's HAL (start-up code, register definitions, ...)

Optional software:

- [OpenOCD][2] debugger
- [Eclipse CDT Stand-alone Debugger][3]
- [Segger J-Link Software and Documentation Pack][10]

Installation instructions are described in a [separate file](./INSTALL.md).

## Usage

First, create `Makefile`. The Makefile should link to source files outside the
project's source directory (such as libraries provided by the manufacturer) and
include `*.mk` files from the CM-Makefile project. For example:

	BIN = hello                     # Name of the output binary
	SDK_DIR = ../mcu-vendor         # Target-specific library

	INC = -I$(SDK_DIR)/inc
	SRC_ASM = $(SDK_DIR)/src/startup.S
	SCR_C = $(SDK_DIR)/src/system.c
	SRC_LD = $(SDK_DIR)/target.ld

	OPENOCD = openocd -f board/my_board.cfg

	include cm-makefile/config.mk
	include cm-makefile/openocd.mk  # For flash, debug and gdb targets
	include cm-makefile/rules.mk

See the [cm-makefile-examples][5] repository for real-world examples.

Then simply run `make` to compile the project and `make flash` to upload it to
the target.

The process will create `.elf`, `.hex` and `.bin` binaries in the `build`
directory together with other files useful for debugging:

- `.disasm`: Disassembly output
- `.sym`: Name list (useful to check memory layout)
- `.map`: Linker map (useful to check linked object files, discarded sections,
  etc.)

To use a specific toolchain (other than the default `arm-none-eabi`), simply
set the `CROSS_COMPILE` variable:

	make CROSS_COMPILE=/usr/local/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-

### Flashing and debugging

To flash the program into the target device, run `make flash`.
To reset the CPU, run `make reset`.

Three different flasher implementations are provided:

- OpenOCD (`openocd.mk`)
- Black Magic Probe
- J-Link (`jlink.mk`)

To debug the project, run `make gdb` (which starts OpenOCD's GDB server)
followed by `make debug` in a different terminal (which starts GDB in TUI
mode).

It is also possible to use the Eclipse standalone debugger using
`make cdtdebut`. The debugger has to be available as `cdtdebug`. If it isn't,
simply change the `CDTDEBUG` variable.

## Configuration

The configuration is quite straightforward. Most of variables used in
`config.mk` and `rules.mk` can be modified by the project-specific Makefile.
For example:

- `INC`: Include path for header files (`-I*.h`) or linker scripts (`-L*.ld`)
- `SRC_ASM`: Assembly source files (`*.S`, `*.s`) outside the current directory
  (e.g. target-specific startup code)
- `SRC_C`: C source files (`*.c`) outside the current directory
- `SRC_LD`: Linker script (`*.ld`)
- `DEF`: Custom macros (`-DMACRO` equals to `#define MACRO`)
- `ARCHFLAGS`: CPU architecture (see below)
- `FPFLAGS`: FLoating point configuration (see below)
- `WARNFLAGS`: GCC [warning options][6]. You can define your own set of options
  or disable the unwanted ones by appending `-Wno-*` flags
- `DBGFLAGS`: GCC [debugging options][7] (`-ggdb` by defualt)
- `OPTFLAGS`: GCC [optimization options][8] (`-O3` by defualt)
- `PREPFLAGS`: GCC [preprocessor options][9] (`-MD -MP` by defualt)

### Configuraiton specific to different debug probes

For `openocd.mk`:

- `OPENOCD`: OpenOCD command (with script selection) to set up debug session.
  You can use one of the [built-in scripts][4] or create your own.

For `blackmagic.mk`:

- `BLACKMAGIC_PORT`: Serial port to be used for the Black Magic Probe
- `BLACKMAGIC_AUTO_TPWR`: Automatically enable target power (`0` by default)

For `jlink.mk`:

- `JLINK_DEVICE`: Target device (`Cortex-M0` by default)
- `JLINK_SPEED`: Programming speed in Hz (`1000` by default)

### Configuration for a different target

CM-Makefile can be easily used with a different CPU core than the default
Cortex-M0. It's only necessary to configure the `ARCHFLAGS` and `FPFLAGS`
variables (`FPFLAGS` defaults to `-mfloat-abi=soft`):

| CPU                 | `ARCHFLAGS`           | `FPFLAGS`                              |
|---------------------|-----------------------|----------------------------------------|
| Cortex-M0 (default) | `-mcpu=cortex-m0`     | `-mfloat-abi=soft`                     |
| Cortex-M0+          | `-mcpu=cortex-m0plus` | `-mfloat-abi=soft`                     |
| Cortex-3            | `-mcpu=cortex-m3`     | `-mfloat-abi=soft`                     |
| Cortex-M4 (Soft FP) | `-mcpu=cortex-m4`     | `-mfloat-abi=softfp -mfpu=fpv4-sp-d16` |
| Cortex-M4 (Hard FP) | `-mcpu=cortex-m4`     | `-mfloat-abi=hard -mfpu=fpv4-sp-d16`   |

## License

CM-Makefile is free software: you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

See `COPYING` and `COPYING.LESSER` for details.

[1]: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
[2]: http://openocd.org/
[3]: https://wiki.eclipse.org/CDT/StandaloneDebugger
[4]: https://github.com/ntfreak/openocd/tree/master/tcl/board
[5]: https://github.com/adamheinrich/cm-makefile-examples
[6]: https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
[7]: https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html
[8]: https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
[9]: https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html
[10]: https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack
