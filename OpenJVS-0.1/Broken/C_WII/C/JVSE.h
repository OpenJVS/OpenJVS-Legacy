#ifndef JVSE_H_
#define JVSE_H_

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "Definitions.h"

/* Here you can set the serial device */
char* portName = "/dev/tty.SLAB_USBtoUART";

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


#endif // JVSE_H_
