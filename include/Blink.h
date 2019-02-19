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
#define POUT 12  /* P1-07 */

int GPIOUnexport(int pin);

int GPIOExport(int pin);

int GPIODirection(int pin, int dir);

int GPIORead(int pin);

int GPIOWrite(int pin, int value);

int testmain(int argc, char *argv[]);

#endif // BLINK_H_