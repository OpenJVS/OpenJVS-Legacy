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
#include <signal.h>
#include <sys/select.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#include "Control.h"
#include "Utilities.h"
#include "Definitions.h"
#include "Config.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Controller.h"
#include "Netboot.h"
#include "Blink.h"
#include "CircBuffer.h"

#ifdef WII_INCLUDE
  #include "Wii.h"
#endif

#ifdef FFB_INCLUDE
  #include "FFB.h"
#endif

#define MAJOR_VERSION 1
#define MINOR_VERSION 3

/* Select timeout in ms*/
#define TIMEOUT_SELECT 500

/* Timeout in sec after the receive buffer will be flushed and Sync-line set to not addressed */
#define TIMEOUT_INTER_BYTE 10

#define LINEAR_BUFFER_SIZE 512

#define	debug_line() printf("here: %s:%s:%u \n", __FILE__, __FUNCTION__ ,__LINE__);

#define new_min(x,y) ((x) <= (y)) ? (x) : (y)

/* Function definitions */
void intHandler(int dummy);
int main( int argc, char* argv[] ) ;
void writeEscaped(unsigned char byte);
unsigned char getByte();
void getPacket();
void sendReply();
void writeByte(unsigned char byte);
void writeString(char* string);
void writeBytes(unsigned char bytes[], int size);
void syncFloat();
void syncGround();

uint16_t jvs_get_analog_mask(void);
void jvs_set_analog_mask(uint16_t mask);

uint16_t jvs_get_analog_max(void);
void jvs_set_analog_max(uint16_t max);

open_jvs_status_t jvs_get_io_profile(jvs_io_t ** jvs_io_p);
void jvs_set_io_profile(jvs_io_t * jvs_profile_p);

#endif // JVSE_H_
