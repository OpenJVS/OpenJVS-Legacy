#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

/* JVS IO */

typedef struct
{
  uint8_t * jvs_id_str;
  uint8_t  jvs_cmd_revision;
  uint8_t jvs_standard;
  uint8_t jvs_analog_channels;
  uint16_t jvs_analog_number_bits;
  uint8_t jvs_rotary_channels;
  uint8_t jvs_coin_slots;

} jvs_io_t;

static const jvs_io_t jvs_io_lindbergh =
{
    .jvs_id_str = "SEGA CORPORATION;I/O BD JVS;837-14572;Ver1.00;2005/10",
    .jvs_cmd_revision = 0x13,
    .jvs_standard = 0x30,
    .jvs_analog_channels = 8,
    .jvs_analog_number_bits = 16,
    .jvs_rotary_channels = 8, // is this right?
    .jvs_coin_slots = 2,
};

// todo: @Bobby: I did not check these values but just copied them from the old source - are these verified?
static const jvs_io_t jvs_io_naomi=
{
    .jvs_id_str = "OpenJVS Emulator;I/O BD JVS;837-13551;Ver1.00;98/10",
    .jvs_cmd_revision = 0x11,
    .jvs_standard = 0x30,
    .jvs_analog_channels = 8,
    .jvs_analog_number_bits = 8,
    .jvs_rotary_channels = 8, // is this right?
    .jvs_coin_slots = 2,
};


//static const char OPEN_JVS_ID[] = "SEGA CORPORATION;I/O BD JVS;837-14572;Ver1.00;2005/10";
//static const uint8_t OPEN_JVS_COMMAND_REVISION = 0x13;
//static const uint8_t OPEN_JVS_STANDARD = 0x30;
////static const uint8_t OPEN_JVS_ANALOG_CH_NUMBER_BITS = 8;
//static  uint8_t OPEN_JVS_ANALOG_CH_NUMBER_BITS = 16;

/* Settings for keyboard types */
#define CONFIG_KEY_BIND 0
#define CONFIG_KEY_PLUS 1
#define CONFIG_KEY_MINUS 2

/* This file defines the commands from the JVS spec */
#define CMD_BROADCAST 0xFF
#define CMD_ESCAPE 0xD0
#define CMD_SYNC 0xE0
#define CMD_RESET 0xF0
#define CMD_SETADDRESS 0xF1
#define CMD_SETMETHOD 0xF2
#define CMD_READID 0x10
#define CMD_FORMATVERSION 0x11
#define CMD_JVSVERSION 0x12
#define CMD_COMMSVERSION 0x13
#define CMD_GETFEATURES 0x14
#define CMD_SETMAINBOARDID 0x15
#define CMD_READSWITCHES 0x20
#define CMD_READCOIN 0x21
#define CMD_READANALOG 0x22
#define CMD_READROTARY 0x23
#define CMD_READKEYCODE 0x24
#define CMD_READSCREENPOS 0x25
#define CMD_READGPIO 0x26
#define CMD_WRITEPAYOUTREMAINING 0x2E
#define CMD_RESEND 0x2F
#define CMD_WRITECOINSUBTRACT 0x30
#define CMD_WRITEPAYOUT 0x31
#define CMD_WRITEGPIO1 0x32
#define CMD_WRITEANALOG 0x33
#define CMD_WRITECHAR 0x34
#define CMD_WRITECOINADDED 0x35
#define CMD_WRITEPAYOUTSUBTRACT 0x36
#define CMD_WRITEGPIOBYTE 0x37
#define CMD_WRITEGPIOBIT 0x38
#define BUS_MASTER 0x00


/* JVS status Codes*/
#define JVS_STATUS_NORMAL 0x01
#define JVS_STATUS_COMMAND_UNKNOWN 0x02
#define JVS_STATUS_SUM_ERR 0x03
#define JVS_STATUS_ACK_OVERFLOW 0x04

#define JVS_REPORT_NORMAL 0x1
#define JVS_REPORT_PARAMETER_ERROR 0x02
#define JVS_REPORT_PARAMETER_ERROR2 0x3
#define JVS_REPORT_BUSY 0x4

/* Message Layout:
 * |Sync (1byte) | Node Number (1byte) | Number Bytes Payload (1byte) | Cmd (1byte) | ...(Payload-1) | Checksum (1byte)| */


#define CMD_IDX_SNY 0
#define CMD_IDX_NODE_NUMBER 1
#define CMD_IDX_NUMBER_BYTES_PAYLOAD 2
/* Request: Command byte, Reply: Status byte */
#define CMD_IDX_CMD_STATUS 3
#define CMD_IDX_PAYLOAD 4

/* HEADER_LEN: 3: SYN + Node Number + Number Bytes Payload  (WITH SYN)*/
#define CMD_LEN_HEADER 3
#define CMD_LEN_CHECKSUM 1
#define CMD_LEN_SYNC 1
#define CMD_LEN_NUMBER_BYTES 1
#define CMD_LEN_CMD 1
#define CMD_STATUS 1
#define CMD_LEN_NODE 1

#define GET_MSG_REQ_LEN(packet) (packet[CMD_IDX_NUMBER_BYTES_PAYLOAD] + CMD_LEN_HEADER)

#define GET_MSG_REQ_PAYLOAD_LEN(packet) (packet[CMD_IDX_NUMBER_BYTES_PAYLOAD] - CMD_LEN_CHECKSUM)

/* Only valid in process_cmd() */
#define PROCESS_CMD_IDX_CMD 0
#define PROCESS_CMD_IDX_PAYLOAD 1

/* Buttons */
#define SYSTEM_TEST 7
#define PLAYER_START 7
#define PLAYER_SERVICE 6
#define PLAYER_UP 5
#define PLAYER_DOWN 4
#define PLAYER_LEFT 3
#define PLAYER_RIGHT 2
#define PLAYER_PUSH1 1
#define PLAYER_PUSH2 0
#define PLAYER_PUSH3 7
#define PLAYER_PUSH4 6
#define PLAYER_PUSH5 5
#define PLAYER_PUSH6 4
#define PLAYER_PUSH7 3
#define PLAYER_PUSH8 2

typedef enum
{
        OPEN_JVS_ERR_OK = 0,
        OPEN_JVS_ERR_REC_BUFFER,
        OPEN_JVS_ERR_SERIAL_READ,
        OPEN_JVS_ERR_SERIAL_WRITE,
        OPEN_JVS_ERR_STATE,
        OPEN_JVS_ERR_CHECKSUM,
        OPEN_JVS_ERR_TIMEOUT,
        OPEN_JVS_ERR_NULL,
        OPEN_JVS_ERR_WAIT_BYTES,
        OPEN_JVS_ERR_INVALID_CMD,
        OPEN_JVS_ERR_REPORT,
        OPEN_JVS_ERR_JVS_PROFILE_NULL,
        OPEN_JVS_ERR_ANALOG_MASK,

        OPEN_JVS_ERR_SYNC_BYTE,
        OPEN_JVS_FOUND_HEADER,
        OPEN_JVS_FOUND_MESSAGE,


} open_jvs_status_t;


typedef enum
{
        OPEN_JVS_STATE_WAITING_MESSAGE = 0,
        OPEN_JVS_STATE_WAITING_FOR_HEADER,
        OPEN_JVS_STATE_WAITING_FOR_PAYLOAD,
        OPEN_JVS_STATE_ERROR
} open_jvs_state_t;



#endif // DEFINITIONS_H_
