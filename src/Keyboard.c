#include "Keyboard.h"

int kbd_fd;
pthread_t thread_id;

int initKeyboard() {
    if ((kbd_fd = open(keyboardName, O_RDONLY)) == -1) {
        printf("Failed to open keyboard file descriptor\n");
        return -1;
    }
    return 0;
}

void runKeyboard() {
    pthread_create( & thread_id, NULL, keyboardThread, NULL);
    printf("Keyboard Module Started\n");
}

void * keyboardThread(void * arg) {
    struct input_event event;
    while (read(kbd_fd, & event, sizeof(struct input_event)) > 0) {
        if (event.type == EV_KEY) {
            /* Simple Key Bind */
            if (inputConfig[event.code].type == CONFIG_KEY_BIND) {
                if (inputConfig[event.code].player == 0) {
                    setSystemSwitch(inputConfig[event.code].value, event.value);
                } else {
                    setPlayerSwitch(inputConfig[event.code].player - 1, inputConfig[event.code].value, event.value);
                }
            }

            /* Full Analogue */
            if (inputConfig[event.code].type == CONFIG_KEY_PLUS) {
                if (event.value == 1) {
                    setAnalogue(inputConfig[event.code].value, inputConfig[event.code].player);
                } else if (event.value == 0) {
                    setAnalogue(inputConfig[event.code].value, analogueDefault[inputConfig[event.code].value]);
                }
            }

        }
    }
}

void closeKeyboard() {
    pthread_join(thread_id, NULL);
    close(kbd_fd);
}
