#include "openjvs.h"

#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include "buffer.h"
#include "jvs.h"
#include "input.h"

int main(int argc, char **argv)
{
  signal(SIGINT, handleSignal);

  printf("OpenJVS (Version %s.%s.%s)\n\n", PROJECT_VER_MAJOR, PROJECT_VER_MINOR, PROJECT_VER_PATCH);

  /* Get the config */
  if (processConfig(DEFAULT_GLOBAL_CONFIG_PATH, (argc > 1) ? argv[1] : NULL) != OPEN_JVS_ERR_OK)
  {
    printf("Warning: Could not read the config, using default config instead.\n");
  }

  /* Setup the inputs on the computer */
  if (!initInput())
  {
    printf("Error: Inputs could not be setup properly\n");
    return 1;
  }

  /* Init the IO */
  JVSCapabilities *capabilities;
  switch (getConfig()->defaultIO)
  {
  case 0:
    capabilities = &SegaType3IO;
    printf("Output: Sega Type 3 IO Board\n");
    break;

  case 1:
    capabilities = &NamcoJYUPCB;
    printf("Output: Namco JYU IO Board\n");
    break;
  }
  initIO(capabilities);

  /* Setup the JVS Emulator with the RS485 path and capabilities */
  JVSStatus initJVSStatus = initJVS(getConfig()->devicePath);
  if (initJVSStatus != OPEN_JVS_ERR_OK)
  {
    printf("Error: Failed to initialise JVS. Error code %d\n", initJVSStatus);
    return EXIT_FAILURE;
  }

  /* Try to increase prio of JVS communication thread */
  if (setRealtimePriority(true) != 0)
  {
    printf("Warning: Failed to set realtime priority\n");
  }

  /* Process packets forever */

  int running = 1;
  JVSStatus status;
  while (running)
  {
    status = jvs_do();

#ifdef OFFLINE_MODE
    // Give time for debug prints of task started later
    sleep(1);
    status = OPEN_JVS_ERR_OFFLINE;
    //return 0;
#endif

    switch (status)
    {
    /* Status that are normal */
    case OPEN_JVS_ERR_OK:
    case OPEN_JVS_ERR_REC_BUFFER:
    case OPEN_JVS_ERR_TIMEOUT:
    case OPEN_JVS_ERR_SYNC_BYTE:
    case OPEN_JVS_NO_RESPONSE:
    case OPEN_JVS_ERR_WAIT_BYTES:
      break;
    default:
      printf("Warning: The JVS programed returned the error %d\n", status);
    }
  }

  /* Close the file pointer */
  if (!disconnectJVS())
  {
    printf("Error: Couldn't disconnect from serial\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void handleSignal(int signal)
{
  if (signal == 2)
  {
    printf("Warning: Shutting down\n");
    exit(EXIT_SUCCESS);
  }
}
