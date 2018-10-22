#/bin/bash
make clean
make
sudo stty -F /dev/ttyUSB0 raw
sudo bin/JVSE
