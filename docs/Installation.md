# Installation on Linux (Debian, Raspbian, Ubuntu)

## Overview

Making the JVSE project is simple and can be done on any computer with the correct setup. The steps are listed below, and then explained in more detail later on.

- Make sure `build-essential` and `git` is installed on your linux computer.
- Clone the repository using `git clone http://github.com/bobbydilley/OpenJVS`
- Goto the `scripts` directory and run `./install`.
- Modify the `~/.openjvs/global_config` file to your needs.
- Run `sudo openjvs` from anywhere.

## Details

### Installing the requirements

To install the requirements simply run the command below, pressing enter when it asks if you want to install.

```
sudo apt install build-essential git
```

### Cloning the repository

Navigate to somewhere on your computer where you would like to store the project files, and run the command below.

```
git clone http://github.com/bobbydilley/OpenJVS
cd OpenJVS
```

### Making and Installing

Navigate to the scripts directory, and run the install script. The commands are shown below.

```
cd scripts
sudo ./install
```

### Editing the configuration files

You will need to modify the `DEVICE_PATH` attribute to use your specific USB to RS485 converter. You should change this in the config file, and leave exactly 1 space between the `DEVICE_PATH` name and the value. The commands are shown below.

#### Nano

```
nano ~/.openjvs
```

#### Vim

```
vim ~/.openjvs
```

### Running the application

Now you are ready to run the application, simply run it as root and look at the output.

```
sudo openjvs
```


## Uninstalling the Project on Linux

To uninstall you can either run `sudo ./uninstall` from the `scripts` directory to automatically uninstall. You can also remove the files listed below manually.

- `/usr/bin/openjvs`
- `~/.openjvs`
