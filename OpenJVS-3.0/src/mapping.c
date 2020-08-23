#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/select.h>
#include "mapping.h"
#include "jvs.h"

void *wiiThread(void *_args);

#define test_bit(bit, array) (array[bit / 8] & (1 << (bit % 8)))

int processMaps(Mapping *m)
{
  for (int i = 0; i < m->insideCount; i++)
  {
    MappingOut *tempOutsideMapping = NULL;
    tempOutsideMapping = findMapping(m->insideMappings[i].mode, m);
    if (tempOutsideMapping != NULL)
    {
      switch (m->insideMappings[i].type)
      {
      case ABS:
        m->analogueMapping[m->insideMappings[i].channel] = *tempOutsideMapping;
        m->analogueMapping[m->insideMappings[i].channel].min = m->insideMappings[i].min;
        m->analogueMapping[m->insideMappings[i].channel].max = m->insideMappings[i].max;
        m->analogueMapping[m->insideMappings[i].channel].reverse = m->insideMappings[i].reverse;
        break;
      case KEY:
        m->keyMapping[m->insideMappings[i].channel] = *tempOutsideMapping;
        break;
      default:
        printf("Warning: Unknown inside mapping case, use ABS or KEY.\n");
      }
    }
    else
    {
      printf("Warning: This outside map does not support: %s \n", modeEnumToString(m->insideMappings[i].mode));
    }
  }
}

MappingOut *findMapping(Mode mode, Mapping *m)
{
  for (int i = 0; i < m->outsideCount; i++)
  {
    if (m->outsideMappings[i].mode == mode)
    {
      return &(m->outsideMappings[i]);
    }
  }
  return NULL;
}

void printMapping(Mapping *m)
{
  printf("Buttons\n");
  for (int i = 0; i < MAX_EV_ITEMS; i++)
  {
    if (m->keyMapping[i].type != NONE)
    {
      printf("\tEV_KEY %d -> %d (%d, %s)\n", i, m->keyMapping[i].channel, m->keyMapping[i].type, modeEnumToString(m->keyMapping[i].mode));
    }
  }
  printf("Analogue\n");
  for (int i = 0; i < MAX_EV_ITEMS; i++)
  {
    if (m->analogueMapping[i].type != NONE)
    {
      printf("\tEV_ABS %d [%d -> %d] -> %d  (%d, %s)\n", i, m->analogueMapping[i].min, m->analogueMapping[i].max, m->analogueMapping[i].channel, m->analogueMapping[i].type, modeEnumToString(m->analogueMapping[i].mode));
    }
  }
}

pthread_t threadID[256];
int threadCount = 0;
int threadsRunning = 1;

int startThread(char *eventPath, char *mappingPathIn, char *mappingPathOut)
{
  struct MappingThreadArguments *args = malloc(sizeof(struct MappingThreadArguments));
  strcpy(args->eventPath, eventPath);
  strcpy(args->mappingPathIn, mappingPathIn);
  strcpy(args->mappingPathOut, mappingPathOut);
  pthread_create(&threadID[threadCount], NULL, deviceThread, args);
  threadCount++;
}

int startWiiThread(char *eventPath, char *mappingPathIn, char *mappingPathOut)
{
  struct MappingThreadArguments *args = malloc(sizeof(struct MappingThreadArguments));
  strcpy(args->eventPath, eventPath);
  pthread_create(&threadID[threadCount], NULL, wiiThread, args);
  threadCount++;
}

void stopThreads()
{
  printf("Stopping threads\n");
  threadsRunning = 0;
  for (int i = 0; i < threadCount; i++)
  {
    pthread_join(threadID[i], NULL);
  }
}

