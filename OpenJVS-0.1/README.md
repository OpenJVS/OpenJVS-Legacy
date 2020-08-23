# THIS PROJECT HAS MOVED

Development is now done on the C version over here: https://github.com/bobbydilley/OpenJVS

# Jamma Video System Emulator

Both the C and Python versions of this emulator are now in the repository under their appropriate titles.

The broken directory contains broken version of the code, or testing versions to be merged in at some other point.

## Compiling the C Version

On a Linux / Mac machine simply go into the C folder and type:

```
make
```

This will compile the program, and create a `JVSE` executable.

On most linux machines, you have to set the permissions of the USB to RS485 device, once located (this can be done by typing `ls /dev` while the device is unplugged, and then `ls /dev` when it is plugged in and spotting which device comes up the second time), you should then set its permissions like this.

```
sudo chmod 777 /dev/ttyUSB0
``` 

where `/dev/ttyUSB0` is the address of the USB to RS485 device.

To change the file path of the USB to RS485 device in the code, open JVSE.h and modify the appropriate setting there. You will then have to run `make` again.

To run the program simply type:

```
./JVSE
```

and it should start running.

## Issues with the C Version

It seems to only run on certain systems / operating systems. For example it runs fine on my Mac computer, however doesn't run properly on a Raspberry Pi or a Linux laptop. I have tried to modify the usleep() calls to change when its sleeping, however this hasn't seemed to help.

