#ifndef MAPPING_H_
#define MAPPING_H_

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>

#include "io.h"
#include "config.h"
#include "definitions.h"

#define MAX_EV_ITEMS 1024

struct MappingThreadArguments
{
    char eventPath[4096];
    char mappingPathIn[4096];
    char mappingPathOut[4096];
};

typedef struct
{
    /* To keep hold of the mappings */
    MappingOut analogueMapping[MAX_EV_ITEMS];
    MappingOut keyMapping[MAX_EV_ITEMS];

    /* Initial mappings from config */
    MappingIn insideMappings[MAX_EV_ITEMS];
    MappingOut outsideMappings[MAX_EV_ITEMS];

    /* Counts for mappings */
    int insideCount;
    int outsideCount;

    /* Running the devices */
    int deviceFd;
} Mapping;
int processMaps(Mapping *m);
MappingOut *findMapping(Mode mode, Mapping *m);
void printMapping(Mapping *m);
void *deviceThread(void *_args);
int startThread(char *eventPath, char *mappingPathIn, char *mappingPathOut);
int startWiiThread(char *eventPath, char *mappingPathIn, char *mappingPathOut);
void stopThreads();
#endif // MAPPING_H_