void *deviceThread(void *_args)
{
  /* Device threads run with standard linux prio */
  setRealtimePriority(false);

  struct MappingThreadArguments *args = (struct MappingThreadArguments *)_args;
  char eventPath[4096];
  char mappingPathIn[4096];
  char mappingPathOut[4096];

  strcpy(eventPath, args->eventPath);
  strcpy(mappingPathIn, args->mappingPathIn);
  strcpy(mappingPathOut, args->mappingPathOut);

  free(args);

  Mapping m;

  memset(&m, 0, sizeof(m));

  m.insideCount = processInMapFile(mappingPathIn, m.insideMappings);
  m.outsideCount = processOutMapFile(mappingPathOut, m.outsideMappings);

  if ((m.deviceFd = open(eventPath, O_RDONLY)) < 0)
  {
    printf("mapping.c:initDevice(): Failed to open device file descriptor:%d \n", m.deviceFd);
    exit(-1);
  }

  processMaps(&m);

  struct input_event event;

  int flags = fcntl(m.deviceFd, F_GETFL, 0);
  fcntl(m.deviceFd, F_SETFL, flags | O_NONBLOCK);

  int axisIndex;
  uint8_t absoluteBitmask[ABS_MAX / 8 + 1];
  float percentageDeadzone;
  struct input_absinfo absoluteFeatures;

  memset(absoluteBitmask, 0, sizeof(absoluteBitmask));
  if (ioctl(m.deviceFd, EVIOCGBIT(EV_ABS, sizeof(absoluteBitmask)), absoluteBitmask) < 0)
  {
    perror("evdev ioctl");
  }

  for (axisIndex = 0; axisIndex < ABS_MAX; ++axisIndex)
  {
    if (test_bit(axisIndex, absoluteBitmask))
    {
      if (ioctl(m.deviceFd, EVIOCGABS(axisIndex), &absoluteFeatures))
      {
        perror("evdev EVIOCGABS ioctl");
      }
      m.analogueMapping[axisIndex].max = absoluteFeatures.maximum;
      m.analogueMapping[axisIndex].min = absoluteFeatures.minimum;
    }
  }

  //printMapping(&m);

  fd_set file_descriptor;
  struct timeval tv;

  while (threadsRunning)
  {
    bool data_to_read = false;

    FD_ZERO(&file_descriptor);
    FD_SET(m.deviceFd, &file_descriptor);

    /* set blocking timeout to TIMEOUT_SELECT */
    tv.tv_sec = 0;
    tv.tv_usec = TIMEOUT_SELECT * 1000;

    int n = select(m.deviceFd + 1, &file_descriptor, NULL, NULL, &tv);
    if (0 == n)
    {
      continue;
    }
    else if (n > 0)
    {
      if (FD_ISSET(m.deviceFd, &file_descriptor))
      {
        data_to_read = true;
      }
    }
    else
    {
      /* error from select */
    }

    if (data_to_read && (sizeof(event) == read(m.deviceFd, &event, sizeof(event))))
    {
      switch (event.type)
      {
      case EV_ABS:

        if (m.analogueMapping[event.code].type != NONE)
        {
          float x = event.value;
          float min = m.analogueMapping[event.code].min;
          float max = m.analogueMapping[event.code].max;

          if (m.analogueMapping[event.code].reverse)
          {
            float temp = min;
            min = max;
            max = temp;
          }

          // Scale to between 0 and 1 maybe? How does this actually work?
          double scaled = (x - min) / (max - min);

          if (m.analogueMapping[event.code].type == ANALOGUE)
          {
            setAnalogue(m.analogueMapping[event.code].channel, scaled);
          }
          else if (m.analogueMapping[event.code].type == ROTARY)
          {
            setRotary(m.analogueMapping[event.code].channel, scaled);
          }
        }
        break;
      case EV_KEY:
        if (m.keyMapping[event.code].type != NONE)
        {
          if (event.value != 2)
          {
            if (m.keyMapping[event.code].type == BUTTON)
            {
              setSwitch(m.keyMapping[event.code].player, m.keyMapping[event.code].channel, event.value);
            }
            else if (m.keyMapping[event.code].type == SYSTEM)
            {
              setSwitch(0, m.keyMapping[event.code].channel, event.value);
            }
            else if (m.keyMapping[event.code].type == COIN)
            {
              if (event.value)
                incrementCoin();
            }
          }
        }

        break;
      }

      // controlPrintStatus();
    }
  }

  printf("Closing\n");
  close(m.deviceFd);

  return 0;
}

void *wiiThread(void *_args)
{
  /* Device threads run with standard linux prio */
  setRealtimePriority(false);
  struct MappingThreadArguments *args = (struct MappingThreadArguments *)_args;
  char eventPath[4096];
  strcpy(eventPath, args->eventPath);
  free(args);

  Mapping m;

  if ((m.deviceFd = open(eventPath, O_RDONLY)) == -1)
  {
    printf("mapping.c:initDevice(): Failed to open device file descriptor\n");
    exit(-1);
  }
  struct input_event event;

  int flags = fcntl(m.deviceFd, F_GETFL, 0);
  fcntl(m.deviceFd, F_SETFL, flags | O_NONBLOCK);

  int axisIndex;
  uint8_t absoluteBitmask[ABS_MAX / 8 + 1];
  float percentageDeadzone;
  struct input_absinfo absoluteFeatures;

  memset(absoluteBitmask, 0, sizeof(absoluteBitmask));
  if (ioctl(m.deviceFd, EVIOCGBIT(EV_ABS, sizeof(absoluteBitmask)), absoluteBitmask) < 0)
  {
    perror("evdev ioctl");
  }

  fd_set file_descriptor;
  struct timeval tv;

  int x0 = 0;
  int y0 = 0;
  int x1 = 0;
  int y1 = 0;

  while (threadsRunning)
  {
    if (read(m.deviceFd, &event, sizeof event) > 0)
    {

      if (event.type == EV_ABS)
      {
        switch (event.code)
        {
        case 16:
          x0 = event.value;
          break;
        case 17:
          y0 = event.value;
          break;
        case 18:
          x1 = event.value;
          break;
        case 19:
          y1 = event.value;
          break;
        }
      }

      if (x0 != 1023 && x1 != 1023 && y0 != 1023 && y1 != 1023)
      {
        setSwitch(1, 7, 0);
        int middlex = (int)((x0 + x1) / 2.0);
        int middley = (int)((y0 + y1) / 2.0);

        int valuex = middlex;
        int valuey = 1023 - middley;

        unsigned char finalX = (unsigned char)(((double)valuex / 1023) * 255.0);
        unsigned char finalY = (unsigned char)(((double)valuey / 1023) * 255.0);

        //printf("%d,%d.\n", finalX, finalY);

        setAnalogue(0, finalX);
        setAnalogue(1, finalY);
        //printf("%d, %d\n", valuex, valuey);
      }
      else
      {
        setSwitch(1, 7, 1);
      }
    }
  }

  printf("Closing\n");
  close(m.deviceFd);

  return 0;
}