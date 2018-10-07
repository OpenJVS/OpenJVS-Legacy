#define _BSD_SOURCE
#ifndef UTILITIES_H_
#define UTILITIES_H_

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


int set_interface_attribs (int fd, int speed, int parity);

#endif // UTILITIES_H_
