# IronOS DFU Bootloader for Miniware

Based on the awesome work by https://github.com/davidgfnet/stm32-dfu-bootloader
and huge kudos for the framework https://github.com/adamheinrich/cm-makefile.
Based on ideas and a lot of work done by @perillamint https://github.com/eDesignOSS/ts100-bl-flasher.
This can be thought of as version _2_.

This is _going to be_ an alternative bootloader for the Miniware Irons.

**THIS IS NOT PRODUCTION READY. DO NOT FLASH THIS YET.**

## Planned features:

1. Backup your stock bootloader
2. Flash any other bootloader to your device (With some safety)
3. Supports the OLED screen
4. Designed for working with the Miniware Irons.. But should work on any STM32F1 part with minimal (or no) modifications

## How it works

This bootloader builds in two versions for each supported hardware.
A `bootloader` and a `runtime` version.

### Bootloader

The bootloader version is intended to be used as your device bootloader. It will only go into USB-DFU mode if there is no application flashed to the device _or_ if a button is held down when booting.

### Runtime

In runtime mode, it is expected to be running where your main firmware _would be_.
In this mode it can be used to read/write the actual bootloader on the device.
