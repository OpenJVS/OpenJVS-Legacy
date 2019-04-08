# OpenJVS

OpenJVS is a software emulator for arcade I/O boards. It currently supports all JVS based arcade systems, and a range of USB perhipherals.

## Requirements

- Raspberry Pi 2/3
- RS485 Converter
- Range of USB perhiperals

## Installation

OpenJVS is targeted to run on a Raspberry Pi 2/3 running Ubuntu Linux. You can download the correct version of Ubuntu [here]().

Firstly, make sure that you have all the correct dependencies by typing the command below:

```
sudo apt install git build-essential evtest
```

Next, get the source files for the project by typing the commands below:

```
git clone https://github.com/bobbydilley/OpenJVS
cd OpenJVS
```

Finally create the project and install it by typing the commands below:

```
make
make install
```

## Configuration

OpenJVS by default stores settings in a file at `/etc/OpenJVS/global_config`. Before running OpenJVS you should go here to modify the settings so that they correspond to your setup. The most important settings are listed below:

```
DEVICE_PATH       <path_of_RS485_device>
CONTROLLER_PATH   <path_of_usb_controller>
MOUSE_PATH        <path_of_usb_mouse>
KEYBOARD_PATH     <path_of_usb_keyboard>
```

The _device path_ should be filled in, and will likely be `/dev/ttyUSB0` as long as only have 1 serial device plugged in. You must set at least 1 of the _controller path_, _mouse path_ or _keyboard path_ attributes. To find our the paths to your usb perhipherals you can run the `evtest` command to see a list of device names and their paths.

## Running

Initially to run OpenJVS you must type the command below in your shell:

```
openjvs
```
