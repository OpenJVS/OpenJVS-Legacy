[![Build Status](https://travis-ci.com/bobbydilley/OpenJVS.svg?branch=master)](https://travis-ci.com/bobbydilley/OpenJVS)

# OpenJVS

OpenJVS is a software replacement for the JVS I/O boards used in arcade systems. The only known system OpenJVS currently works with is the Sega Naomi. This is due to the fact that the Sega Naomi does not follow the correct procedures with the sense line, and so this allows us to use any USB to RS485 dongle to control it. The software is written in C and aims to run on a Raspberry Pi allowing users to use their existing peripherals such as keyboards, AimTraks, mice, joysticks and steering wheels to control the games.

This is the new version of the JVSEmulator repository at http://github.com/bobbydilley/JVSEmulator.

## Hardware

OpenJVS requires a RS232 to RS485 converter, wired to the JVS controller cable. Many of these are available, and come in different formats such as USB dongles, or chips that can be wired to the GPIO on a raspberry pi.

Instructions on building the cable can be found here: https://github.com/bobbydilley/JVSEmulator/wiki/The-JVSE-Cable

## Installation

### Debian Linux (PC + RaspberryPi)

OpenJVS supports a few Naomi games on the Raspberry Pi. To install run the commands below:

```
sudo apt install build-essential git
git clone http://github.com/bobbydilley/OpenJVS
cd OpenJVS
cd scripts
./install
```

After you have installed OpenJVS, setup the configuration files (shown in the section below) and run OpenJVS as root by following the commands below:

```
sudo openjvs
```

Make sure you run the program as root, or it cannot access the device properly.

## Config

The configuration file is called `~/.openjvs/global_config` and is stored in the current users home directory. For OpenJVS to work you need to give it the path of your RS485 device, and the path of one of the input devices.

The config file is setup as a list of key value pairs, with a single space as the delimeter. Starting a line with a `#` symbol will make that line a comment, and so won't be read. An example configuration file is below:

```
# Default configuration file for OpenJVS

DEVICE_PATH /dev/ttyUSB0
KEYBOARD_PATH /dev/input/kbd-0
MOUSE_PATH /dev/input/mouse-0
CONTROLLER_PATH /dev/input/controller-0
```

Both the controller, and mouse devices can be any sort of EVDEV device which supports the EV_ABS and EV_KEY commands. This for example could be two mice, one on the MOUSE_PATH and one on the CONTROLLER_PATH or 2 play station controllers, or one mouse and one playstation controller etc. They are simply named MOUSE/CONTROLLER to seperate them. This allows you to play with 2 players.

## Maps

The map files are used to tell OpenJVS what keys on the input device are mapped to which inputs on the JVS arcade system. The default map is stored in `~/.openjvs/maps/default_config` and contains an example to play Crazy Taxi on the Sega Naomi with a keyboard. 

To use a different map, simply place the map file in the `~/.openjvs/maps/` directory, and place the map file name at the end of the command. For example, if the map was called `lets-go-jungle-ps4`, you would run.

```
sudo openjvs lets-go-jungle-ps4
```
