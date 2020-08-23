#include <stdio.h>
#include <string.h>

#include "config.h"

JVSConfig config;

void trimToken(char *str, int maxlen)
{
    int length = strnlen(str, maxlen);

    /* Look for unwated paterrns and terminate the token*/
    for (int i = 0; i < length; i++)
    {
        char val = *(str + i);
        if (('\n' == val) || ('\r' == val))
        {
            *(str + i) = '\0';
        }
    }
}

JVSConfig *getConfig()
{
    return &config;
}

JVSStatus processConfig(char *filePath, char *custom_mapping)
{
    // Setup default values
    strcpy(config.devicePath, "/dev/ttyUSB0");
    strcpy(config.defaultMapping, "driving-generic");
    config.atomiswaveFix = 0;
    config.debugMode = 0;
    config.defaultIO = 1;
    config.senseType = 1;

    FILE *fp;
    char buffer[1024];
    if ((fp = fopen(filePath, "r")) != NULL)
    {
        fgets(buffer, 1024, fp);
        while (!feof(fp))
        {
            if (buffer[0] != '#' && buffer[0] != 0 && strcmp(buffer, "") != 0)
            {
                char *saveptr;
                char *token = strtok_r(buffer, " ", &saveptr);
                trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));

                /* Grab the Device Path */
                if (strcmp(token, "DEVICE_PATH") == 0)
                {
                    token = strtok_r(NULL, " ", &saveptr);
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));
                    strcpy(config.devicePath, token);
                }

                /* Grab sense type */
                if (strcmp(token, "SENSE_TYPE") == 0)
                {
                    token = strtok_r(NULL, " ", &saveptr);
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));
                    config.senseType = atoi(token);
                }

                /* Grab debug type */
                if (strcmp(token, "DEBUG_MODE") == 0)
                {
                    token = strtok_r(NULL, " ", &saveptr);
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));
                    config.debugMode = atoi(token);
                }

                /* Grab default mapping */
                if (strcmp(token, "DEFAULT_MAPPING") == 0)
                {
                    token = strtok_r(NULL, " ", &saveptr);
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));
                    strcpy(config.defaultMapping, token);
                }

                /* Get IO Choice */
                if (strcmp(token, "DEFAULT_IO") == 0)
                {
                    token = strtok_r(NULL, " ", &saveptr);
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));
                    config.defaultIO = atoi(token);
                }

                /* Get IO Choice */
                if (strcmp(token, "ATOMISWAVE_FIX") == 0)
                {
                    token = strtok_r(NULL, " ", &saveptr);
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));
                    config.atomiswaveFix = atoi(token);
                    if (config.atomiswaveFix)
                    {
                        printf("Warning: Running ATOMISWAVE analogue fix, make sure the IO is 8-bit.\n");
                    }
                }
            }
            fgets(buffer, 1024, fp);
        }
    }
    else
    {
        return OPEN_JVS_ERR_NULL;
    }
    fclose(fp);

    /* Use custom OutMapping if provided */
    if (custom_mapping != NULL)
    {
        strcpy(config.defaultMapping, custom_mapping);
    }

    return OPEN_JVS_ERR_OK;
}

void print_mapping_in(MappingIn *mappingIn)
{
    if (NULL != mappingIn)
    {
        printf("Type:%u Mode:%u Key/Channel:%u Min:%d Max:%d \n",
               mappingIn->type,
               mappingIn->mode,
               mappingIn->channel,
               mappingIn->min,
               mappingIn->max);
    }
}

