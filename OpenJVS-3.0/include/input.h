#ifndef INPUT_H_
#define INPUT_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <linux/input.h>
#include <ctype.h>

#include "config.h"
#include "mapping.h"

#define DEV_INPUT_EVENT "/dev/input"
#define EVENT_DEV_NAME "event"

typedef struct
{
    char name[256];
    char path[256];
} DeviceTree;

int initInput();
int scanInputs(void);
int isEventDevice(const struct dirent *dir);
void printDeviceTree();
void connectDevices();

#endif // INPUT_H_
