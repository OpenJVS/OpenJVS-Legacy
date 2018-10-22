#include "Keyboard.h"

int kbd_fd;

int initKeyboard() {
  if((kbd_fd = open("/dev/input/kbd", O_RDONLY)) == -1) {
    printf("JVSE: Failed to open keyboard file descriptor.");
    return -1;
  }
  return 0;
}

void runKeyboard() {
/*  input_event event;
  while (read(kbd_fd, &event, sizeof(input_event)) > 0) {
    if (event.type == EV_KEY) {
      if (event.value == KEY_PRESS) {
        printf("PRESS %d\n", event.code);
      }
      else if (event.value == KEY_RELEASE) {
        printf("RELEASE %d\n", event.code);
      }
    }
  }*/
}

void closeKeyboard() {
  close(kbd_fd);
}
