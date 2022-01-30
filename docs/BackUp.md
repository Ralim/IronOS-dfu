# Backing up your existing bootloader

This DFU bootloader comes in two versions, a `runtime` and a `bootloader`.
The `runtime` mode allows running the bootloader from the application slot, so it does not (yet) replace your device bootloader.
This means you can use it to take a backup of the bootloader that came on your device; or use this to upgrade the bootloader on your device.
This also means you can _safely_ get a feel for using `dfu-util` before committing to using this as your primary bootloader.

Before doing this, you must have `dfu-util` installed on your machine _somehow_.

## Step 1: Load the runtime DFU code

Download from the latest release the `runtime.hex` file.
Program this onto your iron like a normal firmware, then power cycle your iron.

The Iron will show a IronOS logo and then show the text "DFU" with its version number.

## Step 2: Use dfu-util to read out your current bootloader

```
dfu-util -d 28e9:0189 -U backup.bin  -s 0x08000000:0x4000
```

Run this command with the iron connected via USB, and it will create a file called backup.bin in the current folder. this will be a copy of your bootloader for you to keep should you wish to return.
Due to this being copright Miniware, the bootloader files will _NOT_ be hosted in this repo.
However, we will publish known checksums of the backups so you can check if yours is a known one or not.

If it works, our output should look like this:

```
❯ dfu-util -d 28e9:0189 -U backup2.bin  -s 0x08000000:0x4000
dfu-util 0.11

Opening DFU capable USB device...
Device ID 28e9:0189
Device DFU version 011a
Claiming USB DFU Interface...
Setting Alternate Interface #0 ...
Determining device status...
DFU state(2) = dfuIDLE, status(0) = No error condition is present
DFU mode device DFU version 011a
Device returned transfer size 1024
DfuSe interface name: "Internal Flash "
Upload	[=========================] 100%        16384 bytes
Upload done.
```

If you have lost your backup, and you need one; if you contact @ralim really nicely, and can prove your iron is legitament and not a clone, they _may_ help you out.
