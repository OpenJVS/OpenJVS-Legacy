#include "OpenJVS.h"

//#define OFFLINE_MODE
//#define USE_DEBUG_PIN

static jvs_io_t * jvs_io_profile = NULL;
static uint16_t jvs_analog_mask = 0;
static uint16_t jvs_analog_max = 0;

/* Variables used in the program */
uint16_t deviceID = 0xFFFF;
int serial;

uint8_t response[LINEAR_BUFFER_SIZE];
uint8_t request[LINEAR_BUFFER_SIZE];

/* Use for timeout between received bytes */
time_t time_last_reception;
time_t time_current;

struct timeval tv;
circ_buffer_t read_buffer;

/* Version number of the program */
int majorVersion = 2;
int minorVersion = 1;

#ifndef OFFLINE_MODE
void intHandler (int dummy)
{
  closeKeyboard ();
  closeMouse ();
#ifdef WII_INCLUDE
  closeWii();
#endif
#ifdef FFB_INCLUDE
  closeFFB();
#endif
  closeController ();
  close (serial);
  GPIOUnexport (sync_pin);
  exit (0);
}
#endif

open_jvs_status_t find_start_of_message (void);
open_jvs_status_t decode_escape_circ (uint8_t *out_packet_decoded, uint32_t *out_decoded_length, uint32_t *out_raw_length);
open_jvs_status_t check_message (uint8_t *request, uint32_t request_len);
open_jvs_status_t processPacket (uint8_t *request, uint8_t *resp_packet, uint32_t *out_resp_len);
open_jvs_status_t check_checksum (uint8_t *message);
uint8_t calc_checksum (uint8_t *message, uint8_t len);

//void initJvs(void);

open_jvs_status_t initJvsProfile(jvs_io_t * jvs_profile_p)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  /* Set JVS IO */
  // todo: @Bobby:add an option to the config to choose standard lindbergh, naomi...JVS profiles?
  if(NULL != jvs_profile_p)
  {
    jvs_set_io_profile(jvs_profile_p);
  }
  else
  {
    retval = OPEN_JVS_ERR_JVS_PROFILE_NULL;
  }

  if(OPEN_JVS_ERR_OK == retval)
  {
    jvs_set_analog_max(0);
    jvs_set_analog_mask(0);

    if(jvs_profile_p->jvs_analog_number_bits > (sizeof(jvs_analog_mask) * 8))
    {
      retval = OPEN_JVS_ERR_ANALOG_MASK;
    }

    if(jvs_profile_p->jvs_analog_number_bits > (sizeof(jvs_analog_max) * 8))
    {
      retval = OPEN_JVS_ERR_ANALOG_MASK;
    }

    if(OPEN_JVS_ERR_OK == retval)
    {
      /* Set max value that is supported for the set number of bits set for the analog channel */

      uint16_t max = 0;
      uint16_t mask = 0;

      printf("jvs_analog_number_bits:%d\n", jvs_profile_p->jvs_analog_number_bits);

      for(int16_t i = 0; i < jvs_profile_p->jvs_analog_number_bits; i++)
      {
        max |= (1 << i);
      }

      for(int16_t i = 0; i <= (jvs_profile_p->jvs_analog_number_bits - 1); i++)
      {
        mask |= (1 << (((sizeof(jvs_analog_mask) * 8)-1) -i));
      }

      jvs_set_analog_max(max);
      jvs_set_analog_mask(mask);

    }

    // DEBUG ONLY
    printf("Retval : %d \n", retval);
    printf("jvs_analog_max:%04X \n", jvs_get_analog_max());
    printf("jvs_analog_mask:%04X \n", jvs_get_analog_mask());

  }


  return retval;

}

open_jvs_status_t write_serial (int serial, uint8_t *data, uint32_t data_len)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  int n = write (serial, data, data_len);

  if (n != data_len)
  {
    printf ("Error from write: %d, %d\n", n, errno);

    retval = OPEN_JVS_ERR_OK;
  }
  return retval;
}