int processInMapFile(char *filePath, MappingIn *mappingIn)
{
    int count = 0;
    FILE *fp;
    char buffer[1024];
    char *str_ptr = NULL;

    if ((fp = fopen(filePath, "r")) != NULL)
    {
        do
        {
            str_ptr = fgets(buffer, 1024, fp);
            if ((str_ptr != NULL) && (buffer[0] != '#') && (buffer[0] != 0) && (buffer[0] != '\r') && (buffer[0] != '\n') && (strcmp(buffer, "") != 0))
            {
                char *saveptr;
                char *token = strtok_r(buffer, " ", &saveptr);
                trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));

                InType type = KEY;
                /* KEY <CHANNEL> <MODE> */
                if (strcmp(token, "KEY") == 0 || strcmp(token, "ABS") == 0 || strcmp(token, "REV_ABS") == 0)
                {
                    int reverse = 0;
                    if (strcmp(token, "KEY") == 0)
                        type = KEY;
                    if (strcmp(token, "ABS") == 0)
                        type = ABS;
                    if (strcmp(token, "REV_ABS") == 0)
                    {
                        type = ABS;
                        reverse = 1;
                    }

                    if (strcmp(token, "REV_ABS") == 0)
                    {
                        type = ABS;
                        reverse = 1;
                    }
                    token = strtok_r(NULL, " ", &saveptr);
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));

                    int channel = atoi(token);

                    token = strtok_r(NULL, " ", &saveptr);
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));

                    Mode mode = modeStringToEnum(token);

                    // Bobby: is setting min/max here still necessary? These values are set later in deviceThread() according to the InputDevice?
                    int min = 0;
                    int max = 0;
                    if (type == ABS)
                    {
                        min = 0;
                        max = 255;
                    }

                    MappingIn tempMapping = {
                        .channel = channel,
                        .type = type,
                        .mode = mode,
                        .min = min,
                        .max = max,
                        .reverse = reverse};

                    mappingIn[count] = tempMapping;
                    count++;
                }
                else
                {
                    printf("config.c: processInMapFile: incorrect settings keyword (%s).\n", token);
                }
            }
        } while (!feof(fp));
    }
    return count;
}

int processOutMapFile(char *filePath, MappingOut *mappingIn)
{
    int count = 0;
    FILE *fp;
    char buffer[1024];
    char *str_ptr = NULL;
    if ((fp = fopen(filePath, "r")) != NULL)
    {
        do
        {
            str_ptr = fgets(buffer, 1024, fp);
            if ((str_ptr != NULL) && (buffer[0] != '#') && (buffer[0] != 0) && (buffer[0] != '\r') && (buffer[0] != '\n') && (strcmp(buffer, "") != 0))
            {
                char *saveptr;
                char *token = strtok_r(buffer, " ", &saveptr);
                InType type = KEY;
                /* KEY <CHANNEL> <MODE> */
                if (strcmp(token, "ROTARY") == 0 || strcmp(token, "ANALOGUE") == 0 || strcmp(token, "BUTTON") == 0 || strcmp(token, "SYSTEM") == 0 || strcmp(token, "COIN") == 0)
                {
                    if (strcmp(token, "ANALOGUE") == 0)
                        type = ANALOGUE;
                    if (strcmp(token, "BUTTON") == 0)
                        type = BUTTON;
                    if (strcmp(token, "SYSTEM") == 0)
                        type = SYSTEM;
                    if (strcmp(token, "ROTARY") == 0)
                        type = SYSTEM;
                    if (strcmp(token, "COIN") == 0)
                        type = COIN;

                    token = strtok_r(NULL, " ", &saveptr);
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));
                    int channel = atoi(token);

                    token = strtok_r(NULL, " ", &saveptr);
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));
                    Mode mode = modeStringToEnum(token);

                    /* Optional: Player Number*/
                    int player_number = 1;
                    token = strtok_r(NULL, " ", &saveptr);

                    if (token != NULL)
                    {
                        player_number = atoi(token);
                    }

                    MappingOut tempMapping = {
                        .channel = channel,
                        .type = type,
                        .mode = mode,
                        .player = player_number};

                    mappingIn[count] = tempMapping;
                    count++;
                }
                else
                {
                    printf("config.c: processOutMapFile: incorrect settings keyword:%x \n", buffer[0]);
                }
            }
        } while (!feof(fp));
    }
    return count;
}
