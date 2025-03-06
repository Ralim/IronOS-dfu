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

Run this command with the iron connected via USB, and it will create a file called backup.bin in the current folder. this will be a copy of your bootloader for you to keep should you wish to return.
Due to this being copright Miniware, the bootloader files will _NOT_ be hosted in this repo.
However, we will publish known checksums of the backups so you can check if yours is a known one or not.

### TS80,TS80P,TS100

`sudo dfu-util -d 28e9:0189 -U backup.bin  -s 0x08000000:0x4000`

### S60

`sudo dfu-util -d 28e9:0189 -U backup.bin  -s 0x08000000:0x4400`

### S60P

`sudo dfu-util -d 28e9:0189 -U backup.bin  -s 0x08000000:0x5000`

### S99

`sudo dfu-util -d 28e9:0189 -U backup.bin  -s 0x08000000:0x4C00`

### MHP30

`sudo dfu-util -d 28e9:0189 -U backup.bin  -s 0x08000000:0x8000`

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

If you have lost your backup, and you need one; if you contact @ralim really nicely, and can prove your iron is legitament and not a clone, they _may_ help you out. **Do not open Github issues asking for files, these will be closed.**

## Checking your bootloader backup is valid

Known recorded MD5 checksums:

| Device | DFU Version | MD5                              |
| :----- | :---------: | -------------------------------- |
| TS100  |    3.42     | CD1038179B3FDA32E75D6F300D153CB3 |
| TS100  |    3.43     | F67879424D73345E4BDDCA69A4F9C75C |
| TS100  |    3.45     | 17FFF8A9D561B226D1DDC2D19BA2198E |
| TS80P  |    3.50     | 1805EC83F64C74DD89F87A1B57B7E631 |
| TS80   |    3.45     | FADAE45B4249D4F156C30B7D4B0A853E |
| S60P   |   V14.00    | 631C8823D84D3F53ED80266ACE37139E |
| S99    |   V12.00    | ca373fe47f4d3f736479e671c481fd6e |

In the mean time, you can validate if your backup looks valid by loading it into [hexed.it](https://hexed.it/).

The beginning of the file should start with repeating patterns of bytes that look roughly like the following, where its a set of repeating bytes every 4 bytes. The actual values will be different for every bootloader build; but the pattern will be roughly the same.

```C
	0x18, 0x31, 0x00, 0x20, 0x45, 0x2F, 0x00, 0x08, 0x09, 0x2F, 0x00, 0x08,
	0x0B, 0x2F, 0x00, 0x08, 0x0D, 0x2F, 0x00, 0x08, 0x0F, 0x2F, 0x00, 0x08,
	0x11, 0x2F, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x2F, 0x00, 0x08,
	0x15, 0x2F, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x17, 0x2F, 0x00, 0x08,
	0x19, 0x2F, 0x00, 0x08, 0x89, 0x2F, 0x00, 0x08, 0x8D, 0x2F, 0x00, 0x08,
	0x91, 0x2F, 0x00, 0x08, 0x95, 0x2F, 0x00, 0x08, 0x99, 0x2F, 0x00, 0x08,
```

## Restoring your bootloader

If for some reason you want to restore your bootloader, you can flash the `.bin` backup file you downloaded.
Load the runtime back onto your iron

```
dfu-util -D runtime.dfu
```

then flash the `backup.bin` you created earlier

```
sudo dfu-util -d 0483:df11 -a 0 -D backup.bin -s 0x08000000
```

After this flash finishes, you will now have your backup restored to your iron.

## A note on newer bootloaders

Presumably to stop "clone" devices, Miniware has started adding this "demo-mode" lockout to their official firmware.
The way this works is that at the very end of the first 32K of flash, they encode a signature that is based on the STM32(or clone's) CPU ID.
For example, for a TS101 at the end of the bootloader I have `00007ff0: ffff eb35 4739 ca4b 341a bc4a 363e ffff`.

This means that you cant restore a backup from someone elses device _if_ you use the Miniware firmware.
You _can_ generate these signature bytes yourself fairly easily however, if you use a debugger to read the CPU ID from the STM32, you can generate the signature bytes yourself.
You will need to read the CPU ID from `0x1FFFF7E8` and then do a rolling XOR with `0x1FFFF7E8` to generate the check value.

This is why the reccomendation to backup your bootloader.
Also as a trend, if your device has this signature at the end of the bootloader, you _will_ have the flash locked and require a programmer to load the main bootloader.

If you do **not** plan to run Miniware firmware, this will not matter as IronOS (and other firmwares) as of present do not implement this largely pointless check.
