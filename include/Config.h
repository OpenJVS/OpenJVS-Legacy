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

extern char mapName[];
extern char portName[];
extern char keyboardName[];
extern char mouseName[];
extern char controllerName[];
extern int allDeviceMode;
extern char* configPath;
extern InputStruct inputConfig[];

extern InputStruct mouseInputConfig[];
extern int MouseAbsChannel[];
extern int MouseAbsMax[];
extern int MouseAbsAdd[];

extern InputStruct controllerInputConfig[];
extern int ControllerAbsChannel[];
extern int ControllerAbsMax[];
extern int ControllerAbsAdd[];

int initConfig();
int parseConfig(char* fileName);
#endif // CONFIG_H_
