#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define MAX_STATE_SIZE 16
#define MAX_PACKET_SIZE 1024
#define DEVICE_ID 0x01

#define SYNC 0xE0
#define ESCAPE 0xD0
#define NODE_BROADCAST 0xFF
#define NODE_BUS_MASTER 0x00
#define DEVICE_ADDR_START 0x01

#define STATUS_SUCCESS 0x01
#define STATUS_UNSUPPORTED 0x02      // an unsupported command was sent
#define STATUS_CHECKSUM_FAILURE 0x03 // the checksum on the command packet did not match a computed checksum
#define STATUS_OVERFLOW 0x04         // an overflow occurred while processing the command
#define REPORT_SUCCESS 0x01          // all went well
#define REPORT_PARAMETER_ERROR1 0x02 // TODO: work out difference between this one and the next
#define REPORT_PARAMETER_ERROR2 0x03
#define REPORT_BUSY 0x04            // some attached hardware was busy, causing the request to fail
#define CMD_RESET 0xF0              // reset bus
#define CMD_RESET_ARG 0xD9          // fixed argument to reset command
#define CMD_ASSIGN_ADDR 0xF1        // assign address to slave
#define CMD_SET_COMMS_MODE 0xF2     // switch communications mode for devices that support it, for compatibility
#define CMD_REQUEST_ID 0x10         // requests an ID string from a device
#define CMD_COMMAND_VERSION 0x11    // gets command format version as two BCD digits packed in a byte
#define CMD_JVS_VERSION 0x12        // gets JVS version as two BCD digits packed in a byte
#define CMD_COMMS_VERSION 0x13      // gets communications version as two BCD digits packed in a byte
#define CMD_CAPABILITIES 0x14       // gets a special capability structure from the device
#define CMD_CONVEY_ID 0x15          // convey ID of main board to device
#define CMD_READ_SWITCHES 0x20      // read switch inputs
#define CMD_READ_COINS 0x21         // read coin inputs
#define CMD_READ_ANALOGS 0x22       // read analog inputs
#define CMD_READ_ROTARY 0x23        // read rotary encoder inputs
#define CMD_READ_KEYPAD 0x24        // read keypad inputs
#define CMD_READ_LIGHTGUN 0x25      // read light gun inputs
#define CMD_READ_GPI 0x26           // read general-purpose inputs
#define CMD_RETRANSMIT 0x2F         // ask device to retransmit data
#define CMD_DECREASE_COINS 0x30     // decrease number of coins
#define CMD_WRITE_GPO 0x32          // write to general-purpose outputs
#define CMD_WRITE_ANALOG 0x33       // write to analog outputs
#define CMD_WRITE_DISPLAY 0x34      // write to an alphanumeric display
#define CMD_WRITE_GPO_2 0x37        // write to general purpose outputs 2
#define CMD_MANUFACTURER_START 0x60 // start of manufacturer-specific commands
#define CMD_MANUFACTURER_END 0x7F   // end of manufacturer-specific commands
#define CAP_END 0x00                // end of structure
#define CAP_PLAYERS 0x01            // player/switch info
#define CAP_COINS 0x02              // coin slot info
#define CAP_ANALOG_IN 0x03          // analog info
#define CAP_ROTARY 0x04             // rotary encoder info
#define CAP_KEYPAD 0x05             // keypad info
#define CAP_LIGHTGUN 0x06           // light gun info
#define CAP_GPI 0x07                // general purpose input info
#define CAP_CARD 0x10               // card system info
#define CAP_HOPPER 0x11             //token hopper info
#define CAP_GPO 0x12                // general purpose output info
#define CAP_ANALOG_OUT 0x13         // analog output info
#define CAP_DISPLAY 0x14            // character display info
#define CAP_BACKUP 0x15             // backup memory?
#define ENCODINGS [ "unknown", "ascii numeric", "ascii alphanumeric", "alphanumeric/katakana", "alphanumeric/SHIFT-JIS" ]
#define INIT_DELAY 1.0 // delay after a bus reset to wait for devices to initialize
#define CMD_DELAY 0.01 // delay between commands
#endif                 // CONSTANTS_H_

/* Message Layout:
 * |Sync (1byte) | Node Number (1byte) | Number Bytes Payload (1byte) | Cmd (1byte) | ...(Payload-1) | Checksum (1byte)| */
#define CMD_IDX_SNY 0
#define CMD_IDX_NODE_NUMBER 1
#define CMD_IDX_NUMBER_BYTES_PAYLOAD 2
/* Request: Command byte, Reply: Status byte */
#define CMD_IDX_CMD_STATUS 3
#define CMD_IDX_PAYLOAD 4

/* HEADER_LEN: 3: SYN + Node Number + Number Bytes Payload  (WITH SYN)*/
#define CMD_LEN_HEADER (CMD_LEN_SYNC + CMD_LEN_NODE + CMD_LEN_NUMBER_BYTES)
#define CMD_LEN_CHECKSUM 1
#define CMD_LEN_SYNC 1
#define CMD_LEN_NUMBER_BYTES 1
#define CMD_LEN_CMD 1
#define CMD_STATUS 1
#define CMD_LEN_NODE 1

#define GET_MSG_REQ_LEN(packet) (packet[CMD_IDX_NUMBER_BYTES_PAYLOAD] + (uint32_t)CMD_LEN_HEADER)

/* Timeout in sec after the receive buffer will be flushed and Sync-line set to not addressed */
#define TIMEOUT_INTER_BYTE 20
