#ifndef BLINK_H_
#define BLINK_H_

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define PIN  24 /* P1-18 */
#define POUT 4  /* P1-07 */

static int
GPIOUnexport(int pin);

static int
GPIOExport(int pin);

static int
GPIODirection(int pin, int dir);

static int
GPIORead(int pin);

static int
GPIOWrite(int pin, int value);

int
main(int argc, char *argv[]);

#endif // BLINK_H_
