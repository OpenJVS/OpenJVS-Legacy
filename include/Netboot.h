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

typedef long long		INT_64;
typedef unsigned long long	UINT_64;
typedef int 			INT_32;
typedef unsigned int 		UINT_32;
typedef short			INT_16;
typedef unsigned short		UINT_16;
typedef char 			INT_8;
typedef unsigned char 		UINT_8;

extern UINT_32 crc32(UINT_32 crc, const void *buf, UINT_32 size);

int initNetboot();
void runNetboot();
void closeNetboot();
void *netbootThread(void *arg);

#endif // NETBOOT_H_
