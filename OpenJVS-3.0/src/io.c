#include <string.h>
#include <math.h>

#include "definitions.h"
#include "io.h"
#include "config.h"

JVSState state;
JVSCapabilities capabilities;

JVSStatus initIO(JVSCapabilities *initialCapabilities)
{
  if (initialCapabilities == NULL)
    return OPEN_JVS_ERR_NULL;

  memcpy(&capabilities, initialCapabilities, sizeof(JVSCapabilities));

  // Set the maximum analogue values
  if (capabilities.analogueInBits > 16)
    return OPEN_JVS_ERR_ANALOG_BITS;

  capabilities.analogueMax = pow(2, capabilities.analogueInBits) - 1;

  div_t switchDiv = div(capabilities.switches, 8);
  int switchBytes = switchDiv.quot + (switchDiv.rem ? 1 : 0);
  for (int players = 0; players < (capabilities.players + 1); players++)
  {
    for (int switches = 0; switches < switchBytes; switches++)
    {
      state.inputSwitch[players][switches] = 0x00;
    }
  }

  for (int analogueChannels = 0; analogueChannels < capabilities.analogueInChannels; analogueChannels++)
  {
    state.analogueChannel[analogueChannels] = 0;
  }

  for (int rotaryChannels = 0; rotaryChannels < capabilities.rotaryChannels; rotaryChannels++)
  {
    state.rotaryChannel[rotaryChannels] = 0;
  }

  state.coinCount = 0;

  return OPEN_JVS_ERR_OK;
}

int setSwitch(int player, int switchNumber, int value)
{
  if (player > capabilities.players)
  {
    printf("Error: That player does not exist.\n");
    return 0;
  }

  if (switchNumber >= capabilities.switches)
  {
    printf("Error: That switch does not exist.\n");
    return 0;
  }

  div_t switchDiv = div(switchNumber, 8);
  int switchBytes = switchDiv.quot + (switchDiv.rem ? 1 : 0);

  if (value)
  {
    state.inputSwitch[player][switchDiv.quot] |= 1 << (7 - switchDiv.rem);
  }
  else
  {
    state.inputSwitch[player][switchDiv.quot] &= ~(1 << (7 - switchDiv.rem));
  }

  return 1;
}

int incrementCoin()
{
  state.coinCount++;
  return 1;
}

int setAnalogue(int channel, double value)
{
  if (channel < capabilities.analogueInChannels)
  {
    state.analogueChannel[channel] = (int)((double)value * (double)capabilities.analogueMax);
    return 1;
  }
  return 0;
}

int setRotary(int channel, double value)
{
  if (channel < capabilities.rotaryChannels)
  {
    state.rotaryChannel[channel] = value * 255;
    return 1;
  }

  return 0;
}

JVSState *getState()
{
  return &state;
}

JVSCapabilities *getCapabilities()
{
  return &capabilities;
}