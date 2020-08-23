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
#include <sys/time.h>

#include "Definitions.h"
/* Here you can set the serial device */
char* portName = "/dev/tty.SLAB_USBtoUART";

/* allDeviceMode replies to all requests regardless of deviceID */
int allDeviceMode = 1;

/* Variables used in the program */
int deviceID;
int serial;
char reply[256];
int replyCount = 0;

char players = 2;
char bytesPerPlayer = 2;
char playerSwitches[255];
char systemSwitches = 0x0;
char boardID[255];

char analogueChannels = 4;
char analogue[255];

char coin = 0;

/* Function definitions */
int main(void);
void init(void);
void writeEscaped(unsigned char byte);
unsigned char getByte();
void processPacket(unsigned char packet[], int packet_length, int packet_address);
void getPacket();
void sendReply();
int set_interface_attribs (int fd, int speed, int parity);
void writeByte(unsigned char byte);
void writeString(char* string);
void writeBytes(unsigned char bytes[], int size);

void setSystemSwitch(int bit, int value);
void setPlayerSwitch(int player, int bit, int value);
void setAnalogue(int channel, char value);
void incrementCoin();




#endif // JVSE_H_
