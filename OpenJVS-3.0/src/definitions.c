#include "definitions.h"

Mode modeStringToEnum(const char *str)
{
  for (int j = 0; j < sizeof(modeConversion) / sizeof(modeConversion[0]); ++j)
    if (!strcmp(str, modeConversion[j].str))
    {
      return modeConversion[j].val;
    }

  printf("mapping.c:modeStringToEnum: no such enum for string (%s)\n", str);
  return 0;
}

const char *modeEnumToString(Mode mode)
{
  for (int j = 0; j < sizeof(modeConversion) / sizeof(modeConversion[0]); ++j)
    if (mode == modeConversion[j].val)
      return modeConversion[j].str;
  printf("mapping.c:modeEnumToString: no such string for enum %d \n", (int)mode);
  return 0;
}

int setRealtimePriority(bool realtime)
{
  int ret = 0;
  int policy = 0;
  int policy_wanted;

  pthread_t this_thread = pthread_self();
  struct sched_param params;

  /* Set the priority to the maximum. */

  if (realtime)
  {
    policy_wanted = SCHED_FIFO;
  }
  else
  {
    policy_wanted = SCHED_OTHER;
  }

  params.sched_priority = sched_get_priority_max(policy_wanted);

  ret = pthread_setschedparam(this_thread, policy_wanted, &params);
  if (ret != 0)
  {
    printf("Unsuccessful in setting thread realtime prio \n");
  }

  if (ret == 0)
  {
    ret = pthread_getschedparam(this_thread, &policy, &params);
    if (ret != 0)
    {
      printf("Couldn't retrieve real-time scheduling paramers \n");
    }
  }

  /* check the schedule policy set */
  if (ret == 0)
  {
    if (policy != policy_wanted)
    {
      printf("Scheduling is %d instead of %d \n", policy, policy_wanted);
    }
  }
  return ret;
}