open_jvs_status_t read_serial (int serial)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  fd_set fd_serial;
  uint32_t circ_free;

  FD_ZERO(&fd_serial);
  FD_SET(serial, &fd_serial);

  /* set blocking timeout to TIMEOUT_SELECT */
  tv.tv_sec = 0;
  tv.tv_usec = TIMEOUT_SELECT * 1000;

  int asd = select (serial + 1, &fd_serial, NULL, NULL, &tv);

  if (0 == asd)
  {
    retval = OPEN_JVS_ERR_TIMEOUT;
  }
  else if (asd > 0)
  {
    /* Something received ? */
    if (!FD_ISSET(serial, &fd_serial))
    {
      retval = OPEN_JVS_ERR_TIMEOUT;
    }
  }
  else
  {
    retval = OPEN_JVS_ERR_SERIAL_READ;
  }

  /* Space in circ buffer ?*/
  if (OPEN_JVS_ERR_OK == retval)
  {
    if (CIRC_BUFFER_ERR_OK != circ_buffer_available (&read_buffer, &circ_free))
    {
      retval = OPEN_JVS_ERR_REC_BUFFER;
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    uint8_t receive_buffer[LINEAR_BUFFER_SIZE];
    int n;

    /* Get data from serial */
    n = read (serial, receive_buffer, new_min(sizeof(receive_buffer), circ_free));

    if (0 > n)
    {
      retval = OPEN_JVS_ERR_SERIAL_READ;
    }
    else if (0 == n)
    {
      retval = OPEN_JVS_ERR_WAIT_BYTES;
    }
    else
    {
      /* Copy data to circ buffer */
      for (uint32_t i = 0; i < n; i++)
      {
        if (CIRC_BUFFER_ERR_OK != circ_buffer_push (&read_buffer, receive_buffer[i]))
        {
          retval = OPEN_JVS_ERR_REC_BUFFER;
          break;
        }
      }

    }
  }

  return retval;
}

/* DEBUG STUFF*/

#ifdef OFFLINE_MODE

void test_buffer ()
{
  //uint8_t cmd[] = {0xE0, 0xFF, 0x03, 0xF0, 0xD9, 0xCB}; /* Reset ok */
  //uint8_t cmd[] = {0x23, 0x45, 0xFF ,0xE0, 0xFF, 0x03, 0xF0, 0xD9, 0xCB}; /* some bytes + Reset ok */
  //uint8_t cmd[] = {0x23, 0x45, 0xFF ,0xE0, 0xFF, 0x03, 0xF0, 0xD8, 0xCB}; /* some bytes + Reset with crc */
  //uint8_t cmd[] = {0x23, 0x45, 0xFF ,0xE0, 0xFF, 0x03, 0xF0, 0xD8}; /* some bytes + Reset with missing crc */

  //uint8_t cmd[] = {0xE0, 0xFF, 0x03, 0xF1, 0x01, 0xF4, 0xFF}; /* CMD_SETADDRESS */

  //uint8_t cmd[] = {0xE0, 0xFF, 0x03, 0xF1, CMD_ESCAPE, 0x01 -1, 0xF4, 0xFF}; /* CMD_SETADDRESS with escape injected*/
  uint8_t cmd[] = {0xE0, 0x01, 0x02, 0x10, 0x13};  /* CMD_READID */

  //uint8_t cmd[] = {0xE0, 0x01, 0x05, 0x11, 0x12, 0x13, 0x14, 0x50}; /* Multi Request */

  //uint8_t cmd[] = {0xE0, 0x01, 0x0D, 0x20, 0x02, 0x02, 0x22, 0x08, 0x21, 0x02, 0x32, 0x03, 0x00, 0x00, 0x00, 0xB4}; /* Multi Request*/

  //uint8_t cmd[] =  { 0xE0, 0xFF, 0x3, 0xef, 0x10, 0x01 }; /* Test CMD */

  for (uint32_t i = 0; i < sizeof(cmd); i++)
  {
    if (CIRC_BUFFER_ERR_OK != circ_buffer_push (&read_buffer, cmd[i]))
    {
      printf ("circ_buffer_push returned error!");
      exit (-1);
    }
  }
}

#endif

void print_msg (uint8_t *msg, uint32_t len)
{
  printf ("Data:\n");
  for (uint32_t i = 0; i < len; i++)
  {
    printf ("%02X", msg[i]);
  }
  printf ("\n");
}

