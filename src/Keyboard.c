#include "Keyboard.h"

int kbd_fd;
pthread_t thread_id;

int initKeyboard() {
  if((kbd_fd = open(keyboardName, O_RDONLY)) == -1) {
    printf("JVSE: Failed to open keyboard file descriptor\n");
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
      //printf("Key %d\n", event.code);
      
      /* Simple Key Bind */
      if(inputConfig[event.code].type == CONFIG_KEY_BIND) {
        setPlayerSwitch(inputConfig[event.code].player, inputConfig[event.code].value, event.value);
      }

      /* Full Analogue */
      if(inputConfig[event.code].type == CONFIG_KEY_PLUS) {
	if(event.value == 1) {
          setAnalogue(inputConfig[event.code].value, inputConfig[event.code].player);
	} else if(event.value == 0) {
	  setAnalogue(inputConfig[event.code].value, analogueDefault[inputConfig[event.code].value]);
	}
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
