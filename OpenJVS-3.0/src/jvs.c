#include "jvs.h"

#include "buffer.h"
#include "config.h"
#include "constants.h"
#include "debug.h"
#include "definitions.h"
#include "sense.h"

/* Use for timeout between received bytes */
time_t lastByteTime;
time_t currentByteTime;

int deviceID = -1;

Buffer readBuffer;

JVSPacket packetIn;
JVSPacket packetOut;

// DEBUG STUFF
void print_msg(JVSPacket *msg)
{
  if (getConfig()->debugMode == 2)
  {
    debug(2, "Data:\n");
    for (uint32_t i = 0; i < msg->length; i++)
    {
      debug(2, "%02X", msg->data[i]);
    }
    debug(2, "\n");
  }
}

JVSStatus initJVS(char *devicePath)
{
  JVSStatus retval = OPEN_JVS_ERR_OK;

  initBuffer(&readBuffer);

  /* Set Sync algorithm from settings */
  switch (getConfig()->senseType)
  {
  case 1:
    setSenseCircuit(SENSE_FLOAT);
    initSense();
    break;
  case 2:
    setSenseCircuit(SENSE_SWITCH);
    initSense();
    break;
  }

  /* Init the connection to the JVS-Master */
  initDevice(devicePath);

#ifdef DEBUG_PIN_ENALBED
  DebugPinInit();
#endif

  return retval;
}

int disconnectJVS()
{
  return closeDevice();
}

int writeCapabilities(JVSPacket *outputPacket, JVSCapabilities *capabilities)
{
  outputPacket->data[outputPacket->length] = STATUS_SUCCESS;
  outputPacket->length += 1;

  if (capabilities->players > 0)
  {
    outputPacket->data[outputPacket->length] = CAP_PLAYERS;
    outputPacket->data[outputPacket->length + 1] = capabilities->players;
    outputPacket->data[outputPacket->length + 2] = capabilities->switches;
    outputPacket->data[outputPacket->length + 3] = CAP_END;
    outputPacket->length += 4;
  }

  if (capabilities->coins > 0)
  {
    outputPacket->data[outputPacket->length] = CAP_COINS;
    outputPacket->data[outputPacket->length + 1] = capabilities->coins;
    outputPacket->data[outputPacket->length + 2] = 0x00;
    outputPacket->data[outputPacket->length + 3] = CAP_END;
    outputPacket->length += 4;
  }

  if (capabilities->analogueInChannels > 0)
  {
    outputPacket->data[outputPacket->length] = CAP_ANALOG_IN;
    outputPacket->data[outputPacket->length + 1] = capabilities->analogueInChannels;
    outputPacket->data[outputPacket->length + 2] = capabilities->analogueInBits;
    outputPacket->data[outputPacket->length + 3] = CAP_END;
    outputPacket->length += 4;
  }

  if (capabilities->rotaryChannels > 0)
  {
    outputPacket->data[outputPacket->length] = CAP_ROTARY;
    outputPacket->data[outputPacket->length + 1] = capabilities->rotaryChannels;
    outputPacket->data[outputPacket->length + 2] = 0x00;
    outputPacket->data[outputPacket->length + 3] = CAP_END;
    outputPacket->length += 4;
  }

  if (capabilities->generalPurposeOutputs > 0)
  {
    outputPacket->data[outputPacket->length] = CAP_GPO;
    outputPacket->data[outputPacket->length + 1] = capabilities->generalPurposeOutputs;
    outputPacket->data[outputPacket->length + 2] = 0x00;
    outputPacket->data[outputPacket->length + 3] = CAP_END;
    outputPacket->length += 4;
  }

  if (capabilities->analogueOutChannels > 0)
  {
    outputPacket->data[outputPacket->length] = CAP_ANALOG_OUT;
    outputPacket->data[outputPacket->length + 1] = capabilities->analogueOutChannels;
    outputPacket->data[outputPacket->length + 2] = 0x00;
    outputPacket->data[outputPacket->length + 3] = CAP_END;
    outputPacket->length += 4;
  }

  if (capabilities->gunChannels > 0)
  {
    outputPacket->data[outputPacket->length] = CAP_LIGHTGUN;
    outputPacket->data[outputPacket->length + 1] = capabilities->gunXBits;
    outputPacket->data[outputPacket->length + 2] = capabilities->gunYBits;
    outputPacket->data[outputPacket->length + 3] = capabilities->gunChannels;
    outputPacket->length += 4;
  }

  outputPacket->data[outputPacket->length] = CAP_END;
  outputPacket->length += 1;

  return 1;
}

