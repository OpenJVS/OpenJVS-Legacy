#ifndef FFB_H_
#define FFB_H_

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h>

#include "Utilities.h"
#include "Control.h"
#include "Config.h"

int initFFB();
void runFFB();
void closeFFB();
void *FFBThread(void *arg);

#endif // FFB_H_
