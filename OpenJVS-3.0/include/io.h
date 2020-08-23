#ifndef IO_H_
#define IO_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "definitions.h"
#include "constants.h"

typedef struct
{
    uint8_t inputSwitch[MAX_STATE_SIZE][MAX_STATE_SIZE];
    uint16_t coinCount;
    uint16_t analogueChannel[MAX_STATE_SIZE];
    uint16_t rotaryChannel[MAX_STATE_SIZE];
} JVSState;

typedef struct
{
    char name[101];
    uint8_t players;
    uint8_t switches;
    uint8_t coins;
    uint8_t analogueInChannels;
    uint8_t analogueInBits;
    uint8_t rotaryChannels;
    uint8_t keypad;
    uint8_t gunChannels;
    uint8_t gunXBits;
    uint8_t gunYBits;
    uint8_t generalPurposeInputs;
    uint8_t card;
    uint8_t hopper;
    uint8_t generalPurposeOutputs;
    uint8_t analogueOutChannels;
    uint8_t displayOutRows;
    uint8_t displayOutColumns;
    uint8_t displayOutEncodings;
    uint8_t backup;

    uint8_t jvsCommandVersion;
    uint8_t jvsVersion;
    uint8_t jvsComVersion;

    uint16_t analogueMax;

} JVSCapabilities;

static JVSCapabilities SegaType3IO =
    {
        .name = "SEGA CORPORATION;I/O BD JVS;837-14572;Ver1.00;2005/10",
        .players = 2,
        .switches = 0xE,
        .jvsCommandVersion = 0x13,
        .jvsVersion = 0x20,
        .jvsComVersion = 0x10,
        .analogueInChannels = 8,
        .analogueInBits = 10,
        .rotaryChannels = 8,
        .coins = 2,
        .generalPurposeOutputs = 20,
};

static JVSCapabilities NamcoJYUPCB =
    {
        .name = "namco ltd.;JYU-PCB;Ver1.00;JPN,2Coins 2Guns",
        .players = 2,
        .switches = 12,
        .jvsCommandVersion = 0x11,
        .jvsVersion = 0x20,
        .jvsComVersion = 0x10,
        .coins = 2,
        .generalPurposeOutputs = 16,
        .gunChannels = 2,
        .gunXBits = 16,
        .gunYBits = 16,

};



JVSStatus initIO(JVSCapabilities *capabilitiesSetup);
int setSwitch(int player, int switchNumber, int value);
int incrementCoin();
int setAnalogue(int channel, double value);
int setRotary(int channel, double value);
JVSState *getState();
JVSCapabilities *getCapabilities();

#endif // DEVICE_H_
