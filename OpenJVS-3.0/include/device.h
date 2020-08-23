#ifndef DEVICE_H_
#define DEVICE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

#include "buffer.h"
#include "definitions.h"

/* Select timeout in ms*/
#define TIMEOUT_SELECT 500

int initDevice(char *devicePath);
int closeDevice();
int setSerialAttributes(int fd, int myBaud);
int setSerialLowLatency(int fd);

JVSStatus write_serial(/*int serial,*/ uint8_t *data, uint32_t data_len);
JVSStatus read_serial(/*int serial, */ Buffer *read_buffer);

#endif // DEVICE_H_
