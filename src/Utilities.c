#include "Utilities.h"

/* Sets the configuration of the serial port */
int set_interface_attribs(int fd, int speed) {
    struct termios tty;
    memset( & tty, 0, sizeof tty);
    if (tcgetattr(fd, & tty) != 0) {
        printf("JVSE: Error %d from tcgetattr", errno);
        return -1;
    }

    cfsetospeed( & tty, speed);
    cfsetispeed( & tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= (CS8);

    tty.c_oflag &= ~OPOST;
    tty.c_lflag |= (PENDIN);
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);



    if (tcsetattr(fd, TCSANOW, & tty) != 0) {
        printf("JVSE: Error %d from tcsetattr", errno);
        return -1;
    }
    return 0;
}
