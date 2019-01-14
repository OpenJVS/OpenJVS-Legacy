#ifndef NETBOOT_H_
#define NETBOOT_H_

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>

#include <stdlib.h>
#include <string.h>  // strerror
#include <errno.h>
#include <stdint.h>
#include <assert.h>

#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h>

#include "Control.h"
#include "Config.h"

int initNetboot();
void runNetboot();
void closeNetboot();
void *netbootThread(void *arg);

#endif // NETBOOT_H_