void print_circ_buffer ()
{

  open_jvs_status_t retval = OPEN_JVS_ERR_OK;
  uint32_t bytes_available = 0;

  if (OPEN_JVS_ERR_OK == retval)
  {
    if (CIRC_BUFFER_ERR_OK != circ_buffer_filled (&read_buffer, &bytes_available))
    {
      retval = OPEN_JVS_ERR_REC_BUFFER;
    }
    printf ("bytes_available:%d \n", bytes_available);
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    uint8_t data;
    /* Find start of message */
    for (uint32_t i = 0; i < bytes_available; i++)
    {
      retval = circ_buffer_peek (&read_buffer, i, &data);

      if (OPEN_JVS_ERR_OK != retval)
      {
        break;
      }

      printf ("%02X", data);
    }
    printf ("\n");
  }

  if (OPEN_JVS_ERR_OK != retval)
  {
    printf ("CircBuffer error: %d \n", retval);
  }

}
/* END DEBUG STUFF */

int main (int argc, char *argv[])
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  /* Setup signal handlers */
#ifndef OFFLINE_MODE
  signal (SIGINT, intHandler);
#endif

  /* Print out information */
  printf ("OpenJVS Emulator %d.%d (Beta)\n", majorVersion, minorVersion);
  printf ("(C) Robert Dilley 2018\n\n");

  // todo: put jvs-io selection unto config?
  retval = initJvsProfile( (jvs_io_t * ) &jvs_io_lindbergh);

#ifndef OFFLINE_MODE
  if (argc > 1)
  {
    strcpy (mapName, argv[1]);
    printf ("Using Map %s\n", mapName);
  }

  if (argc > 2)
  {
    netboot_enable = 0;
  }

#ifdef USE_DEBUG_PIN
  init_debug_pin();
#endif

  initConfig();


  if(OPEN_JVS_ERR_OK != retval)

  if (debug_mode)
  {
    printf ("Info: Debug mode enabled\n");
  }

  if (initKeyboard () == 0)
  {
    runKeyboard ();
  }

  if (initMouse () == 0)
  {
    runMouse ();
  }

  if (OPEN_JVS_ERR_OK == initController ())
  {
    runController ();
  }

#ifdef FFB_INCLUDE
  if(ffb_enable && initFFB() == 0) {
      runFFB();
  }
#endif

#ifdef WII_INCLUDE
  if(initWii() == 0) {
      runWii();
  }
#endif

  if (netboot_enable && initNetboot () == 0)
  {
    runNetboot ();
  }

  /* GPIO SYNC PINS */
  if (GPIOExport (sync_pin) == -1)
  {
    printf ("Warning: Sync pin %d not available\n", sync_pin);
  }

  syncFloat ();

  /* Setup the serial interface here */
  serial = open (portName, O_RDWR | O_NOCTTY | O_NDELAY);

  if (serial < 0)
  {
    printf ("Failed to open RS485 Dongle file descriptor\n");
    return -1;
  }
  set_interface_attribs (serial, B115200);

  /* Init the modules here */
  initControl();

  printf ("OpenJVS Started\n");

  /* Run the system forever */
#endif

  bool timeout_enable = false;
  while (true)
  {
    open_jvs_status_t retval = OPEN_JVS_ERR_OK;
    uint32_t request_len_decoded;
    uint32_t request_len_raw;
    uint32_t response_len = 0;

#ifndef OFFLINE_MODE
    retval = read_serial (serial);
#else
      test_buffer();
#endif

    /* Reset inter-byte timeout */
    if (OPEN_JVS_ERR_OK == retval)
    {
      time (&time_last_reception);
      timeout_enable = true;
    }

    /* Find start sequence in circ buffer*/
    if ((OPEN_JVS_ERR_OK == retval) || (OPEN_JVS_ERR_WAIT_BYTES == retval))
    {
      retval = find_start_of_message ();

      if(debug_mode)
      {
        printf("Received Message: \n");
        print_circ_buffer ();
      }
    }

    /* Remove escape sequence and store in request*/
    if (OPEN_JVS_ERR_OK == retval)
    {
      retval = decode_escape_circ (request, &request_len_decoded, &request_len_raw);

      if(debug_mode)
      {
        printf("After decode_escape_circ: \n");
        print_msg(request, request_len_decoded);
      }
    }

    /* Check length and checksum */
    if (OPEN_JVS_ERR_OK == retval)
    {
      retval = check_message (request, request_len_decoded);

      if(debug_mode)
      {
        printf("check_message: %d\n", retval);
      }
    }
    /* Process data and generate response if message is complete and valid */
    if (OPEN_JVS_ERR_OK == retval)
    {
      retval = processPacket(request, response, &response_len);

      if(debug_mode)
      {
        printf("processPacket:%u\n response_len:%u ", retval,response_len);
        print_msg(response, response_len);
      }
    }

    /* Send response */
    if (OPEN_JVS_ERR_OK == retval)
    {
      if (response_len > 0)
      {
#ifndef OFFLINE_MODE
        retval = write_serial (serial, response, response_len);
#endif
      }
    }

    /* Remove processed request from circ read-buffer */
    if (OPEN_JVS_ERR_OK == retval)
    {
      if (CIRC_BUFFER_ERR_OK != circ_buffer_discard (&read_buffer, request_len_raw))
      {
        retval = OPEN_JVS_ERR_REC_BUFFER;
      }
    }

    /* Check for inter-byte timeout */
    time (&time_current);

    if ((((time_current - time_last_reception) > TIMEOUT_INTER_BYTE) && timeout_enable))
    {
      deviceID = 0xFFFF;
      /* Flush receive buffer and start over */
      circ_buffer_init (&read_buffer);
      syncFloat();

      if (debug_mode)
      {
        printf ("Timeout Reset buffer and address\n");
      }
    }

#ifdef OFFLINE_MODE
      exit(0);
#endif
  }

  return 0;
}

