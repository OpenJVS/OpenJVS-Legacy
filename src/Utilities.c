#include "Utilities.h"
#include "linux/serial.h"

/* Sets the configuration of the serial port */
int set_interface_attribs (int fd, int myBaud)
{
  struct termios options;
  int retval = 0;
  tcgetattr (fd, &options);

  cfmakeraw (&options);
  cfsetispeed (&options, myBaud);
  cfsetospeed (&options, myBaud);

  options.c_cflag |= (CLOCAL | CREAD | CS8);
  options.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_oflag &= ~OPOST;

  options.c_cc[VMIN] = 1;
  options.c_cc[VTIME] = 0;	// Ten seconds (100 deciseconds)

  tcsetattr (fd, TCSANOW, &options);

  struct serial_struct serial_settings;
  if (retval == 0)
  {
    retval = ioctl (fd, TIOCGSERIAL, &serial_settings);
    if (retval < 0)
    {
      printf ("TIOCGSERIAL returend :%d \n", retval);
    }
  }

  if (retval == 0)
  {
    serial_settings.flags |= ASYNC_LOW_LATENCY;
    retval = ioctl (fd, TIOCSSERIAL, &serial_settings);

    if (retval < 0)
    {
      printf ("TIOCSSERIAL returend :%d \n", retval);
    }
  }

  return retval;
}
