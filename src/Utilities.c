#include "Utilities.h"

/* Sets the configuration of the serial port to low latency mode */
int set_low_latency(int fd)
{
	struct serial_struct serial_settings;

	if (ioctl(fd, TIOCGSERIAL, &serial_settings) < 0)
	{
		printf("Failed to read serial settings for low latency mode\n");
		return 0;
	}

	serial_settings.flags |= ASYNC_LOW_LATENCY;
	if (ioctl(fd, TIOCSSERIAL, &serial_settings) < 0)
	{
		printf("Failed to write serial settings for low latency mode\n");
		return 0;
	}
	return 1;
}

/* Sets the configuration of the serial port */
int set_interface_attribs(int fd, int myBaud)
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
	options.c_cc[VTIME] = 100; // Ten seconds (100 deciseconds)

	tcsetattr(fd, TCSANOW, &options);

	ioctl(fd, TIOCMGET, &status);

	status |= TIOCM_DTR;
	status |= TIOCM_RTS;

	ioctl(fd, TIOCMSET, &status);

	usleep(10000); // 10mS

	return 0;
}
