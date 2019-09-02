#include "Mouse.h"

int mouse_fd;
pthread_t thread_id;

int initMouse()
{
  if ((mouse_fd = open(mouseName, O_RDONLY)) == -1)
  {
    printf("Failed to open mouse file descriptor\n");
    return -1;
  }
  return 0;
}

void runMouse()
{
  pthread_create(&thread_id, NULL, mouseThread, NULL);
  printf("Mouse Module Started\n");
}

void *mouseThread(void *arg)
{
  struct input_event event;
  while (read(mouse_fd, &event, sizeof event) > 0)
  {

    if (event.type == EV_ABS)
    {
      if (MouseAbsChannel[event.code] != -1)
      {
        int scaledValue = MouseAbsAdd[event.code] + ((double)event.value / (double)MouseAbsMax[event.code]) * 255;
        setAnalogue(MouseAbsChannel[event.code], AnalogueFlip[MouseAbsChannel[event.code]] == 0 ? scaledValue : 255 - scaledValue);
      }
    }

    if (event.type == EV_KEY)
    {
      if (mouseInputConfig[event.code].player == 0)
      {
        setSystemSwitch(mouseInputConfig[event.code].value, event.value);
      }
      else
      {
        if (mouseInputConfig[event.code].type == CONFIG_KEY_BIND)
        {
          setPlayerSwitch(mouseInputConfig[event.code].player - 1, mouseInputConfig[event.code].value, event.value);
        }
      }
    }
  }
}

void closeMouse()
{
  pthread_join(thread_id, NULL);
  close(mouse_fd);
}
