#define _BSD_SOURCE
#ifndef CONFIG_H_
#define CONFIG_H_

extern char* portName;
extern int allDeviceMode;
extern char* configPath;

int initConfig();

#endif // CONFIG_H_
