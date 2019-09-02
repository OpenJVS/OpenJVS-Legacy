#include "Controller.h"

int controller_fd;
pthread_t thread_id;

int initController()
{
  if ((controller_fd = open(controllerName, O_RDONLY)) == -1)
  {
    printf("Failed to open controller file descriptor\n");
    return -1;
  }
  return 0;
}

void runController()
{
  pthread_create(&thread_id, NULL, controllerThread, NULL);
  printf("Controller Module Started\n");
}

void *controllerThread(void *arg)
{
  struct input_event event;
  while (read(controller_fd, &event, sizeof event) > 0)
  {

    if (event.type == EV_ABS)
    {
      if (ControllerAbsChannel[event.code] != -1)
      {
        int scaledValue = ControllerAbsAdd[event.code] + ((double)event.value / (double)ControllerAbsMax[event.code]) * 255;
        setAnalogue(ControllerAbsChannel[event.code], AnalogueFlip[ControllerAbsChannel[event.code]] == 0 ? scaledValue : 255 - scaledValue);
      }
    }

    if (event.type == EV_KEY)
    {
      if (controllerInputConfig[event.code].player == 0)
      {
        setSystemSwitch(controllerInputConfig[event.code].value, event.value);
      }
      else
      {
        if (controllerInputConfig[event.code].type == CONFIG_KEY_BIND)
        {
          setPlayerSwitch(controllerInputConfig[event.code].player - 1, controllerInputConfig[event.code].value, event.value);
        }
      }
    }
  }
}

void closeController()
{
  pthread_join(thread_id, NULL);
  close(controller_fd);
}
