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

## Running

Initially to run OpenJVS you must type the command below in your shell:

```
openjvs
```
