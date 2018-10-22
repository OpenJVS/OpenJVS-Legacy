#include "Keyboard.h"

int kbd_fd;

int initKeyboard() {
  if((kbd_fd = open("/dev/input/kbd", O_RDONLY)) == -1) {
    printf("JVSE: Failed to open keyboard file descriptor.");
    return -1;
  }
  return 0;
}

void closeKeyboard() {
  close(kbd_fd);
}