JVSStatus processPacket(JVSPacket *inPacket, JVSPacket *outPacket)
{
  JVSStatus retval = OPEN_JVS_NO_RESPONSE;

  JVSState *state = getState();
  JVSCapabilities *capabilities = getCapabilities();

  if ((NULL == inPacket) || (NULL == outPacket) || (NULL == state) || (NULL == capabilities))
  {
    debug(2, "arg state:%p capabilities:%p \n", (void *)state, (void *)capabilities);
    retval = OPEN_JVS_ERR_NULL;
  }

  if (OPEN_JVS_NO_RESPONSE == retval)
  {
    uint8_t node_dest = inPacket->data[CMD_IDX_NODE_NUMBER];

    outPacket->length = 0;

    /* Check if we are the destination node */
    if ((node_dest == NODE_BROADCAST) || (node_dest == deviceID) || getConfig()->debugMode == 2 || 1 /* DEBUG ONLY*/)
    {
      outPacket->data[CMD_IDX_NODE_NUMBER] = NODE_BUS_MASTER;

      /* inPacketIndex always points at an CMD byte offset */
      uint32_t inPacketIndex = CMD_IDX_CMD_STATUS;
      uint32_t inPacketPayloadLenght = GET_MSG_REQ_LEN(inPacket->data);

      outPacket->length = CMD_LEN_HEADER + CMD_LEN_CMD;

      while (inPacketIndex < (inPacketPayloadLenght - CMD_LEN_CHECKSUM))
      {
        uint32_t sizeCurrentCmd;

        if (getConfig()->debugMode == 2)
        {
          debug(2, "cmd:%x \n", inPacket->data[inPacketIndex]);
        }

        switch (inPacket->data[inPacketIndex])
        {
        case CMD_RESET:
        {
          debug(1, "CMD_RESET\n");
          deviceID = -1;
          setSensePin(0);

          sizeCurrentCmd = CMD_LEN_CMD + 1;
        }
        break;

        case CMD_ASSIGN_ADDR:
        {
          debug(1, "CMD_ASSIGN_ADDR\n");
          deviceID = (uint8_t)inPacket->data[inPacketIndex + CMD_LEN_CMD];

          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;

          setSensePin(1);

          sizeCurrentCmd = CMD_LEN_CMD + 1;
        }
        break;

        case CMD_REQUEST_ID:
        {
          debug(1, "CMD_REQUEST_ID\n");

          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;

          memcpy(&outPacket->data[outPacket->length], capabilities->name, strlen(capabilities->name));
          outPacket->length += strlen(capabilities->name);

          /* Add null termination */
          outPacket->data[outPacket->length] = 0;
          outPacket->length += 1;

          sizeCurrentCmd = CMD_LEN_CMD + 0;
        }
        break;

        case CMD_COMMAND_VERSION:
        {
          debug(1, "CMD_COMMAND_VERSION\n");

          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;

          outPacket->data[outPacket->length] = capabilities->jvsCommandVersion;
          outPacket->length += 1;

          sizeCurrentCmd = CMD_LEN_CMD + 0;
        }
        break;

        case CMD_JVS_VERSION:
        {
          debug(1, "CMD_JVS_VERSION\n");
          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;
          outPacket->data[outPacket->length] = capabilities->jvsVersion;
          outPacket->length += 1;

          sizeCurrentCmd = CMD_LEN_CMD + 0;
        }
        break;

        case CMD_COMMS_VERSION:
        {
          debug(1, "CMD_COMMS_VERSION\n");

          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;
          outPacket->data[outPacket->length] = capabilities->jvsComVersion;
          outPacket->length += 1;

          sizeCurrentCmd = CMD_LEN_CMD + 0;
        }
        break;

        case CMD_CAPABILITIES:
        {
          debug(1, "CMD_CAPABILITIES\n");
          writeCapabilities(outPacket, capabilities);

          sizeCurrentCmd = CMD_LEN_CMD + 0;
        }
        break;

        case CMD_READ_SWITCHES:
        {
          debug(1, "CMD_READSWITCHES\n");

          uint8_t numberPlayers = inPacket->data[inPacketIndex + CMD_LEN_CMD + 0];
          uint8_t numberBytesPerPlayer = inPacket->data[inPacketIndex + CMD_LEN_CMD + 1];

          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;

          /* System switches */
          outPacket->data[outPacket->length] = (state->inputSwitch[0][0]);
          outPacket->length += 1;

          /* Player switches according to reuest*/
          for (int i = 1; i < (numberPlayers + 1); i++)
          {
            for (int j = 0; j < numberBytesPerPlayer; j++)
            {
              outPacket->data[outPacket->length] = (state->inputSwitch[i][j]);
              outPacket->length += 1;
            }
          }

          sizeCurrentCmd = CMD_LEN_CMD + 2;
        }
        break;

        case CMD_READ_COINS:
        {
          debug(1, "CMD_READ_COINS\n");

          uint8_t numberCoinSlots = inPacket->data[inPacketIndex + CMD_LEN_CMD + 0];
          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;

          for (uint8_t i = 0; i < numberCoinSlots; i++)
          {
            // todo: Status stuff in high 3 bit of first byte (for now set to ok with bitmask)
            // We have shared coins for all players
            outPacket->data[outPacket->length + 0] = (state->coinCount << 8) & 0x1F;
            outPacket->data[outPacket->length + 1] = (state->coinCount << 0) & 0xFF;
            outPacket->length += 2;
          }

          sizeCurrentCmd = CMD_LEN_CMD + 1;
        }
        break;

        case CMD_READ_ANALOGS:
        {
          debug(1, "CMD_READ_ANALOGS\n");

          uint8_t rest_bits;
          uint8_t numberAnalogChannels = inPacket->data[inPacketIndex + CMD_LEN_CMD + 0];

          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;

          rest_bits = 16 - capabilities->analogueInBits;

          for (uint32_t i = 0; i < numberAnalogChannels; i++)
          {

            if (getConfig()->atomiswaveFix)
            {
              /* Data must be 8 bit right aligned for atomiswave maximum speed */
              outPacket->data[outPacket->length + 0] = 0x00;
              outPacket->data[outPacket->length + 1] = state->analogueChannel[i];
            }
            else
            {
              /* Data must be "left aligned" */
              uint16_t analog_data = state->analogueChannel[i] << rest_bits;
              outPacket->data[outPacket->length + 0] = analog_data >> 8;
              outPacket->data[outPacket->length + 1] = analog_data;
            }

            outPacket->length += 2;
          }
          sizeCurrentCmd = CMD_LEN_CMD + 1;
        }
        break;

        case CMD_READ_ROTARY:
        {
          debug(1, "CMD_READ_ROTARY\n");
          uint8_t rest_bits;
          uint8_t numberAnalogChannels = inPacket->data[inPacketIndex + CMD_LEN_CMD + 0];

          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;

          rest_bits = 16 - capabilities->analogueInBits;

          for (uint32_t i = 0; i < numberAnalogChannels; i++)
          {
            uint16_t analog_data = state->rotaryChannel[i] << rest_bits;

            /* Data must be "left aligned" */
            outPacket->data[outPacket->length + 0] = analog_data >> 8;
            outPacket->data[outPacket->length + 1] = analog_data >> 0;

            outPacket->length += 2;
          }
          sizeCurrentCmd = CMD_LEN_CMD + 1;
        }
        break;

        case CMD_DECREASE_COINS:
        {
          //int8_t coin_slot = inPacket->data[inPacketIndex + CMD_LEN_CMD + 0];
          uint16_t coin_decrement = ((uint16_t)(inPacket->data[inPacketIndex + CMD_LEN_CMD + 1]) | ((uint16_t)(inPacket->data[inPacketIndex + CMD_LEN_CMD + 2]) << 8));

          /* Prevent underflow of coins */
          if (coin_decrement > state->coinCount)
          {
            coin_decrement = state->coinCount;
          }
          state->coinCount -= coin_decrement;

          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;

          sizeCurrentCmd = CMD_LEN_CMD + 3;
        }
        break;

        case CMD_WRITE_GPO:
        {
          debug(1, "CMD_WRITE_GPO\n");
          uint8_t numberBytes = inPacket->data[inPacketIndex + CMD_LEN_CMD + 0];
          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;

          sizeCurrentCmd = CMD_LEN_CMD + 1 + numberBytes;
        }
        break;

        case CMD_WRITE_GPO_2:
        {
          debug(1, "CMD_WRITE_GPO2\n");
          uint8_t byte_position = inPacket->data[inPacketIndex + CMD_LEN_CMD + 0];
          uint8_t byte_data = inPacket->data[inPacketIndex + CMD_LEN_CMD + 1];

          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;

          sizeCurrentCmd = CMD_LEN_CMD + 2;
        }
        break;

        case CMD_CONVEY_ID:
        {
          debug(1, "CMD_CONVEY_ID\n");
          int idLength = 0;
          char idData[100];
          for (int i = 0; i < 100; i++)
          {
            idData[i] = (char)inPacket->data[inPacketIndex + CMD_LEN_CMD + i];

            idLength++;

            if (inPacket->data[inPacketIndex + CMD_LEN_CMD + i] == 0x00)
              break;
          }

          debug(1, "ID CONVEYED: %s\n", idData);

          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;

          sizeCurrentCmd = CMD_LEN_CMD + idLength;
        }
        break;

        case CMD_READ_LIGHTGUN:
        {
          debug(1, "CMD_READ_LIGHTGUN\n");

          outPacket->data[outPacket->length] = REPORT_SUCCESS;
          outPacket->length += 1;

          // Write out gun information
          outPacket->data[outPacket->length++] = 0x00;
          outPacket->data[outPacket->length++] = 0xFF - state->analogueChannel[0];
          outPacket->data[outPacket->length++] = 0x00;
          outPacket->data[outPacket->length++] = 0x00 - state->analogueChannel[1];

          sizeCurrentCmd = CMD_LEN_CMD + 1;
        }
        break;

        default:
        {
          retval = OPEN_JVS_ERR_INVALID_CMD;
          debug(0, "Warning: This command is not properly supported [0x%02hhX]\n", inPacket->data[inPacketIndex]);
        }
        break;
        }

        inPacketIndex += sizeCurrentCmd;
      }

      /* Is there a response with payload to send? */
      if (outPacket->length > (CMD_LEN_HEADER + CMD_LEN_CMD))
      {
        /* Append byte for checksum */
        outPacket->length += 1;

        outPacket->data[CMD_IDX_SNY] = SYNC;
        outPacket->data[CMD_IDX_NODE_NUMBER] = NODE_BUS_MASTER;
        outPacket->data[CMD_IDX_NUMBER_BYTES_PAYLOAD] = outPacket->length - (CMD_LEN_NUMBER_BYTES + CMD_LEN_NODE + CMD_LEN_SYNC);

        // todo: Also support other statuses?
        outPacket->data[CMD_IDX_CMD_STATUS] = STATUS_SUCCESS;

        retval = OPEN_JVS_ERR_OK;
      }
    }
  }

  return retval;
}

