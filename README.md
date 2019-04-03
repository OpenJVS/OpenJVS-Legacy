[![Build Status](https://travis-ci.com/bobbydilley/OpenJVS.svg?branch=master)](https://travis-ci.com/bobbydilley/OpenJVS)

# OpenJVS


OpenJVS is a software emulator for JVS I/O boards found in many modern arcade machines. To use JVS a USB to RS485 connector is required, along with a linux computer such as the Raspberry Pi 2 with Ubuntu Linux. OpenJVS currently fully supports the Sega Naomi, and supports other platforms with additional hardware.

## Installation

Installation is described on a Raspberry Pi 2 with Ubuntu Server Linux which can be downloaded here: https://wiki.ubuntu.com/ARM/RaspberryPi. You should be able to download an official or unofficial image for your Raspberry Pi 2 or 3 form this link. This should also work on Ubuntu MATE for Raspberry Pi 2/3. It however does not work correctly on Debian/Arch on Pi's 1, 2 and 3.

Once the image has been installed, the USB to RS485 device and any controllers should be plugged in, and SSH enabled.


First make sure you have the required software installed to download, install and run OpenJVS.

```
sudo apt install build-essential git evtest
```

Now download the OpenJVS software using git.

```
git clone http://github.com/bobbydilley/OpenJVS
```

Now navigate into the downloaded folder, and run the install script.

```
cd OpenJVS/scripts
sudo ./install
```

OpenJVS is now installed and ready to use on your Raspberry Pi. See the Config heading to learn how to set it up. Once setup you can run it on a by using the following command

```
sudo openjvs-pi <map-name>
```


## Sync Line

The red wire coming from the JVS connector in the Sync line, and is required for use with the Lindbergh system. OpenJVS will operate the sync line on GPIO 12. You should connect the sync line directly to GPIO 12, and then a resistor ladder from GPIO 12 to GND.

## Config

To configure OpenJVS, each time it is run 2 seperate configuration files are read. Every time you start OpenJVS, initially the `/etc/OpenJVS/global_config` file is read to pick up global configuration values that won't change such as the path of your RS485 device. After this, a map configuration file is read from `/etc/OpenJVS/maps/<map-name>` where the _map name_ is the name of the game you would like to run, and this contains all the individual configration for that game.

The configuration files are set out as a list of key to a list of value pairs, where the keys are in capital letters, and followed by a space and then the values, each seperated by a space.

Both the controller, and mouse devices can be any sort of EVDEV device which supports the EV_ABS and EV_KEY commands. This for example could be two mice, one on the MOUSE_PATH and one on the CONTROLLER_PATH or 2 play station controllers, or one mouse and one playstation controller etc. They are simply named MOUSE/CONTROLLER to seperate them. This allows you to play with 2 players.

### Config Reference

#### Device Path
```
DEVICE_PATH <PATH>
```
This will set the path to the RS485 device that is connected to the Naomi. An example path would be `/dev/ttyUSB0`.

#### Keyboard Path
```
KEYBOARD_PATH <PATH>
```
This will set the path to the Keyboard device that is connected to the Raspberry Pi. An example path would be `/dev/input/event0`.

#### Mouse Path
```
MOUSE_PATH <PATH>
```
This will set the path to the Mouse device that is connected to the Raspberry Pi. An example path would be `/dev/input/event1`.

#### Controller Path
```
CONTROLLER_PATH <PATH>
```
This will set the path to the Controller device that is connected to the Raspberry Pi. An example path would be `/dev/input/event2`.

#### Keyboard Key Bind
```
KEY_BIND <KEYBOARD_KEY> <PLAYER> <NAOMI_KEY>
```
This will bind a keyboard key press, to a Naomi key press. `<KEYBOARD_KEY>` is the EVDEV key code of the key, `<PLAYER>` is the number of the player on the Naomi you wish to set the key for and `<NAOMI_KEY>` is the key on the Naomi you wish this keyboard key to change. Please note, if `<PLAYER>` is set to 0 this means it will set the system keys, 1 means set player 1 and 2 means set player 2.

#### Mouse Key Bind
```
MOUSE_KEY_BIND <KEYBOARD_KEY> <PLAYER> <NAOMI_KEY>
```
This will bind a mouse key press, to a Naomi key press. `<KEYBOARD_KEY>` is the EVDEV key code of the key, `<PLAYER>` is the number of the player on the Naomi you wish to set the key for and `<NAOMI_KEY>` is the key on the Naomi you wish this mouse key to change. Please note, if `<PLAYER>` is set to 0 this means it will set the system keys, 1 means set player 1 and 2 means set player 2.

#### Controller Key Bind
```
CONTROLLER_KEY_BIND <KEYBOARD_KEY> <PLAYER> <NAOMI_KEY>
```
This will bind a controller key press, to a Naomi key press. `<KEYBOARD_KEY>` is the EVDEV key code of the key, `<PLAYER>` is the number of the player on the Naomi you wish to set the key for and `<NAOMI_KEY>` is the key on the Naomi you wish this controller key to change. Please note, if `<PLAYER>` is set to 0 this means it will set the system keys, 1 means set player 1 and 2 means set player 2.

#### Mouse Analogue Bind
```
MOUSE_ABS_BIND <ABS_EVENT_CODE> <NAOMI_CHANNEL> <ADD> <MAX>
```
This will bind the analogue value denoted by `<ABS_EVENT_CODE>` coming from a mouse device to a Naomi analogue channel denoted by `<NAOMI_CHANNEL>`. The `<ADD>` command will add the value specified to the value recived from the mouse device before anything else happens. So for example if your mouse was reporting values of -128 to 127, if you set the `ADD` value to 128, it would now report between 0 and 255. You would then set the `<MAX>` to 255, as this is the maximum value it can produce. If your mouse was giving you values between 0 and 1024 then you would set the `<ADD>` to 0 and `<MAX>` to 1024. This allows the software to scale the values to ones that the Naomi is expecting.


#### Controller Analogue Bind
```
CONTROLLER_ABS_BIND <ABS_EVENT_CODE> <NAOMI_CHANNEL> <ADD> <MAX>
```
This will bind the analogue value denoted by `<ABS_EVENT_CODE>` coming from a controller device to a Naomi analogue channel denoted by `<NAOMI_CHANNEL>`. The `<ADD>` command will add the value specified to the value recived from the controller device before anything else happens. So for example if your controller was reporting values of -128 to 127, if you set the `ADD` value to 128, it would now report between 0 and 255. You would then set the `<MAX>` to 255, as this is the maximum value it can produce. If your controller was giving you values between 0 and 1024 then you would set the `<ADD>` to 0 and `<MAX>` to 1024. This allows the software to scale the values to ones that the Naomi is expecting.


#### Analogue Flip
```
ANALOGUE_FLIP <ANALOGUE_CHANNEL>
```
You may want to reverse the analogue values that you are sending. So that 0 becomes 255, and 255 becomes 0. For example on Airline Pilots the accelerator is set to full when 0 is sent, and is set to off when 255 is sent. For this you can use the analogue flip  command. The `<ANALOGUE_CHANNEL>` corresponds to the channel you wish to flip.

#### Analogue Default
```
ANALOGUE_DEFAULT <CHANNEL> <VALUE>
```
This command is used to allow keys to set analogue channels. The `<ANALOGUE_DEFAULT>` command will set the resting value of that analogue channel when nothing is attempting to move it, where `<CHANNEL>` is the Naomi channel you wish to set and `<VALUE>` is the resting value you wish to set.

#### Key Plus
```
KEY_PLUS <KEYBOARD_KEY> <CHANNEL> <VALUE>
```
The `KEY_PLUS` command will give a keyboard key the ability to set the value of an analogue channel. The `<KEYBOARD_KEY>` parameter refers to the EVDEV code of the key on the keyboard, and the `<CHANNEL>` and `<VALUE>` parameters refer to the Naomi analogue channel and the value of that channel when the key is pressed respectively.


## Netbooting

OpenJVS now supports netbooting. To get started with netbooting you must do the following things:

Place your roms in the `/etc/OpenJVS/roms/` directory, and name them exactly the same as the map file. So if you wanted to boot Crazy Taxi when you run the `crazy_taxi-ps4` map file, then name the rom crazy_taxi-ps4.
In the gobal configuration file in `/etc/OpenJVS/global_config` make sure that these are set:
```
ROM_DIR /etc/OpenJVS/roms/
NETBOOT_IP 10.0.0.111
```

Then in each game you would like to netboot simply add
```
NETBOOT_ENABLE 1
```
to the map file. Alternatively you can add this to the global_config file to netboot every game.

Now when you run `openjvs-pi crazy_taxi-ps4` OpenJVS will attempt to boot your Sega Naomi/Triforce/Chihiro with the correct game for that map file.

## Force Feedback

OpenJVS is now starting to support force feedback wheels, with the next versions. Please watch this space for updates on the FFB.

Wheels to be supported:

- Microsoft Sidewinder FFB Wheel

To run the force feedback module, in each game you would like to run it (or globally in global_config) add

```
FFB_ENABLE 1
```
