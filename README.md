[![Build Status](https://travis-ci.com/bobbydilley/OpenJVS.svg?branch=master)](https://travis-ci.com/bobbydilley/OpenJVS)

# OpenJVS

OpenJVS is a software replacement for the JVS I/O boards used in arcade systems such as the Sega Naomi, Triforce, Chihiro and Lindbergh as well as other systems by other manufacturers. The software is written in C and aims to run on a Raspberry Pi allowing users to use their existing peripherals such as keyboards, AimTraks, mice, joysticks and steering wheels to control the games.

This is the new version of the JVSEmulator repository at http://github.com/bobbydilley/JVSEmulator.

## Hardware

OpenJVS requires a RS232 to RS485 converter, wired to the JVS controller cable. Many of these are available, and come in different formats such as USB dongles, or chips that can be wired to the GPIO on a raspberry pi.

Instructions on building the cable can be found here: https://github.com/bobbydilley/JVSEmulator/wiki/The-JVSE-Cable

## Installation

### Raspberry Pi

OpenJVS supports a few Naomi games on the Raspberry Pi. To install run the commands below:

```
sudo apt install build-essential git
git clone http://github.com/bobbydilley/OpenJVS
cd OpenJVS
```

At this point you should navigate to the include/Config.h file, and edit it to the address of your serial device. The default is `dev/ttyUSB0` on the Raspberry Pi.

After you've made sure the device path is setup, run these commands.

```
make
sudo bin/JVSE
```

Make sure you run the program as root, or it cannot access the device properly.

### Mac OS

OpenJVS is known to work on Mac OS, and can be installed using the same method as on the Raspberry Pi. Brew should instead be used to install the initial software.

```
brew install build-essential git
```

### Debian x86

OpenJVS is known to work on debian on a standard x86 computer, and can be installed in exactly the same was as on the Raspberry Pi.
