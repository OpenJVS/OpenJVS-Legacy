#include "Config.h"

/* Here you can set the serial device */
char portName[2024] = "/dev/ttyUSB0";
char keyboardName[2024] = "/dev/ttyUSB0";
char mouseName[2024] = "/dev/ttyUSB0";
char controllerName[2024] = "/dev/ttyUSB0";
char mapName[2024] = "default_config";

char romDirectory[2024] = "/etc/OpenJVS/roms/";
char netbootIP[2024] = "10.0.0.111";

/* allDeviceMode replies to all requests regardless of deviceID */
int allDeviceMode = 1;
int netboot = 0;

/* Set the path of the config file */
char * configPath = ".config";

/* Set the absolute values */
int MouseAbsChannel[1024];
int MouseAbsMax[1024];
int MouseAbsAdd[1024];
int ControllerAbsChannel[1024];
int ControllerAbsMax[1024];
int ControllerAbsAdd[1024];

/* Setup the modifiers */
int AnalogueFlip[1024];

/* Setup input structures */
InputStruct inputConfig[1024];
InputStruct mouseInputConfig[1024];
InputStruct controllerInputConfig[1024];

int initConfig() {
    for (int i = 0; i < 1024; i++) {
        inputConfig[i] = (InputStruct) {.value = -1, .player = -1, .type = -1
        };
        mouseInputConfig[i] = (InputStruct) {.value = -1, .player = -1, .type = -1
        };
        controllerInputConfig[i] = (InputStruct) {.value = -1, .player = -1, .type = -1
        };
    }

    for (int i = 0; i < analogueChannels; i++) {
        analogueDefault[i] = 0;
    }

    for (int i = 0; i < 1024; i++) {
        MouseAbsChannel[i] = -1;
        MouseAbsMax[i] = -1;
        MouseAbsAdd[i] = 0;

        ControllerAbsChannel[i] = -1;
        ControllerAbsMax[i] = -1;
        ControllerAbsAdd[i] = 0;

	AnalogueFlip[i] = 0;
    }

    char fileName[1024];
    strcat(strcpy(fileName, "/etc/OpenJVS"), "/global_config");
    parseConfig(fileName);

    strcat(strcpy(fileName, "/etc/OpenJVS"), "/maps/");
    strcat(fileName, mapName);
    parseConfig(fileName);
}

int parseConfig(char * fileName) {
    FILE * fp;
    char buffer[1024];
    if ((fp = fopen(fileName, "r")) != NULL) {
        fgets(buffer, 1024, fp);
        while (!feof(fp)) {
            if (buffer[0] != '#' && buffer[0] != 0 && strcmp(buffer, "") != 0) {
                char * token = strtok(buffer, " ");

                /* Global Configurations */

                if (strcmp(token, "ROM_DIR") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    strcpy(romDirectory, token);
                }
                
		if (strcmp(token, "NETBOOT_ENABLE") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    netboot = atoi(token);
		}
		    
		if (strcmp(token, "NETBOOT_IP") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    strcpy(netbootIP, token);
		}
		    
                if (strcmp(token, "DEVICE_PATH") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    strcpy(portName, token);
                }

                if (strcmp(token, "KEYBOARD_PATH") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    strcpy(keyboardName, token);
                }

                if (strcmp(token, "MOUSE_PATH") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    strcpy(mouseName, token);
                }

                if (strcmp(token, "CONTROLLER_PATH") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    strcpy(controllerName, token);
                }

		/* Modifiers */
		if(strcmp(token, "ANALOGUE_FLIP") == 0) {
		    token = strtok(NULL, " ");
		    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int key = atoi(token);
		    AnalogueFlip[key] = 1;
		    //printf("Analogue Flip %d\n", key);
		}


                /* Bindings */

                /* KEY_BIND KEYBOARD_KEY PLAYER NAOMI_KEY */
                if (strcmp(token, "KEY_BIND") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int key = atoi(token);
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int player = atoi(token);
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int value = atoi(token);
                    inputConfig[key] = (InputStruct) {.value = value, .player = player, .type = CONFIG_KEY_BIND
                    };
                }


                if (strcmp(token, "MOUSE_KEY_BIND") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int key = atoi(token);
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int player = atoi(token);
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int value = atoi(token);
                    mouseInputConfig[key] = (InputStruct) {.value = value, .player = player, .type = CONFIG_KEY_BIND
                    };
                }

                // EventCode Channel Add MaxValue
                if (strcmp(token, "MOUSE_ABS_BIND") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int code = atoi(token);

                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int channel = atoi(token);

                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int add = atoi(token);

                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int maxValue = atoi(token);

                    MouseAbsChannel[code] = channel;
                    MouseAbsMax[code] = maxValue;
                    MouseAbsAdd[code] = add;
                }

                /* ControllerKey Player Value */
                if (strcmp(token, "CONTROLLER_KEY_BIND") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int key = atoi(token);
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int player = atoi(token);
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int value = atoi(token);
                    controllerInputConfig[key] = (InputStruct) {.value = value, .player = player, .type = CONFIG_KEY_BIND
                    };
                }

                // EventCode Channel Add MaxValue
                if (strcmp(token, "CONTROLLER_ABS_BIND") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int code = atoi(token);

                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int channel = atoi(token);

                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int add = atoi(token);

                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int maxValue = atoi(token);

                    ControllerAbsChannel[code] = channel;
                    ControllerAbsMax[code] = maxValue;
                    ControllerAbsAdd[code] = add;
                }

                if (strcmp(token, "KEY_PLUS") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int key = atoi(token);
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int value = atoi(token);
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int player = atoi(token);
                    inputConfig[key] = (InputStruct) {.value = value, .player = player, .type = CONFIG_KEY_PLUS
                    };
                }

                if (strcmp(token, "ANALOGUE_DEFAULT") == 0) {
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int key = atoi(token);
                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';
                    int value = atoi(token);
                    analogueDefault[key] = value;
                }
            }
            fgets(buffer, 1024, fp);
        }
    } else {
        printf("Failed to open config file at %s\n", fileName);
        return -1;
    }
    fclose(fp);
    return 0;
}
