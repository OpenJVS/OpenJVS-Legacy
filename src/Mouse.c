#include "Mouse.h"

int mouse_fd;
pthread_t thread_id;

int initMouse() {
  if((mouse_fd = open(mouseName, O_RDONLY)) == -1) {
    printf("JVSE: Failed to open mouse file descriptor\n");
    return -1;
  }
  return 0;
}

void runMouse() {
    pthread_create(&thread_id, NULL, mouseThread, NULL);
    printf("JVSE: Mouse Listener Started\n");
}

void *mouseThread(void *arg) {
  struct input_event event;
  while (read(mouse_fd, &event, sizeof event) > 0) {
    if(event.type == EV_ABS) {
      if(AbsChannel[event.code] != -1) {
        int scaledValue = ((double) event.value / (double) AbsMax[event.code]) * 255;
        setAnalogue(AbsChannel[event.code], scaledValue);
        //printf("Set Channel %d to %d\n", AbsChannel[event.code], scaledValue);
      }
    }

    if(event.type == EV_KEY) {
      /* Simple Key Bind */
      if(mouseInputConfig[event.code].type == CONFIG_KEY_BIND) {
        //printf("player %d key %d value %d\n", mouseInputConfig[event.code].player, mouseInputConfig[event.code].value, event.value);
        setPlayerSwitch(mouseInputConfig[event.code].player, mouseInputConfig[event.code].value, event.value);
      }
    }
  }
}

void closeMouse() {
    pthread_join(thread_id, NULL);
    close(mouse_fd);
}
