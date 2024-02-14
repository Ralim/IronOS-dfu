# Using this as your DFU bootloader

This software can be used to replace the bootloader built into your device from Miniware. This bootloader is only _designed_ to boot IronOS (its only tested with IronOS) but it _should_ work with the stock firmware too.

This DFU loader replaces the often troubled usb-disk method with the well known dfu-util. This is the same method used on the Pinecil, and lets all your devices enjoy more stable updating.

## Notice

There has been a report that TS100 units shipping with DFU 3.48 may have the lock bits set. If you have this DFU version its reccomended to have a cheap st-link or other SWD programmer in case recovery is needed.
It has also been noted that TS101 units, some MHP30 units and some newer TS80P units may also have the lock bits set.
On **all** Sequre parts the lock bits are set, so a programmer is required.
On these devices, flashing the runtime will work fine, so you can backup your bootloader. However if you use the runtime to flash the main bootloader it will not work and your device will loose its bootloader.
To work around this, the best action is to use a cheap SWD programmer to connect to the device and remove the lock bits. You can then flash your old bootloader or this new one at will.

On Miniware devices, the newer ones have the SWD pins wired up to D+/D-.
On Sequre devices the pads are under the OLED but are clearly labeled (V=3.3V,C=SWCLK,D=SWDIO,G=GND).

## Flashing this bootloader

To flash this as your main DFU bootloader, you need to flash the correct version to the bootloader location on your Iron.
To flash this to your bootloader partition, you cant be running from the bootloader located in that location.
Working around the restriction of the default bootloader not supporting update-in-place, this firmware supports a `runtime` mode; which allows you to run the bootloader from the application part of flash.

Before flashing this bootloader, **PLEASE** backup your existing bootloader and store it safely for future use. This also ensures your system is working with `dfu-util` and you need to do most of this anyway to flash this bootloader.

[Please follow these steps to flash the runtime version of this code, and then backup your existing bootloader](BackUp.md)

Once you have made a backup of your existing bootloader, and you have the `runtime` version of this bootloader running, you can now use `dfu-util` to flash the replacement bootloader.

---

!! WARNING !!

You are about to replace the bootloader on your device. This operation carries some risk, since if you break this bootloader you cant boot into the runtime code anymore. Recovering from a mistake can require a (cheap) st-link or similar arm debugger adapter.
This code has been test a _lot_ by me; but that does not mean that it will work perfectly on your pc.
It is highly reccomended to make sure you took a valid backup of your bootloader, and you were comfortable with the `dfu-util` command.
If anything goes wrong **do not power down your iron**, if you saw any "Erase" progress occur you _must_ restore your old bootloader before you power cycle. If the error occured before the "Erase" or "Download" commands you _may_ be fine. But its best to run the restore first.

---

To flash the bootloader version of the firmware, with the runtime code running; execute this command

`dfu-util -D bootloader.dfu`

And if this works at the end you should see no errors and see "Download done".
At this point the moment of truth is to reboot your iron and check it boots up correctly.
If the iron boots correctly, you should also be able to enter into the bootloader by holding the front button (akin to the original bootloader).
