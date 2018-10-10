# OpenJVS

## Introduction

This repository will include resources, and the OpenJVS emulator written in C/C++.

This is the new and improved version of the JVSEmulator written in python at http://github.com/bobbydilley/JVSEmulator.

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
