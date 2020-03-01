#ifndef CONTROL_H_
#define CONTROL_H_
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>



extern char players;
extern char bytesPerPlayer;
extern uint8_t playerSwitches[255];
extern char systemSwitches;
extern char boardID[255];
extern char analogueChannels;
extern char analogue[255];
extern char rotaryChannels;
extern char rotary[255];
uint16_t coin;
extern char analogueDefault[255];

void initControl(void);
void setSystemSwitch(int bit, int value);
void setPlayerSwitch(int player, int bit, int value);
void setAnalogue(int channel, char value);
void incrementCoin();
unsigned char reverse(unsigned char b);
int getAnalogue(int channel);

#endif // CONTROL_H_
