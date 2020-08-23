#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

/* Select timeout in ms*/
#define TIMEOUT_SELECT 500

//#define OFFLINE_MODE

//#define DEBUG_PIN_ENALBED

#define min(x, y) ((x) <= (y)) ? (x) : (y)

typedef enum
{
    OPEN_JVS_ERR_OK = 0,
    /* Errors that indicates something is not working */
    OPEN_JVS_ERR_REC_BUFFER,
    OPEN_JVS_ERR_SERIAL_READ,
    OPEN_JVS_ERR_SERIAL_WRITE,
    OPEN_JVS_ERR_STATE,
    OPEN_JVS_ERR_CHECKSUM,
    OPEN_JVS_ERR_NULL,
    OPEN_JVS_ERR_INVALID_CMD,
    OPEN_JVS_ERR_OFFLINE,
    //       OPEN_JVS_ERR_REPORT,
    OPEN_JVS_ERR_JVS_PROFILE_NULL,
    OPEN_JVS_ERR_ANALOG_BITS,
    OPEN_JVS_ERR_PACKET_BUFFER_OVERFLOW,
    OPEN_JVS_ERR_INVALID_SYNC_CIRCUIT,

    /* Errors/Status that are fine */
    OPEN_JVS_ERR_TIMEOUT,
    OPEN_JVS_ERR_SYNC_BYTE,
    OPEN_JVS_NO_RESPONSE,
    OPEN_JVS_ERR_WAIT_BYTES,

} JVSStatus;

typedef enum
{
    WHEEL,
    ACCELERATOR,
    BREAK,
    ANALOGUE_X,
    ANALOGUE_Y,
    ANALOGUE_Z,
    START,
    SERVICE,
    TEST,
    GEAR_UP,
    GEAR_DOWN,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_0,
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
    BUTTON_5,
    BUTTON_6,
    BUTTON_7,
    BUTTON_8,
    BUTTON_9,
    BUTTON_10,
    BUTTON_11,
    BUTTON_12,
    BUTTON_13,
    BUTTON_14,
    BUTTON_15,
    BUTTON_16,
    SYSTEM_1,
    SYSTEM_2,
    SYSTEM_3,
    SYSTEM_4,
    SYSTEM_5,
    SYSTEM_6,
    SYSTEM_7,
    SYSTEM_8,
    COIN_1,
    COIN_2,
} Mode;

static const struct
{
    Mode val;
    const char *str;
} modeConversion[] = {
    {WHEEL, "WHEEL"},
    {ACCELERATOR, "ACCELERATOR"},
    {BREAK, "BREAK"},
    {ANALOGUE_X, "ANALOGUE_X"},
    {ANALOGUE_Y, "ANALOGUE_Y"},
    {ANALOGUE_Z, "ANALOGUE_Z"},
    {SERVICE, "SERVICE"},
    {TEST, "TEST"},
    {GEAR_UP, "GEAR_UP"},
    {GEAR_DOWN, "GEAR_DOWN"},
    {BUTTON_UP, "BUTTON_UP"},
    {BUTTON_DOWN, "BUTTON_DOWN"},
    {BUTTON_LEFT, "BUTTON_LEFT"},
    {BUTTON_RIGHT, "BUTTON_RIGHT"},
    {BUTTON_0, "BUTTON_0"},
    {BUTTON_1, "BUTTON_1"},
    {BUTTON_2, "BUTTON_2"},
    {BUTTON_3, "BUTTON_3"},
    {BUTTON_4, "BUTTON_4"},
    {BUTTON_5, "BUTTON_5"},
    {BUTTON_6, "BUTTON_6"},
    {BUTTON_7, "BUTTON_7"},
    {BUTTON_8, "BUTTON_8"},
    {BUTTON_9, "BUTTON_9"},
    {BUTTON_10, "BUTTON_10"},
    {BUTTON_11, "BUTTON_11"},
    {BUTTON_12, "BUTTON_12"},
    {BUTTON_13, "BUTTON_13"},
    {BUTTON_14, "BUTTON_14"},
    {BUTTON_15, "BUTTON_15"},
    {BUTTON_16, "BUTTON_16"},
    {SYSTEM_1, "SYSTEM_1"},
    {SYSTEM_2, "SYSTEM_2"},
    {SYSTEM_3, "SYSTEM_3"},
    {SYSTEM_4, "SYSTEM_4"},
    {SYSTEM_5, "SYSTEM_5"},
    {SYSTEM_6, "SYSTEM_6"},
    {SYSTEM_7, "SYSTEM_7"},
    {SYSTEM_8, "SYSTEM_8"},
    {START, "START"},
    {COIN_1, "COIN_1"},
    {COIN_2, "COIN_2"}};

typedef enum
{
    KEY,
    ABS
} InType;

typedef enum
{
    NONE,
    SYSTEM,
    BUTTON,
    ANALOGUE,
    ROTARY,
    GUN,
    COIN,
} OutType;

typedef struct
{
    int channel;
    InType type;
    Mode mode;
    int min;
    int max;
    int reverse;
} MappingIn;

typedef struct
{
    int channel;
    OutType type;
    Mode mode;
    int player;
    int min;
    int max;
    int reverse;
} MappingOut;

Mode modeStringToEnum(const char *str);
const char *modeEnumToString(Mode mode);
int setRealtimePriority(bool realtime);

#endif // DEFINITIONS_H_
