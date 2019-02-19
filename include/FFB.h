#ifndef FFB_H_
#define FFB_H_

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

int initFFB();
void runFFB();
void closeFFB();
void *FFBThread(void *arg);

#endif // FFB_H_
