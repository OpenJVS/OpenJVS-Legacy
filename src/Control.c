#include "Control.h"


char players = 2;
char bytesPerPlayer = 2;
char playerSwitches[255];
char systemSwitches = 0x0;
char boardID[255];
char analogueChannels = 12;
char analogue[255];
char rotaryChannels = 12;
char rotary[255];
char coin = 10;
char analogueDefault[255];

unsigned char reverse(unsigned char b) {
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

void initControl() {
	systemSwitches = 0x0;
	for(int i = 0 ; i < players * bytesPerPlayer ; i++) {
		playerSwitches[i] = 0x0;
	}

	for(int i = 0 ; i < analogueChannels ; i++) {
		analogue[i] = analogueDefault[i];
	}

	for(int i = 0 ; i < rotaryChannels ; i++) {
		rotary[i] = 0x05;
	}

}

void setSystemSwitch(int bit, int value) {
	//printf("Info: System Switch switch %d value %d\n", bit, value);
	if(value == 1) {
		systemSwitches |= 1 << bit;
	} else if(value == 0) {
		systemSwitches &= ~(1 << bit);
	}
}

void setPlayerSwitch(int player, int bit, int value) {
	//printf("Info: Player Switch player %d switch %d value %d\n", player, bit, value);
	if(value == 1) {
		playerSwitches[player * bytesPerPlayer + (bit / 8)] |= 1 << (bit - (8 * (bit / 8)));
	} else if(value == 0) {
		playerSwitches[player * bytesPerPlayer + (bit / 8)] &= ~(1 << (bit - (8 * (bit / 8))));
	}
}

void setAnalogue(int channel, char value) {
	//printf("Info: Analogue channel %d value %d", channel, value);
	analogue[channel] = value;
}

int getAnalogue(int channel) {
	return analogue[channel];
}


void setRotary(int channel, char value) {
	rotary[channel] = value;
}

int getRotary(int channel) {
	return rotary[channel];
}

void incrementCoin() {
	coin++;
}