void debug (char *string)
{
  if (debug_mode)
  {
    printf ("%s", string);
  }
}

open_jvs_status_t check_message (uint8_t *request, uint32_t request_len)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  /* Check if we have enough bytes to process packet*/
  if (OPEN_JVS_ERR_OK == retval)
  {
    if ((request_len < CMD_LEN_HEADER) || (request_len < (GET_MSG_REQ_LEN(request))))
    {
      retval = OPEN_JVS_ERR_WAIT_BYTES;
    }
  }

  /* Message received completely */
  if (OPEN_JVS_ERR_OK == retval)
  {
    /* Check checksum */
    retval = check_checksum (request);
  }

  return retval;
}

uint32_t encode_escape (uint8_t *packet, uint32_t length)
{
  uint8_t temp[LINEAR_BUFFER_SIZE];
  uint32_t len_new = length;
  uint32_t i, j;

  for (i = j = 0; i < length; i++, j++)
  {
    if ((i >= LINEAR_BUFFER_SIZE) || (j >= LINEAR_BUFFER_SIZE))
    {
      printf ("Buffer overflow");
      exit (-1);
    }

    if (packet[i] == CMD_SYNC || packet[i] == CMD_ESCAPE)
    {
      /* Insert escape sequence */
      temp[j] = CMD_ESCAPE;
      j++;

      temp[j] = packet[i] - 1;

      len_new++;
    }
    else
    {
      temp[j] = packet[i];
    }
  }

  /* Copy escaped packet */
  if (len_new != length)
  {
    memcpy (packet, temp, len_new);
  }

  return len_new;
}

