#include "Config.h"

/* Here you can set the serial device */
char* portName = "/dev/tty.SLAB_USBtoUART";

/* allDeviceMode replies to all requests regardless of deviceID */
int allDeviceMode = 1;

/* Set the path of the config file */
char* configPath = ".config";

int initConfig() {
  return 0;
}
