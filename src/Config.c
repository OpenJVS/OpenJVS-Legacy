#include "Config.h"

/* Here you can set the serial device */
char portName[1024] = "/dev/ttyUSB0";

/* allDeviceMode replies to all requests regardless of deviceID */
int allDeviceMode = 1;

/* Set the path of the config file */
char* configPath = ".config";

int initConfig() {
  FILE *fp;
  char buffer[1024];

  if((fp = fopen(".config", "r")) != NULL) {
    
    fgets(buffer, 1024, fp);
    while(!feof(fp)) {
      if(buffer[0] != '#' && buffer[0] != 0 && strcmp(buffer, "") != 0) {
        char *token = strtok(buffer, " ");
	if(strcmp(token, "DEVICE_PATH") == 0) {
		token = strtok(NULL, " ");
		if(token[strlen(token) - 1] == '\n') {
		  token[strlen(token) - 1] = '\0';
		}
		strcpy(portName, token);
	}
      }
      fgets(buffer, 1024, fp);
    }
  } else {
    printf("JVSE: Couldn't open the config file!\n");
    return -1;
  }


  fclose(fp);

  return 0;
}
