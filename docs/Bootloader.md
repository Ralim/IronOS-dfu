# Using this as your DFU bootloader

## Flashing this bootloader

To flash this as your main DFU bootloader, you need to flash the correct version to the bootloader location on your Iron.
To flash this to your bootloader partition, you cant be running from the bootloader located in that location.
Working around the restriction of the default bootloader not supporting update-in-place, this firmware supports a `runtime` mode; which allows you to run the bootloader from the application part of flash.

Before flashing this bootloader, **PLEASE** backup your existing bootloader and store it safely for future use. This also ensures your system is working with `dfu-util` and you need to do most of this anyway to flash this bootloader.

[Please follow these steps to flash the runtime version of this code, and then backup your existing bootloader](BackUp.md)
