[![Build Status](https://travis-ci.com/bobbydilley/OpenJVS.svg?branch=master)](https://travis-ci.com/bobbydilley/OpenJVS)

# OpenJVS

OpenJVS is a software replacement for the JVS I/O boards used in arcade systems. The only known system OpenJVS currently works with fully is the Sega Naomi. This is due to the fact that the Sega Naomi does not follow the correct procedures with the sense line, and so this allows us to use any USB to RS485 dongle to control it. OpenJVS is compatible with all other systems, but requires some circutry to run the Sense Line which is not yet released. The software is written in C and aims to run on a Raspberry Pi allowing users to use their existing peripherals such as keyboards, AimTraks, mice, joysticks and steering wheels to control the games.

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
make
cd scripts
./install
```

After you have installed OpenJVS, setup the configuration files (shown in the section below) and run OpenJVS as root by following the commands below:

```
sudo openjvs <name_of_map_file_if_not_default>
```

On a raspberry pi, you may need to run:

```
cd scripts
sudo run_with_rawmode <name_of_map_file_if_not_default>
```

This attempts to setup the serial port to go faster!

Make sure you run the program as root, or it cannot access the device properly.

## Config

The configuration file is called `~/.openjvs/global_config` and is stored in the current users home directory. For OpenJVS to work you need to give it the path of your RS485 device, and the path of one of the input devices.

The config file is setup as a list of key value pairs, with a single space as the delimeter. Starting a line with a `#` symbol will make that line a comment, and so won't be read. An example configuration file is below:

```
# Default configuration file for OpenJVS

DEVICE_PATH /dev/ttyUSB0
KEYBOARD_PATH /dev/input/kbd-0
MOUSE_PATH /dev/input/mouse-0
CONTROLLER_PATH /dev/input/event0
```

Both the controller, and mouse devices can be any sort of EVDEV device which supports the EV_ABS and EV_KEY commands. This for example could be two mice, one on the MOUSE_PATH and one on the CONTROLLER_PATH or 2 play station controllers, or one mouse and one playstation controller etc. They are simply named MOUSE/CONTROLLER to seperate them. This allows you to play with 2 players.

### Config Reference

Here I will note how to setup the config values for the maps:

Everything inside a `<` and `>` is a value that should be replaced with a number. Everything not in one of those is the keywords that should be written exactly, in the correct case. All values should be taken from `evtest` program on linux.

Please note, for `PLAYER` 0 means it will set the system keys, 1 means set player 1 and 2 means set player 2.

```
KEY_BIND <KEYBOARD_KEY> <PLAYER> <NAOMI_KEY>
MOUSE_KEY_BIND <KEYBOARD_KEY> <PLAYER> <NAOMI_KEY>
CONTROLLER_KEY_BIND <KEYBOARD_KEY> <PLAYER> <NAOMI_KEY>
```

For the next set of configuration values, please note that `ADD` will add the value specified to the value got from the mouse/controller before anything else happens. So for example if your joystick was reporting values of -128 to 127, if you set the `ADD` value to 128, it would now report between 0 and 255. You would then set the `MAX` to 255, as this is the maximum value it can produce. If your joystick was giving you values between 0 and 1024 then you would set the `MAX` to 1024 and the `ADD` to 0.

```
MOUSE_ABS_BIND <ABS_EVENT_CODE> <NAOMI_CHANNEL> <ADD> <MAX>
CONTROLLER_ABS_BIND <ABS_EVENT_CODE> <NAOMI_CHANNEL> <ADD> <MAX>
```

You may want to reverse the analogue values that you are sending. So that 0 becomes 255, and 255 becomes 0. For example on Airline Pilots the accelerator is set to full when 0 is sent, and is set to off when 255 is sent. For this you can use the analogue flip  command.


```
ANALOGUE_FLIP <ANALOGUE_CHANNEL>
```

The next set of commands allow keys to set analogue channels. The `ANALOGUE_DEFAULT` command will set the resting value of that analogue channel when nothing is attempting to move it.

```
ANALOGUE_DEFAULT <CHANNEL> <VALUE>
```

The `KEY_PLUS` command will give a keyboard key the ability to set the value of an analogue channel. Again if `PLAYER` is set to 0 then system keys will be set.

```
KEY_PLUS <KEYBOARD_KEY> <CHANNEL> <VALUE>
```

## Maps

The map files are used to tell OpenJVS what keys on the input device are mapped to which inputs on the JVS arcade system. The default map is stored in `~/.openjvs/maps/default_config` and contains an example to play Crazy Taxi on the Sega Naomi with a keyboard. 

To use a different map, simply place the map file in the `~/.openjvs/maps/` directory, and place the map file name at the end of the command. For example, if the map was called `lets-go-jungle-ps4`, you would run.

```
sudo openjvs lets-go-jungle-ps4
```
