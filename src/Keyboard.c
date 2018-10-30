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
	int switchLevel = 1;
	if(event.code == 2) setSystemSwitch(0, event.value);
	if(event.code == 3) setSystemSwitch(1, event.value);
	if(event.code == 4) setSystemSwitch(2, event.value);
	if(event.code == 5) setSystemSwitch(3, event.value);
	if(event.code == 6) setSystemSwitch(4, event.value);
	if(event.code == 7) setSystemSwitch(5, event.value);
	if(event.code == 8) setSystemSwitch(6, event.value);
	if(event.code == 9) setSystemSwitch(7, event.value);

	if(event.code == KEY_Q) setPlayerSwitch(0, 0, event.value);
	if(event.code == KEY_W) setPlayerSwitch(0, 1, event.value);
	if(event.code == KEY_E) setPlayerSwitch(0, 2, event.value);
	if(event.code == KEY_R) setPlayerSwitch(0, 3, event.value);
	if(event.code == KEY_T) setPlayerSwitch(0, 4, event.value);
	if(event.code == KEY_Y) setPlayerSwitch(0, 5, event.value);
	if(event.code == KEY_U) setPlayerSwitch(0, 6, event.value);
	if(event.code == KEY_I) setPlayerSwitch(0, 7, event.value);
	if(event.code == KEY_O) setPlayerSwitch(0, 8, event.value);
	if(event.code == KEY_P) setPlayerSwitch(0, 9, event.value);

	
	if(event.code == KEY_DOWN) setAnalogue(2, getAnalogue(2) - 1);
	if(event.code == KEY_UP) setAnalogue(2, getAnalogue(2) + 1);
	if(event.code == KEY_LEFT) setAnalogue(5, getAnalogue(5) - 1);
	if(event.code == KEY_RIGHT) setAnalogue(5, getAnalogue(5) + 1);
    }
  }
}

void closeKeyboard() {
    pthread_join(thread_id, NULL); 
    close(kbd_fd);
}
