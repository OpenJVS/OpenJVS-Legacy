#include "FFB.h"

int ffb_fd;
pthread_t thread_id;

int ffb_running = 0;
int ffb_serial;

int initFFB() {
  /* Setup the serial interface here */
  ffb_serial = open(ffbName, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);

  if (ffb_serial < 0) {
      printf("Failed to open RS232 FFB file descriptor\n");
      return -1;
  }
  set_interface_attribs(ffb_serial, B38400);
  ffb_running = 1;
  return 0;
}

void runFFB() {
    pthread_create(&thread_id, NULL, FFBThread, NULL);
    printf("FFB Module Started\n");
}

void *FFBThread(void *arg) {

  while (ffb_running) {
    unsigned char buffer[] = {
        0x00
    };
    int n = -1;
    while (n < 1) {
        //usleep(10);
        n = read(ffb_serial, buffer, 1);
    }
    printf("Serial FFB: %d", buffer[0]);
  }
}

void closeFFB() {
    pthread_join(thread_id, NULL);
    close(ffb_fd);
}