open_jvs_status_t find_start_of_message (void)
{
  uint32_t bytes_available;

  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  if (OPEN_JVS_ERR_OK == retval)
  {
    if (CIRC_BUFFER_ERR_OK != circ_buffer_filled (&read_buffer, &bytes_available))
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
      if (CIRC_BUFFER_ERR_OK != circ_buffer_peek (&read_buffer, i, &data))
      {
        retval = OPEN_JVS_ERR_REC_BUFFER;
      }

      if (OPEN_JVS_ERR_OK == retval)
      {
        if (CMD_SYNC == data)
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
        if (CIRC_BUFFER_ERR_OK != circ_buffer_discard (&read_buffer, i))
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

open_jvs_status_t decode_escape_circ (uint8_t *out_packet_decoded, uint32_t *out_decoded_length, uint32_t *out_raw_length)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;
  uint32_t i, j = 0;
  bool next_escaped = false;
  uint32_t len_buffer_circ;
  uint32_t len_buffer_decoded;
  uint32_t len_buffer_raw;
  uint8_t byte;

  if (OPEN_JVS_ERR_OK == retval)
  {
    if ((NULL == out_packet_decoded) || (NULL == out_decoded_length) || (NULL == out_raw_length))
    {
      retval = OPEN_JVS_ERR_NULL;
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    if (CIRC_BUFFER_ERR_OK != circ_buffer_filled (&read_buffer, &len_buffer_circ))
    {
      retval = OPEN_JVS_ERR_REC_BUFFER;
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    /* Limit number of bytes copied to linear buffer */
    if (len_buffer_circ > LINEAR_BUFFER_SIZE)
    {
      len_buffer_decoded = len_buffer_circ = LINEAR_BUFFER_SIZE;
    }
    else
    {
      len_buffer_decoded = len_buffer_circ;
    }

    for (i = 0; i < len_buffer_circ; i++)
    {
      if (CIRC_BUFFER_ERR_OK != circ_buffer_peek (&read_buffer, i, &byte))
      {
        retval = OPEN_JVS_ERR_REC_BUFFER;
        break;
      }

      if (byte == CMD_ESCAPE)
      {
        /* Remove escape sequence */
        next_escaped = true;
        len_buffer_decoded--;
      }
      else if (next_escaped)
      {
        out_packet_decoded[j] = byte + 1;
        next_escaped = false;
        j++;
      }
      else
      {
        /* Normal bytes */
        out_packet_decoded[j] = byte;
        j++;
      }
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    *out_raw_length = len_buffer_circ;
    *out_decoded_length = len_buffer_decoded;
  }

  return retval;
}

open_jvs_status_t process_cmd (uint8_t *request, uint32_t request_len_remaining, uint32_t * request_len_processed,
                               uint8_t *resp_packet,uint32_t *response_len_p)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  uint32_t response_payload_len = 0;
  uint32_t request_len_cmd = 0;

  if((NULL == request) || (NULL == request_len_processed) || (NULL == resp_packet) || (NULL == response_len_p))
  {
    retval = OPEN_JVS_ERR_NULL;
  }

  if(OPEN_JVS_ERR_OK == retval)
  {
    switch (request[PROCESS_CMD_IDX_CMD])
    {
      case CMD_RESET:
      {
        debug ("CMD_RESET\n");
        deviceID = 0xFFFF;
        syncFloat ();

        request_len_cmd = CMD_LEN_CMD + 1;
        response_payload_len = 0;
      }
      break;

      case CMD_SETADDRESS:
      {
        debug ("CMD_SETADDRESS\n");

        deviceID = (uint8_t) request[PROCESS_CMD_IDX_PAYLOAD];

        resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
        response_payload_len += 1;

        request_len_cmd = CMD_LEN_CMD + 1;
        syncGround ();

      }
      break;

      case CMD_READID:
      {
        debug ("CMD_READID\n");

        jvs_io_t * jvs_io = NULL;
        retval = jvs_get_io_profile(&jvs_io);

        if(OPEN_JVS_ERR_OK == retval)
        {
          resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
          response_payload_len += 1;

          memcpy (&resp_packet[response_payload_len], jvs_io->jvs_id_str, strlen(jvs_io->jvs_id_str));
          response_payload_len += strlen(jvs_io->jvs_id_str);

          request_len_cmd = CMD_LEN_CMD + 0;
        }
        else
        {
          retval = OPEN_JVS_ERR_JVS_PROFILE_NULL;
        }
      }
      break;

      case CMD_FORMATVERSION:
      {
        debug ("CMD_FORMATVERSION\n");
        jvs_io_t * jvs_io = NULL;
        retval = jvs_get_io_profile(&jvs_io);

        if(OPEN_JVS_ERR_OK == retval)
        {
          resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
          response_payload_len += 1;

          resp_packet[response_payload_len] = jvs_io->jvs_cmd_revision;
          response_payload_len += 1;

          request_len_cmd = CMD_LEN_CMD + 0;
        }
        else
        {
          retval = OPEN_JVS_ERR_JVS_PROFILE_NULL;
        }
      }
      break;

      case CMD_JVSVERSION:
      {
        debug ("CMD_JVSVERSION\n");

        jvs_io_t * jvs_io = NULL;
        retval = jvs_get_io_profile(&jvs_io);

        if(OPEN_JVS_ERR_OK == retval)
        {
          resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
          response_payload_len += 1;

          resp_packet[response_payload_len] = jvs_io->jvs_standard;
          response_payload_len += 1;

          request_len_cmd = CMD_LEN_CMD + 0;
        }
        else
        {
          retval = OPEN_JVS_ERR_JVS_PROFILE_NULL;
        }

      }
      break;

      case CMD_COMMSVERSION:
      {
        debug ("CMD_COMMSVERSION\n");

        resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
        response_payload_len += 1;

        resp_packet[response_payload_len] = 0x10;
        response_payload_len += 1;

        request_len_cmd = CMD_LEN_CMD + 0;
      }
      break;

      case CMD_GETFEATURES:
      {
        jvs_io_t * jvs_io = NULL;
        retval = jvs_get_io_profile(&jvs_io);

        if(OPEN_JVS_ERR_OK == retval)
        {
          uint8_t features[] =
          {   /* Code 0x01: Sw input*/ 0x01, players, bytesPerPlayer * 8, 0x00,
              /* Code 0x02: Number Coin Slots*/ 0x02,  jvs_io->jvs_coin_slots, 0x00, 0x00,
              /* Code 0x03: Number Analog channels, Bits per channel*/ 0x03, jvs_io->jvs_analog_channels, jvs_io->jvs_analog_number_bits, 0x00,
              /* Code 0x04: Number Rotary channels */ 0x04,  jvs_io->jvs_rotary_channels, 0x00, 0x00,
              /* Code 0x07: General purpose SW inputs */ 0x07, 0x00, 0x08, 0x00,
              /* Code 0x13: Analog Output channels */ 0x13, 0x08, 0x00, 0x00,
              /* Code 0x06: Enter screen position ???*/ 0x06, 0x08, 0x08, 0x02,
              /* Code 0x12: General purpose driver */ 0x12,  0x08, 0x00, 0x00,
              /* End */ 0x00 };


          debug ("CMD_GETFEATURES\n");
          resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
          response_payload_len += 1;

          memcpy (&resp_packet[response_payload_len], features, sizeof(features));
          response_payload_len += sizeof(features);

          request_len_cmd = CMD_LEN_CMD + 0;
        }
        else
        {
          retval = OPEN_JVS_ERR_JVS_PROFILE_NULL;
        }
      }
      break;

      case CMD_READSWITCHES:
      {
        debug ("CMD_READSWITCHES\n");

        resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
        response_payload_len += 1;

        resp_packet[response_payload_len] = reverse(systemSwitches);
        response_payload_len += 1;

        for (uint32_t i = 0; i < request[PROCESS_CMD_IDX_PAYLOAD] * request[PROCESS_CMD_IDX_PAYLOAD + 1]; i++)
        {
          resp_packet[response_payload_len] = reverse (playerSwitches[i]);
          response_payload_len += 1;
        }

        if (request[PROCESS_CMD_IDX_PAYLOAD] != players || request[PROCESS_CMD_IDX_PAYLOAD + 1] != bytesPerPlayer)
        {
          printf ("Warning: Switch request differs from offered no. of players\n");
        }

        request_len_cmd = CMD_LEN_CMD + 2;
      }
      break;

      case CMD_WRITEGPIO1:
      {
        debug ("CMD_WRITEGPIO1\n");

        resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
        response_payload_len += 1;

        request_len_cmd = CMD_LEN_CMD + 1 + request[PROCESS_CMD_IDX_PAYLOAD];
      }
      break;

      case CMD_WRITEGPIOBYTE:
      {
        debug ("CMD_WRITEGPIOBYTE\n");

        resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
        response_payload_len += 1;

        request_len_cmd = CMD_LEN_CMD + 2;
      }
      break;

      case CMD_WRITEGPIOBIT:
      {
        debug ("CMD_WRITEGPIOBIT\n");

        resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
        response_payload_len += 1;

        request_len_cmd = CMD_LEN_CMD + 2;
      }
      break;

      case CMD_SETMAINBOARDID:
      {
        debug ("CMD_MAINBOARDID\n");
        size_t str_len;

        str_len = strnlen (&request[PROCESS_CMD_IDX_PAYLOAD], sizeof(boardID));

        memcpy (boardID, &request[PROCESS_CMD_IDX_PAYLOAD], str_len);
        printf ("Main ID written :%s\n", boardID);

        resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
        response_payload_len += 1;

        request_len_cmd = CMD_LEN_CMD + str_len;
      }
      break;

      case CMD_READCOIN:
      {
        debug ("CMD_READCOIN\n");

        resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
        response_payload_len += 1;

        resp_packet[response_payload_len] = 0x00;
        response_payload_len += 1;

        resp_packet[response_payload_len] = coin;
        response_payload_len += 1;

        resp_packet[response_payload_len] = 0x00;
        response_payload_len += 1;

        resp_packet[response_payload_len] = 0x00;
        response_payload_len += 1;

        request_len_cmd = CMD_LEN_CMD + 1;
      }
      break;

      case CMD_READANALOG:
      {
        debug ("CMD_READANALOG\n");

        resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
        response_payload_len += 1;

        for (uint32_t i = 0; i < request[PROCESS_CMD_IDX_PAYLOAD]; i++)
        {
          resp_packet[response_payload_len] = (analogue[i] >> 8 ) & (jvs_analog_mask >> 8);
          response_payload_len += 1;

          resp_packet[response_payload_len] = (analogue[i] >> 0 ) & (jvs_analog_mask >> 0);
          response_payload_len += 1;
        }

        request_len_cmd = CMD_LEN_CMD + 1;
      }
      break;

      case CMD_READROTARY:
      {
        debug ("CMD_READROTARY\n");

        resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
        response_payload_len += 1;

        for (uint32_t i = 0; i < request[PROCESS_CMD_IDX_PAYLOAD]; i++)
        {

#warning "Are the analog data in analogue[] only in 1 byte precision? Is the rest cut off or scaled?"

          resp_packet[response_payload_len] = rotary[i];
          response_payload_len += 1;

          resp_packet[response_payload_len] = 0x00;
          response_payload_len += 1;
        }

        request_len_cmd = CMD_LEN_CMD + 1;
      }
      break;

      case CMD_READSCREENPOS:
      {
        debug ("CMD_READSCREENPOS\n");

        resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
        response_payload_len += 1;

        resp_packet[response_payload_len] = 0x00;
        response_payload_len += 1;

        resp_packet[response_payload_len] = analogue[0];
        response_payload_len += 1;

        resp_packet[response_payload_len] = analogue[1];
        response_payload_len += 1;

        request_len_cmd = CMD_LEN_CMD + 1;
      }
      break;

      case CMD_WRITECOINSUBTRACT:
      {
        coin -= ((uint16_t) request[PROCESS_CMD_IDX_PAYLOAD + 1]) | ((uint16_t) (request[PROCESS_CMD_IDX_PAYLOAD + 1]) << 8);

        resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
        response_payload_len += 1;

        request_len_cmd = CMD_LEN_CMD + 3;
      }
      break;

#ifdef OFFLINE_MODE
      case 0xEF:
      {
        debug("Test CMD\n");

        resp_packet[response_payload_len] = JVS_REPORT_NORMAL;
        response_payload_len += 1;

        /* Insert value that needs to be escaped */
        resp_packet[response_payload_len] = CMD_ESCAPE;
        response_payload_len += 1;

        resp_packet[response_payload_len] = CMD_SYNC;
        response_payload_len += 1;
      }
      break;

#endif
      default:
      {
        retval = OPEN_JVS_ERR_INVALID_CMD;
        request_len_cmd = CMD_LEN_CMD;
        printf ("CMD_UNKNOWN %02hhX\n", request[PROCESS_CMD_IDX_CMD]);
      }
      break;
    }

    *request_len_processed = request_len_cmd;
    *response_len_p = response_payload_len;

  }

  return retval;
}

open_jvs_status_t processPacket (uint8_t *request, uint8_t *resp_packet, uint32_t *out_resp_len)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;
  open_jvs_state_t report_ok = OPEN_JVS_ERR_OK;
  uint8_t packet_address;
  uint32_t response_payload_len = 0;
  uint32_t request_payload_len = GET_MSG_REQ_PAYLOAD_LEN(request);

  uint32_t request_idx = CMD_IDX_CMD_STATUS;


  if ((NULL == request) || (NULL == resp_packet) || (NULL == out_resp_len))
  {
    retval = OPEN_JVS_ERR_NULL;
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    packet_address = request[CMD_IDX_NODE_NUMBER];
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    if ((packet_address == CMD_BROADCAST) || (packet_address == deviceID) || allDeviceMode)
    {

      while(request_idx < (CMD_IDX_CMD_STATUS + request_payload_len))
      {
        uint32_t request_processed;
        uint32_t temp_response_len;

        retval = process_cmd(
            &request[request_idx],
            (CMD_IDX_CMD_STATUS + request_payload_len) - request_idx,
            &request_processed,
            &resp_packet[CMD_IDX_PAYLOAD + response_payload_len],
            &temp_response_len);

        request_idx+= request_processed;
        response_payload_len += temp_response_len;

        if(OPEN_JVS_ERR_OK != retval)
        {
          printf("Invalid command:%x or negative report retval:%d \n", request[CMD_IDX_PAYLOAD + request_idx], retval);

          if((OPEN_JVS_ERR_REPORT == retval) || (OPEN_JVS_ERR_INVALID_CMD == retval))
          {
            report_ok = retval;

            /* Continue with the processing so that the error report can be sent */
            retval = OPEN_JVS_ERR_OK;
          }
        }

        if(OPEN_JVS_ERR_OK != retval)
        {
          break;
        }
      }

      /* Reply to send? */
      if (response_payload_len > 0)
      {
        uint32_t response_len = response_payload_len + CMD_STATUS + CMD_LEN_CHECKSUM;
        uint32_t response_len_raw = response_len + CMD_LEN_NUMBER_BYTES + CMD_LEN_NODE + CMD_LEN_SYNC;

        resp_packet[CMD_IDX_SNY] = CMD_SYNC;
        resp_packet[CMD_IDX_NODE_NUMBER] = BUS_MASTER;
        resp_packet[CMD_IDX_NUMBER_BYTES_PAYLOAD] = response_len;

        if(OPEN_JVS_ERR_OK == report_ok)
        {
          resp_packet[CMD_IDX_CMD_STATUS] = JVS_STATUS_NORMAL;
        }
        else
        {
          if( OPEN_JVS_ERR_INVALID_CMD == report_ok)
          {
            resp_packet[CMD_IDX_CMD_STATUS] = JVS_STATUS_COMMAND_UNKNOWN;
          }
          else
          {
            /* Set some other error - currently these are not actually used*/
            resp_packet[CMD_IDX_CMD_STATUS] = JVS_STATUS_ACK_OVERFLOW;
          }
        }

        /* Calculate checksum */
        resp_packet[response_len_raw - 1] = calc_checksum(&(response[CMD_IDX_NODE_NUMBER]), response_len + CMD_LEN_NODE);

        /* Encode escape sequence */
        *out_resp_len = encode_escape (&(response[CMD_IDX_NODE_NUMBER]), response_len_raw - CMD_LEN_SYNC) + CMD_LEN_SYNC;

      }
    }
  }

  return retval;
}

uint8_t calc_checksum (uint8_t *message, uint8_t len)
{
  uint8_t checksum_calc = 0;

  for (uint8_t i = 0; i < len; i++)
  {
    checksum_calc += message[i];
  }

  return checksum_calc;
}

open_jvs_status_t check_checksum (uint8_t *message)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  uint8_t message_len = GET_MSG_REQ_LEN(message);

  if (calc_checksum (message + CMD_LEN_SYNC, message_len - CMD_LEN_SYNC - CMD_LEN_CHECKSUM) != message[message_len - 1])
  {
    retval = OPEN_JVS_ERR_CHECKSUM;
  }

  return retval;
}

void syncFloat ()
{
  if ((GPIODirection (sync_pin, OUT) == -1) || (GPIOWrite (sync_pin, 0) == -1))
  {
    if (debug_mode)
      printf ("Warning: Failed to float sync pin %d\n", sync_pin);
  }
}

void syncGround ()
{
  if ((GPIODirection (sync_pin, OUT) == -1) || (GPIOWrite (sync_pin, 1) == -1))
  {
    if (debug_mode)
      printf ("Warning: Failed to sink sync pin %d\n", sync_pin);
  }
}

uint16_t jvs_get_analog_mask(void)
{
  return jvs_analog_mask;
}

void jvs_set_analog_mask(uint16_t mask)
{
  jvs_analog_mask = mask;
}

uint16_t jvs_get_analog_max(void)
{
  return jvs_analog_max;
}

void jvs_set_analog_max(uint16_t max)
{
  jvs_analog_max = max;
}

open_jvs_status_t jvs_get_io_profile(jvs_io_t ** jvs_io_p)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  if(NULL != jvs_io_profile)
  {
    *jvs_io_p = jvs_io_profile;
  }
  else
  {
    retval = OPEN_JVS_ERR_JVS_PROFILE_NULL;
  }

  return retval;
}

void jvs_set_io_profile(jvs_io_t * jvs_profile_p)
{
  jvs_io_profile = jvs_profile_p;
}