#ifdef OFFLINE_MODE

void test_buffer()
{
  //uint8_t cmd[] = {0xE0, 0xFF, 0x03, 0xF0, 0xD9, 0xCB}; /* Reset ok */
  //uint8_t cmd[] = {0x23, 0x45, 0xFF ,0xE0, 0xFF, 0x03, 0xF0, 0xD9, 0xCB}; /* some bytes + Reset ok */
  //uint8_t cmd[] = {0x23, 0x45, 0xFF ,0xE0, 0xFF, 0x03, 0xF0, 0xD8, 0xCB}; /* some bytes + Reset with crc */
  //uint8_t cmd[] = {0x23, 0x45, 0xFF ,0xE0, 0xFF, 0x03, 0xF0, 0xD8}; /* some bytes + Reset with missing crc */

  //uint8_t cmd[] = {0xE0, 0xFF, 0x03, 0xF1, 0x01, 0xF4, 0xFF}; /* CMD_SETADDRESS */

  //uint8_t cmd[] = {0xE0, 0xFF, 0x03, 0xF1, CMD_ESCAPE, 0x01 -1, 0xF4, 0xFF}; /* CMD_SETADDRESS with escape injected*/
  //uint8_t cmd[] = {0xE0, 0x01, 0x02, 0x10, 0x13};  /* CMD_READID */

  //uint8_t cmd[] = {0xE0, 0x01, 0x05, 0x11, 0x12, 0x13, 0x14, 0x50}; /* Multi Request */

  /* Multi Request CT*/
  /* 0x20 digital input -> 2 Player * 2 byte = 4byte */
  /* 0x22 analog -> 8 Channels *2 byte: 16 byte*/
  /* 0x23 Rotary -> 8 Channesl * 2 byte = 16 byte*/
  /* 0x21 Coins: -> 2 slots : 2*2 bytes = 4 byte*/

  //uint8_t cmd[] = {0xE0, 0x01, 0x0a, 0x20, 0x02, 0x2, 0x22, 0x8, 0x23, 0x08, 0x21, 0x02, 0xa7};

  //uint8_t cmd[] = {0xE0, 0x01, 0x0a, 0x20, 0x02, 0x2, 0x22, 0x8, 0x23, 0x08, 0x21, 0x02, 0xa7, 0xFF};

  //uint8_t cmd[] = {0xE0, 0x01, 0x0D, 0x20, 0x02, 0x02, 0x22, 0x08, 0x21, 0x02, 0x32, 0x03, 0x00, 0x00, 0x00, 0xB4}; /* Multi Request*/

  //uint8_t cmd[] =  { 0xE0, 0xFF, 0x3, 0xef, 0x10, 0x01 }; /* Test CMD */

  // Maze of Kings
  //uint8_t cmd[] = {0xE0, 0x01, 0x0D, 0x32, 0x01, 0x00, 0x20, 0x02, 0x02, 0x22, 0x08, 0x23, 0x08, 0x21, 0x02, 0xDD};

  // Dream Riders
  uint8_t cmd[] = {0xE0, 0x01, 0x11, 0x21, 0x02, 0x20, 0x02, 0x02, 0x22, 0x08, 0x37, 0x01, 0x00, 0x37, 0x02, 0x00, 0x37, 0x03, 0x00, 0x2E};

  for (uint32_t i = 0; i < sizeof(cmd); i++)
  {
    if (BUFFER_SUCCESS != pushToBuffer(&readBuffer, cmd[i]))
    {
      debug(2, "circ_buffer_push returned error!");
      exit(-1);
    }
  }

  sleep(2);
}

