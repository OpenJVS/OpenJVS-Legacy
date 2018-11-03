#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Definitions.h"
#include "Control.h"

typedef struct {
  int value;
  int player;
  int type;
} InputStruct;


extern char portName[];
extern int allDeviceMode;
extern char* configPath;
extern InputStruct inputConfig[];
extern int inputConfigCount;

int initConfig();

#endif // CONFIG_H_
