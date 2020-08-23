#include "input.h"

DeviceTree devices[256];
int deviceCount = 0;

int initInput()
{
    if (!scanInputs())
    {
        printf("Error: Failed to scan inputs correctly\n");
        return 0;
    }
    usleep(100);
    connectDevices();
    return 1;
}

int isEventDevice(const struct dirent *dir)
{
    return strncmp(EVENT_DEV_NAME, dir->d_name, 5) == 0;
}

void printDeviceTree()
{
    for (int i = 0; i < deviceCount; i++)
    {
        printf("Name: %s, Path: %s\n", devices[i].name, devices[i].path);
    }
}

void connectDevices()
{
    for (int i = 0; i < deviceCount; i++)
    {
        usleep(100);
        char temp[4096] = DEFAULT_DEVICE_MAP_PATH;
        strcat(temp, devices[i].name);
        if (access(temp, F_OK) != -1)
        {
            printf("Input: %s\n", devices[i].name);

            char mappingName[4096];
            strcpy(mappingName, DEFAULT_ARCADE_MAP_PATH);
            strcat(mappingName, getConfig()->defaultMapping);

            if (strcmp(devices[i].name, "nintendo-wii-remote-ir") == 0)
            {
                startWiiThread(devices[i].path, temp, mappingName);
            }
            else
            {
                startThread(devices[i].path, temp, mappingName);
            }
        }
    }
}

int scanInputs(void)
{
    struct dirent **namelist;

    int i, ndev, devnum;
    char *filename;

    ndev = scandir(DEV_INPUT_EVENT, &namelist, isEventDevice, alphasort);
    if (ndev <= 0)
    {
        return 0;
    }

    for (i = 0; i < ndev; i++)
    {
        char fname[512];
        int fd = -1;
        char name[256] = "???";

        snprintf(fname, sizeof(fname), "%s/%s", DEV_INPUT_EVENT, namelist[i]->d_name);
        fd = open(fname, O_RDONLY);
        if (fd >= 0)
        {
            ioctl(fd, EVIOCGNAME(sizeof(name)), name);
            close(fd);
        }

        for (int i = 0; i < strlen(name); i++)
        {
            name[i] = tolower(name[i]);
            if (name[i] == ' ' || name[i] == '/')
            {
                name[i] = '-';
            }
        }

        strcpy(devices[deviceCount].name, name);
        strcpy(devices[deviceCount].path, fname);
        deviceCount++;
        free(namelist[i]);
    }
    free(namelist);

    return 1;
}
