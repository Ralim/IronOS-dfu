# CM-Makefile: How to install required tools

This document describes how to install tools required by CM-Makefile.

## Ubuntu

### GCC ARM Embedded Toolchain

The toolchain can be installed from repositories (unfortunately, the Ubuntu
repository does not contain GDB):

	sudo apt-get install gcc-arm-embedded

A preferred way is to install the latest toolchain (with GDB which is needed for
debugging or for flashing with the Black Magic Probe) downloaded directly from
[Arm website][1]. In this case, it might be necessary to obtain 32-bit `libc`
and `libncurses` (required by GDB):

	sudo apt-get install libc6:i386 libncurses5:i386

Then simply add the toolchain to `PATH`:

	echo "export PATH="/path/to/gcc-arm-none-eabi-8-2018-q4-major/bin:$PATH"" >> ~/.bashrc
	source ~/.bashrc

Alternatively (or to use a specific toolchain version), set the `CROSS_COMPILE`
variable when running `make`:

	make CROSS_COMPILE=/usr/local/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-

### OpenOCD

OpenOCD can be installed from Ubuntu repository:

	sudo apt-get install openocd

However, as the release cycle is rather slow, it might be more useful to compile
it from sources (especially when working with newer MCUs).

[1]: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
