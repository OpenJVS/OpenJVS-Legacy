#ifndef JVSE_H_
#define JVSE_H_

#define MAJOR_VERSION 1
#define MINOR_VERSION 3

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

#include "Control.h"
#include "Utilities.h"
#include "Definitions.h"
#include "Config.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Controller.h"
#include "Netboot.h"
#include "Blink.h"

#ifdef WII_INCLUDE
#include "Wii.h"
#endif

#ifdef FFB_INCLUDE
#include "FFB.h"
#endif

/* Variables used in the program */
int deviceID;
int serial;
char reply[256];
int replyCount = 0;

/* Version number of the program */
int majorVersion = 1;
int minorVersion = 3;

/* Function definitions */
void intHandler(int dummy);
int main(int argc, char *argv[]);
void writeEscaped(unsigned char byte);
unsigned char getByte();
void processPacket(unsigned char packet[], int packet_length, int packet_address);
void getPacket();
void sendReply();
void writeByte(unsigned char byte);
void writeString(char *string);
void writeBytes(unsigned char bytes[], int size);
void syncFloat();
void syncGround();

#endif // JVSE_H_
