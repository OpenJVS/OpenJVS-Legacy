#ifndef WII_H_
#define WII_H_

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>

#include <stdlib.h>
#include <string.h> // strerror
#include <errno.h>
#include <stdint.h>
#include <assert.h>

#include <unistd.h> //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>

#include "Control.h"
#include "Config.h"

#include <time.h>
#include <stdarg.h>

#include <bluetooth/bluetooth.h>
#include <cwiid.h>

#include <math.h>

#define toggle_bit(bf, b)      \
	(bf) = ((bf)&b)            \
			   ? ((bf) & ~(b)) \
			   : ((bf) | (b))

int initWii();
void runWii();
void closeWii();
void *wiiThread(void *arg);
void print_state(struct cwiid_state *state);

#endif // WII_H_
