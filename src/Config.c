#include "Config.h"



/* Here you can set the serial device */
char portName[1024] = "/dev/ttyUSB0";

/* allDeviceMode replies to all requests regardless of deviceID */
int allDeviceMode = 1;

/* Set the path of the config file */
char* configPath = ".config";

int inputConfigCount = 0;
InputStruct inputConfig[1024];

int initConfig() {
  for(int i = 0 ; i < 1024 ; i++) {
    inputConfig[i] = (InputStruct) { .value = -1, .player = -1, .type = -1 };
  }


  for(int i = 0 ; i < analogueChannels ; i++) {
    analogueDefault[i] = 0;
  }

  FILE *fp;
  char buffer[1024];
  char fileName[1024];
  strcat(strcpy(fileName, getenv("HOME")), "/.openjvs/global_config");
  if((fp = fopen(fileName, "r")) != NULL) {
    fgets(buffer, 1024, fp);
    while(!feof(fp)) {
      if(buffer[0] != '#' && buffer[0] != 0 && strcmp(buffer, "") != 0) {
        char *token = strtok(buffer, " ");


        if(strcmp(token, "DEVICE_PATH") == 0) {
          token = strtok(NULL, " ");
          if(token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
          strcpy(portName, token);
        }

        if(strcmp(token, "KEY_BIND") == 0) {
          token = strtok(NULL, " ");
          if(token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
          int key = atoi(token);
          token = strtok(NULL, " ");
          if(token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
          int player = atoi(token);
          token = strtok(NULL, " ");
          if(token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
          int value = atoi(token);
          inputConfig[key] = (InputStruct) { .value = value, .player = player, .type = CONFIG_KEY_BIND };
        }

	
        if(strcmp(token, "KEY_PLUS") == 0) {
          token = strtok(NULL, " ");
          if(token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
          int key = atoi(token);
	  token = strtok(NULL, " ");
	  if(token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
	  int value = atoi(token);
          token = strtok(NULL, " ");
          if(token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
          int player = atoi(token);
          inputConfig[key] = (InputStruct) { .value = value, .player = player, .type = CONFIG_KEY_PLUS };
        }


        if(strcmp(token, "ANALOGUE_DEFAULT") == 0) {
          token = strtok(NULL, " ");
          if(token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
          int key = atoi(token);
          token = strtok(NULL, " ");
          if(token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
          int value = atoi(token);
          analogueDefault[key] = value;
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
