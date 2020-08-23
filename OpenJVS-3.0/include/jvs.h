#ifndef JVS_H_
#define JVS_H_

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

#include "device.h"
#include "constants.h"
#include "io.h"
#include "config.h"

/* Global definitions */

extern int debugEnabled;

typedef struct
{
    //    uint8_t destination;
    uint32_t length;

    uint8_t data[MAX_PACKET_SIZE];
} JVSPacket;

JVSStatus initJVS(char *devicePath);
int disconnectJVS();
int writeCapabilities(JVSPacket *outputPacket, JVSCapabilities *capabilities);
int readPacket(JVSPacket *packet);
int writePacket(JVSPacket *packet);

JVSStatus jvs_do(void);

JVSStatus find_start_of_message(Buffer *readBuffer);
JVSStatus decode_escape_circ(Buffer *readBuffer, JVSPacket *out_packet_decoded, uint32_t *out_raw_length);
JVSStatus check_message(JVSPacket *packet);
uint8_t calc_checksum(uint8_t *message, uint8_t len);
JVSStatus encode_escape(JVSPacket *packet);

#endif // JVS_H_
