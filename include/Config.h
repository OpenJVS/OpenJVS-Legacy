#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern char portName[];
extern int allDeviceMode;
extern char* configPath;

int initConfig();

#endif // CONFIG_H_
