#define _BSD_SOURCE
#ifndef JVSE_H_
#define JVSE_H_

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

#include <bluetooth/bluetooth.h>
#include <cwiid.h>

#include "Definitions.h"

#define toggle_bit(bf,b)	\
	(bf) = ((bf) & b)		\
	       ? ((bf) & ~(b))	\
	       : ((bf) | (b))


int mot_x = 0;
int mot_y = 0;

/* Here you can set the serial device */
char* portName = "/dev/ttyUSB0";

/* Variables used in the program */
int deviceID;
int serial;
char reply[256];
int replyCount = 0;

/* Function definitions */
int main(void);
void writeEscaped(unsigned char byte);
unsigned char getByte();
void processPacket(unsigned char packet[], int packet_length, int packet_address);
void getPacket();
void sendReply();
int set_interface_attribs (int fd, int speed, int parity);
void writeByte(unsigned char byte);
void writeString(char* string);
void writeBytes(unsigned char bytes[], int size);

/* Wii Stuf */
void print_state(struct cwiid_state *state);



#endif // JVSE_H_
