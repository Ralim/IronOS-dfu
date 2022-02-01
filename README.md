# IronOS DFU Bootloader for Miniware

This is a replacement bootloader for Miniware irons. This replaces the build in bootloader with one that can be used with `dfu-util`.

## How it works

This bootloader builds in two versions for each supported hardware.
A `bootloader` and a `runtime` version.

### Bootloader

The bootloader version is intended to be used as your device bootloader. It will only go into USB-DFU mode if there is no application flashed to the device _or_ if a button is held down when booting.

### Runtime

In runtime mode, it is expected to be running where your main firmware _would be_.
In this mode it can be used to read/write the actual bootloader on the device.
This is not a completely risk free process since if you wipe your bootloader and then power cycle you cant boot. But it does mean it can keep running _whileever_ you have power so you can fix it.

---

Based on the awesome work by https://github.com/davidgfnet/stm32-dfu-bootloader
and huge kudos for the framework https://github.com/adamheinrich/cm-makefile.
Based on ideas and a lot of work done by @perillamint https://github.com/eDesignOSS/ts100-bl-flasher.
This can be thought of as version _2_.
