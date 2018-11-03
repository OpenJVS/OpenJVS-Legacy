#include "Keyboard.h"

int kbd_fd;
pthread_t thread_id;

int initKeyboard() {
  if((kbd_fd = open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDONLY)) == -1) {
    printf("JVSE: Failed to open keyboard file descriptor.");
    return -1;
  }
  return 0;
}

void runKeyboard() {
    pthread_create(&thread_id, NULL, keyboardThread, NULL);
    printf("JVSE: Keyboard Listener Started\n");
}

void *keyboardThread(void *arg) {
  struct input_event event;
  while (read(kbd_fd, &event, sizeof(struct input_event)) > 0) {
    if (event.type == EV_KEY) {
      printf("Key %d\n", event.code);
      if(inputConfig[event.code].type != -1) {
        setPlayerSwitch(inputConfig[event.code].player, inputConfig[event.code].value, event.value);
        printf("Key found\n");
      }

    	if(event.code == 2) setSystemSwitch(0, event.value);
    	if(event.code == 3) setSystemSwitch(1, event.value);
    	if(event.code == 4) setSystemSwitch(2, event.value);
    	if(event.code == 5) setSystemSwitch(3, event.value);
    	if(event.code == 6) setSystemSwitch(4, event.value);
    	if(event.code == 7) setSystemSwitch(5, event.value);
    	if(event.code == 8) setSystemSwitch(6, event.value);
    	if(event.code == 9) setSystemSwitch(7, event.value);

    }
  }
}

void closeKeyboard() {
    pthread_join(thread_id, NULL);
    close(kbd_fd);
}
