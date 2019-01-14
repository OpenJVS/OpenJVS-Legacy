#include "Netboot.h"

pthread_t thread_id;

int initNetboot() {
  return 0;
}

void runNetboot() {
    pthread_create(&thread_id, NULL, netbootThread, NULL);
    printf("Netboot Module Started\n");
    printf("Netboot IP %s\n", netbootIP);
}

void *netbootThread(void *arg) {
	return 0;
}

void closeNetboot() {
    pthread_join(thread_id, NULL);
}
