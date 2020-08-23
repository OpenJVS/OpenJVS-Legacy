#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "definitions.h"

#define DEFAULT_GLOBAL_CONFIG_PATH "/etc/openjvs/global_config"
#define DEFAULT_DEVICE_MAP_PATH "/etc/openjvs/maps/device/"
#define DEFAULT_ARCADE_MAP_PATH "/etc/openjvs/maps/arcade/"

#define MAX_STRING_LENGTH 1024

typedef struct JVSConfig
{
    char devicePath[MAX_STRING_LENGTH];
    int senseType;
    char defaultMapping[MAX_STRING_LENGTH];
    int defaultIO;
    int debugMode;
    int atomiswaveFix;
} JVSConfig;

JVSConfig *getConfig();

JVSStatus processConfig(char *filePath, char *custom_mapping);
int processInMapFile(char *filePath, MappingIn *mappingIn);
int processOutMapFile(char *filePath, MappingOut *mappingOut);

void print_mapping_in(MappingIn *mappingIn);

#endif // CONFIG_H_
