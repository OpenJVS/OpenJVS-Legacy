#include "Control.h"
#include "OpenJVS.h"

char players = 2;
char bytesPerPlayer = 2;
uint8_t playerSwitches[255];
char systemSwitches = 0x0;
char boardID[255];
uint16_t analogue[255];
char rotary[255];
uint16_t coin = 10;
uint16_t analogueDefault[255];

uint8_t reverse(uint8_t b)
{
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

open_jvs_status_t initControl()
{
  open_jvs_status_t retval;
  jvs_io_t * jvs_io = NULL;

  retval = jvs_get_io_profile(&jvs_io);

  if(OPEN_JVS_ERR_OK == retval)
  {
    systemSwitches = 0x0;
    for (int i = 0; i < players * bytesPerPlayer; i++)
    {
      playerSwitches[i] = 0x0;
    }

    for (int i = 0; i < jvs_io->jvs_analog_channels; i++)
    {
      analogue[i] = analogueDefault[i];
    }

    for (int i = 0; i < jvs_io->jvs_rotary_channels; i++)
    {
      rotary[i] = 0x05;
    }
  }
  return retval;
}

void setSystemSwitch(int bit, int value)
{
	if (value == 1)
	{
		systemSwitches |= 1 << bit;
	}
	else if (value == 0)
	{
		systemSwitches &= ~(1 << bit);
	}
}

void setPlayerSwitch(int player, int bit, int value)
{
  if (value == 1)
  {
    playerSwitches[player * bytesPerPlayer + (bit / 8)] |= (1 << (bit % 8));
  }
  else if (value == 0)
  {
    playerSwitches[player * bytesPerPlayer + (bit / 8)] &= ~(1 << (bit % 8));
  }
}

void setAnalogue(int channel, uint16_t value)
{
	analogue[channel] = value;
}

uint16_t getAnalogue(int channel)
{
	return analogue[channel];
}

void setRotary(int channel, char value)
{
	rotary[channel] = value;
}

int getRotary(int channel)
{
	return rotary[channel];
}

void incrementCoin()
{
	coin++;
}
