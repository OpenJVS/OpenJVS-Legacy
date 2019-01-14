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

#include <sys/socket.h>
#include <arpa/inet.h>

#define port 10703
#define MAXDATASIZE 256
#define BUFFER_SIZE 0x8000

typedef long long		INT_64;
typedef unsigned long long	UINT_64;
typedef int 			INT_32;
typedef unsigned int 		UINT_32;
typedef short			INT_16;
typedef unsigned short		UINT_16;
typedef char 			INT_8;
typedef unsigned char 		UINT_8;
static int socket_fd;


/* Local declarations */
static INT_32 read_socket(char *recv_buf);
static void set_security_keycode(UINT_64 data);
static INT_32 set_mode_host(char *recv_buf);
static void set_information_dimm(UINT_32 crc, UINT_32 length);
static void upload_dimm(UINT_32 addr, char *buff, INT_32 mark, UINT_32 buff_size);
static void upload_file_dimm(char* gameFile);
static void restart_host();
static void set_time_limit(UINT_32 data);
static void print_usage();



extern UINT_32 crc32(UINT_32 crc, const void *buf, UINT_32 size);

int initNetboot();
void runNetboot();
void closeNetboot();
void *netbootThread(void *arg);

#endif // NETBOOT_H_
