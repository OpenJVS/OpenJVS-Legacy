#include "buffer.h"
#include "device.h"
#include "definitions.h"

int serialIO = -1;

int initDevice(char *devicePath)
{
  if ((serialIO = open(devicePath, O_RDWR | O_NOCTTY | O_SYNC | O_NDELAY)) < 0)
  {
    printf("Failed to open %s\n", devicePath);
    return 0;
  }

  /* Setup the serial connection */
  setSerialAttributes(serialIO, B115200);
  setSerialLowLatency(serialIO);

  usleep(100 * 1000); //required to make flush work, for some reason

  tcflush(serialIO, TCIOFLUSH);

  usleep(100 * 1000); //required to make flush work, for some reason

  return 1;
}

JVSStatus write_serial(/*int serial,*/ uint8_t *data, uint32_t data_len)
{
  JVSStatus retval = OPEN_JVS_ERR_OK;

  int n = write(serialIO, data, data_len);

  if (n != data_len)
  {
    // todo: Return error code or play along when serial buffer on driver level overflows?
    printf("Error from write: retval%d, data to be written:%u  errno%d %s \n", n, data_len, errno, strerror(errno));

    retval = OPEN_JVS_ERR_OK;
  }

  return retval;
}

JVSStatus read_serial(/*int serial ,*/ Buffer *read_buffer)
{
  JVSStatus retval = OPEN_JVS_ERR_OK;

  fd_set fd_serial;
  uint32_t circ_free;
  struct timeval tv;

  int serial = serialIO;

  FD_ZERO(&fd_serial);
  FD_SET(serialIO, &fd_serial);

  /* set blocking timeout to TIMEOUT_SELECT */
  tv.tv_sec = 0;
  tv.tv_usec = TIMEOUT_SELECT * 1000;

  int asd = select(serial + 1, &fd_serial, NULL, NULL, &tv);

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
    if (BUFFER_SUCCESS != bufferAvailable(read_buffer, &circ_free))
    {
      retval = OPEN_JVS_ERR_REC_BUFFER;
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    uint8_t receive_buffer[LINEAR_BUFFER_SIZE];
    int n;

    /* Get data from serial */
    n = read(serial, receive_buffer, min(sizeof(receive_buffer), circ_free));

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
        if (BUFFER_SUCCESS != pushToBuffer(read_buffer, receive_buffer[i]))
        {
          retval = OPEN_JVS_ERR_REC_BUFFER;
          break;
        }
      }
    }
  }

  return retval;
}

int closeDevice()
{
  return close(serialIO);
}

/* Sets the configuration of the serial port */
int setSerialAttributes(int fd, int myBaud)
{
  struct termios options;
  int status;
  tcgetattr(fd, &options);

  cfmakeraw(&options);
  cfsetispeed(&options, myBaud);
  cfsetospeed(&options, myBaud);

  options.c_cflag |= (CLOCAL | CREAD);
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_oflag &= ~OPOST;

  options.c_cc[VMIN] = 0;
  options.c_cc[VTIME] = 0; // Ten seconds (100 deciseconds)

  tcsetattr(fd, TCSANOW, &options);

  ioctl(fd, TIOCMGET, &status);

  status |= TIOCM_DTR;
  status |= TIOCM_RTS;

  ioctl(fd, TIOCMSET, &status);

  usleep(100 * 1000); // 10mS

  return 0;
}

/* Sets the serial port to low latency mode */
int setSerialLowLatency(int fd)
{
  struct serial_struct serial_settings;

  if (ioctl(fd, TIOCGSERIAL, &serial_settings) < 0)
  {
    printf("Error: Failed to read serial settings for low latency mode\n");
    return 0;
  }

  serial_settings.flags |= ASYNC_LOW_LATENCY;
  if (ioctl(fd, TIOCSSERIAL, &serial_settings) < 0)
  {
    printf("Error: Failed to write serial settings for low latency mode\n");
    return 0;
  }
  return 1;
}