#endif

JVSStatus jvs_do(void)
{
  static bool timeout_enable = false;
  JVSStatus retval = OPEN_JVS_ERR_OK;
  uint32_t request_len_raw;

#ifndef OFFLINE_MODE
  retval = read_serial(/*serial */ &readBuffer);
#else
  static bool once = false;
  if (!once)
  {
    test_buffer();
  }
  once = true;
#endif

  packetOut.length = 0;

  /* Reset inter-byte timeout */
  if (OPEN_JVS_ERR_OK == retval)
  {
    time(&lastByteTime);
    timeout_enable = true;
  }

  /* Find start sequence in circ buffer*/
  if ((OPEN_JVS_ERR_OK == retval) || (OPEN_JVS_ERR_WAIT_BYTES == retval))
  {
    retval = find_start_of_message(&readBuffer);

    if (getConfig()->debugMode == 2)
    {
      debug(1, "Received Message: \n");
      printBuffer(&readBuffer);
    }
  }

  /* Remove escape sequence and store in request*/
  if (OPEN_JVS_ERR_OK == retval)
  {
    retval = decode_escape_circ(&readBuffer, &packetIn, &request_len_raw);

    if (getConfig()->debugMode == 2)
    {
      debug(2, "After decode_escape_circ: \n");
      print_msg(&packetIn);
    }
  }

  /* Check length and checksum */
  if (OPEN_JVS_ERR_OK == retval)
  {
    retval = check_message(&packetIn);

    if (getConfig()->debugMode == 2)
    {
      debug(2, "check_message: %d\n", retval);
    }

    /* Remove processed request from circ read-buffer */
    if ((OPEN_JVS_ERR_OK == retval) || (OPEN_JVS_ERR_CHECKSUM == retval))
    {
      if (BUFFER_SUCCESS != discardFromBuffer(&readBuffer, request_len_raw))
      {
        debug(2, "%d\n", __LINE__);
        retval = OPEN_JVS_ERR_REC_BUFFER;
      }
    }
  }

  /* Process data and generate response if message is complete and valid */
  if (OPEN_JVS_ERR_OK == retval)
  {
    retval = processPacket(&packetIn, &packetOut);

    if (getConfig()->debugMode == 2)
    {
      debug(2, "processPacket:%u response_len:%u ", retval, packetOut.length);
      print_msg(&packetOut);
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    /* Calculate checksum */
    packetOut.data[packetOut.length - 1] = calc_checksum(&packetOut.data[CMD_IDX_NODE_NUMBER], packetOut.length - 1 - CMD_LEN_CHECKSUM);
  }

  /* Encode escape sequence */
  if (OPEN_JVS_ERR_OK == retval)
  {
    retval = encode_escape(&packetOut);

    if (getConfig()->debugMode == 2)
    {
      debug(2, "encode_escape:%u response_len:%u ", retval, packetOut.length);
      print_msg(&packetOut);
    }
  }

  /* Send response */
  if (OPEN_JVS_ERR_OK == retval)
  {

    if (packetOut.length > 0)
    {
#ifndef OFFLINE_MODE
      retval = write_serial(packetOut.data, packetOut.length);
#endif
    }
  }

  /* Check for inter-byte timeout */
  time(&currentByteTime);

  if ((((currentByteTime - lastByteTime) > TIMEOUT_INTER_BYTE) && timeout_enable))
  {
    deviceID = -1;
    /* Flush receive buffer and start over */
    initBuffer(&readBuffer);

    setSensePin(0);
    if (getConfig()->debugMode == 2)
    {
      debug(2, "Timeout Reset buffer and address\n");
    }
  }

  return retval;
}

JVSStatus find_start_of_message(Buffer *read_buffer)
{
  uint32_t bytes_available;

  JVSStatus retval = OPEN_JVS_ERR_OK;

  if (OPEN_JVS_ERR_OK == retval)
  {
    if (BUFFER_SUCCESS != bufferFilled(read_buffer, &bytes_available))
    {
      retval = OPEN_JVS_ERR_REC_BUFFER;
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    uint8_t data;
    uint32_t i;
    bool found_start = false;

    /* Find start of message */
    for (i = 0; i < bytes_available; i++)
    {
      if (BUFFER_SUCCESS != peekFromBuffer(read_buffer, i, &data))
      {
        retval = OPEN_JVS_ERR_REC_BUFFER;
      }

      if (OPEN_JVS_ERR_OK == retval)
      {
        if (SYNC == data)
        {
          found_start = true;
          break;
        }
      }

      if (OPEN_JVS_ERR_OK != retval)
      {
        break;
      }
    }

    if (OPEN_JVS_ERR_OK == retval)
    {

      if (!found_start)
      {
        /* Get rid of all bytes including the current one when there is not CMD_SYNC */
        i += 1;
      }

      /* Discard bytes before Sync */
      if (0 != i)
      {
        if (BUFFER_SUCCESS != discardFromBuffer(read_buffer, i))
        {
          retval = OPEN_JVS_ERR_REC_BUFFER;
        }
      }

      if (OPEN_JVS_ERR_OK == retval)
      {
        if (!found_start)
        {
          retval = OPEN_JVS_ERR_SYNC_BYTE;
        }
      }
    }
  }

  return retval;
}

JVSStatus decode_escape_circ(Buffer *read_buffer, JVSPacket *out_packet, uint32_t *out_raw_length)
{
  JVSStatus retval = OPEN_JVS_ERR_OK;
  uint32_t i, j = 0;
  bool next_escaped = false;
  uint32_t len_buffer_circ;
  uint32_t len_buffer_decoded;
  uint8_t byte;

  if (OPEN_JVS_ERR_OK == retval)
  {
    if ((NULL == out_packet) || (NULL == out_raw_length))
    {
      retval = OPEN_JVS_ERR_NULL;
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    if (BUFFER_SUCCESS != bufferFilled(read_buffer, &len_buffer_circ))
    {
      retval = OPEN_JVS_ERR_REC_BUFFER;
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    /* Limit number of bytes copied to size of linear buffer */
    if (len_buffer_circ > sizeof(out_packet->data))
    {
      len_buffer_decoded = len_buffer_circ = sizeof(out_packet->data);
    }
    else
    {
      len_buffer_decoded = len_buffer_circ;
    }

    for (i = 0; i < len_buffer_circ; i++)
    {
      if (BUFFER_SUCCESS != peekFromBuffer(read_buffer, i, &byte))
      {
        retval = OPEN_JVS_ERR_REC_BUFFER;
        break;
      }

      if (byte == ESCAPE)
      {
        /* Remove escape sequence */
        next_escaped = true;
        len_buffer_decoded--;
      }
      else if (next_escaped)
      {
        out_packet->data[j] = byte + 1;
        next_escaped = false;
        j++;
      }
      else
      {
        /* Normal bytes */
        out_packet->data[j] = byte;
        j++;
      }
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    *out_raw_length = len_buffer_circ;
    out_packet->length = len_buffer_decoded;
  }

  return retval;
}

JVSStatus check_checksum(JVSPacket *packet)
{
  JVSStatus retval = OPEN_JVS_ERR_OK;

  if (NULL == packet)
  {
    retval = OPEN_JVS_ERR_NULL;
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    uint8_t message_len = GET_MSG_REQ_LEN(packet->data);

    if (calc_checksum((packet->data + CMD_LEN_SYNC), message_len - CMD_LEN_SYNC - CMD_LEN_CHECKSUM) != packet->data[GET_MSG_REQ_LEN(packet->data) - 1])
    {
      retval = OPEN_JVS_ERR_CHECKSUM;
    }
  }

  return retval;
}

JVSStatus check_message(JVSPacket *packet)
{
  JVSStatus retval = OPEN_JVS_ERR_OK;

  if (NULL == packet)
  {
    retval = OPEN_JVS_ERR_NULL;
  }

  /* Check if we have enough bytes to process packet*/
  if (OPEN_JVS_ERR_OK == retval)
  {
    if ((packet->length < CMD_LEN_HEADER) || (packet->length < (GET_MSG_REQ_LEN(packet->data))))
    {
      retval = OPEN_JVS_ERR_WAIT_BYTES;
    }
  }

  /* Message received completely */
  if (OPEN_JVS_ERR_OK == retval)
  {
    /* Check checksum */
    retval = check_checksum(packet);
  }

  return retval;
}

uint8_t calc_checksum(uint8_t *message, uint8_t len)
{
  uint8_t checksum_calc = 0;

  for (uint8_t i = 0; i < len; i++)
  {
    checksum_calc += message[i];
  }

  return checksum_calc;
}

JVSStatus encode_escape(JVSPacket *packet)
{
  JVSStatus retval = OPEN_JVS_ERR_OK;

  uint8_t temp[MAX_PACKET_SIZE];
  uint32_t len_new = 0;
  uint32_t i, j;

  if (NULL == packet)
  {
    retval = OPEN_JVS_ERR_NULL;
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    len_new = packet->length;

    /* Copy SYNC*/
    for (i = j = 0; i < CMD_LEN_SYNC; i++, j++)
    {
      temp[j] = packet->data[i];
    }

    for (i = j = CMD_LEN_SYNC; i < packet->length; i++, j++)
    {
      if ((i >= sizeof(packet->data)) || (j >= sizeof(packet->data)))
      {
        retval = OPEN_JVS_ERR_PACKET_BUFFER_OVERFLOW;
        break;
      }

      if (packet->data[i] == SYNC || packet->data[i] == ESCAPE)
      {
        /* Insert escape sequence */
        temp[j] = ESCAPE;
        j++;

        temp[j] = packet->data[i] - 1;

        len_new++;
      }
      else
      {
        temp[j] = packet->data[i];
      }
    }
    /* Copy escaped packet */
    if (len_new != packet->length)
    {
      memcpy(packet->data, temp, len_new);
      packet->length = len_new;
    }
  }
  return retval;
}
