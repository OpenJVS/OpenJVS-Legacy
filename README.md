# OpenJVS

OpenJVS is a software replacement for the JVS I/O boards used in arcade systems such as the Sega Naomi, Triforce, Chihiro and Lindbergh as well as other systems by other manufacturers. The software is written in C and aims to run on a RaspberryPi allowing users to use their existing peripherals such as keyboards, AimTraks, mice, joysticks and steering wheels to control the games.

This is the new version of the JVSEmulator repository at http://github.com/bobbydilley/JVSEmulator.

## Hardware

OpenJVS requires a RS232 to RS485 converter, wired to the JVS controller cable. Many of these are available, and come in different formats such as USB dongles, or chips that can be wired to the GPIO on a raspberry pi.

## Installation

### Raspberry Pi

OpenJVS supports a few Naomi games on the raspberry pi. To install run the commands below:

```
sudo apt install build-essentials git
git clone http://github.com/bobbydilley/OpenJVS
cd OpenJVS
make
sudo bin/JVSE
```

### Mac OS

OpenJVS is known to work on Mac OS.

### Debian x86

OpenJVS is known to work on debian on a standard x86 computer.
