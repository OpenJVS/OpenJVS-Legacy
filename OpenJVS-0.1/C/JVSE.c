#include "JVSE.h"

int main(void) {

    init();
    serial = open(portName, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
    if (serial < 0) {
        printf("JVSEC: Failed to connect\n");
        return -1;
    }
    set_interface_attribs(serial, B115200, 0);
    struct timeval st, et;

    while (1) {
        gettimeofday(&st,NULL);
        getPacket();
        usleep(10);
        gettimeofday(&et,NULL);
        int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);
        printf("Full cycle time: %d micro seconds\n",elapsed);
    }
    return 0;
}

void init() {
	systemSwitches = 0x0;
	for(int i = 0 ; i < players * bytesPerPlayer ; i++) {
		playerSwitches[i] = 0x0;
	}

	for(int i = 0 ; i < analogueChannels ; i++) {
		analogue[i] = 0x0;
	}
}

void setSystemSwitch(int bit, int value) {
	if(value == 1) {
		systemSwitches |= 1 << bit;
	} else if(value == 0) {
		systemSwitches &= ~(1 << bit);
	}
}

void setPlayerSwitch(int player, int bit, int value) {
	if(value == 1) {
		playerSwitches[player * bytesPerPlayer + (bit / 8)] |= 1 << bit;
	} else if(value == 0) {
		playerSwitches[player * bytesPerPlayer + (bit / 8)] &= ~(1 << bit);
	}
}

void setAnalogue(int channel, char value) {
	analogue[channel] = value;
}

void incrementCoin() {
	coin++;
}

int set_interface_attribs(int fd, int speed, int parity) {
    struct termios tty;
    memset( & tty, 0, sizeof tty);
    if (tcgetattr(fd, & tty) != 0) {
        printf("JVSEC: error %d from tcgetattr", errno);
        return -1;
    }

    cfsetospeed( & tty, speed);
    cfsetispeed( & tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, & tty) != 0) {
        printf("JVSEC: Error %d from tcsetattr", errno);
        return -1;
    }
    return 0;
}

void writeEscaped(unsigned char byte) {
    /* Write the byte to the serial buffer adding appropriate escape bytes */
    if (byte == CMD_SYNC || byte == CMD_ESCAPE) {
        unsigned char buffer[] = {
            CMD_ESCAPE
        };
        int n = write(serial, buffer, sizeof(buffer));
        usleep(100);
        if (n != 1) {
            printf("Error from write: %d, %d\n", n, errno);
        }
        byte -= 1;
    }

    unsigned char buffer[] = {
        byte
    };
    int n = write(serial, buffer, sizeof(buffer));
    usleep(100);
    if (n != 1) {
        printf("JVSEC: Error from write: %d, %d\n", n, errno);
    }
    tcdrain(serial);
}

unsigned char getByte() {
    unsigned char buffer[] = {
        0x00
    };
    int n = -1;
    usleep(100);
    while (n < 1) {
        n = read(serial, buffer, 1);
    }

    if (buffer[0] == CMD_ESCAPE) {
        n = -1;
        usleep(100);
        while (n < 1) {
            n = read(serial, buffer, 1);
        }
        return buffer[0] + 1;
    }
    return buffer[0];
}

void writeByte(unsigned char byte) {
    reply[replyCount] = byte;
    replyCount++;
}

void writeString(char * string) {
    for (int i = 0; i < strlen(string); i++) {
        reply[replyCount] = string[i];
        replyCount++;
    }
}

void writeBytes(unsigned char bytes[], int size) {
    for (int i = 0; i < size; i++) {
        reply[replyCount] = bytes[i];
        replyCount++;
    }
}

void sendReply() {
    if (replyCount > 0) {
        int checksum = BUS_MASTER + replyCount + 2 + STATUS_SUCCESS;

				/* Write out the sync packet */
        unsigned char buffer[] = {
            CMD_SYNC
        };
        write(serial, buffer, 1);
        usleep(100);

        writeEscaped(BUS_MASTER);
        writeEscaped(replyCount + 2);
        writeEscaped(STATUS_SUCCESS);

        int count = 0;
        while (count < replyCount) {
            checksum += reply[count];
            writeEscaped(reply[count]);
            count++;
        }

        writeEscaped(checksum & 0xFF);

        replyCount = 0;
    }

    tcflush(serial, TCIOFLUSH);
    tcdrain(serial);
}

void processPacket(unsigned char packet[], int packet_length, int packet_address) {
    if (packet_address == CMD_BROADCAST || packet_address == deviceID || allDeviceMode) {
        while (packet_length > 0) {
            int command_size = 1;

            if (packet[0] == CMD_RESET) {
                printf("CMD_RESET\n");
                command_size = 2;
                deviceID = -1;
            } else if (packet[0] == CMD_SETADDRESS) {
                printf("CMD_SETADDRESS\n");
                command_size = 2;
                deviceID = packet[1];
                writeByte(STATUS_SUCCESS);
            } else if (packet[0] == CMD_READID) {
                printf("CMD_READID\n");
                writeByte(STATUS_SUCCESS);
                writeString("OpenJVS Emulator;I/O BD JVS;837-13551;Ver1.00;98/10");
            } else if (packet[0] == CMD_FORMATVERSION) {
                printf("CMD_FORMATVERSION\n");
                writeByte(STATUS_SUCCESS);
                writeByte(0x11);
            } else if (packet[0] == CMD_JVSVERSION) {
                printf("CMD_JVSVERSION\n");
                writeByte(STATUS_SUCCESS);
                writeByte(0x20);
            } else if (packet[0] == CMD_COMMSVERSION) {
                printf("CMD_COMMSVERSION\n");
                writeByte(STATUS_SUCCESS);
                writeByte(0x10);
            } else if (packet[0] == CMD_GETFEATURES) {
                printf("CMD_GETFEATURES\n");
                writeByte(STATUS_SUCCESS);
                unsigned char features[] = {
                    0x01, players, bytesPerPlayer * 8, 0x00,
                    0x02, 0x02, 0x00, 0x00,
                    0x03, analogueChannels, 0x08, 0x00,
                    0x06, 0x08, 0x08, 0x02,
                    0x00
                };
                writeBytes(features, sizeof(features));
            } else if (packet[0] == CMD_READSWITCHES) {
                printf("CMD_READSWITCHES\n");
                writeByte(STATUS_SUCCESS);
                writeByte(systemSwitches);
								if(packet[1] != players || packet[2] != bytesPerPlayer) {
									printf("OpenJVS Error: Switch request differs from offered no. of players\n");
								}
								for(int i = 0 ; i < packet[1] * packet[2] ; i++) {
										writeByte(playerSwitches[i]);
								}
                command_size = 3;
            } else if (packet[0] == CMD_WRITEGPIO1) {
                printf("CMD_WRITEGPIO1\n");
                command_size = 2 + packet[1];
                writeByte(STATUS_SUCCESS);
            } else if (packet[0] == CMD_SETMAINBOARDID) {
                printf("CMD_MAINBOARDID\n");
                int counter = 1;
                while (packet[counter] != 0x00 && counter <= packet_length) {
										boardID[counter - 1] = packet[counter];
                    counter++;
                }
								boardID[counter - 1] = 0x00;
                command_size = counter;
                writeByte(STATUS_SUCCESS);
            } else if (packet[0] == CMD_READCOIN) {
                printf("CMD_READCOIN\n");
                writeByte(STATUS_SUCCESS);
                writeByte(0x00);
                writeByte(coin);
                writeByte(0x00);
                writeByte(0x00);
                command_size = 2;
            } else if (packet[0] == CMD_READANALOG) {
                printf("CMD_READANALOG\n");
                command_size = 2;
                writeByte(STATUS_SUCCESS);

								if(packet[1] != analogueChannels) {
									printf("OpenJVS Error: Analogue Channel Requests differs\n");
								}

								for(int i = 0 ; i < packet[1] ; i++) {
									writeByte(0x00);
									writeByte(analogue[i]);
								}
            } else if (packet[0] == CMD_READSCREENPOS) {
                printf("CMD_READSCREENPOS\n");
                command_size = 2;
                writeByte(STATUS_SUCCESS);
                writeByte(0x00);
                writeByte(analogue[0]);
                writeByte(0x00);
                writeByte(analogue[1]);
						} else if (packet[0] == CMD_WRITECOINSUBTRACT) {
							command_size = 4;
							writeByte(STATUS_SUCCESS);
							coin -= packet[3];
            } else {
                printf("CMD_UNKNOWN %02hhX \n", packet[0]);
            }

            packet_length -= command_size;
            packet += command_size;
        }
        usleep(100);
        sendReply();
    }
}

void getPacket() {
    int ourChecksum = 0;

    while (getByte() != CMD_SYNC);

    unsigned char packet_address = getByte();
    ourChecksum += packet_address;

    unsigned char packet_length = getByte() - 1;
    ourChecksum += packet_length + 1;

    unsigned char packet[packet_length];

    int counter = 0;
    while (counter < packet_length) {
        unsigned char byte = getByte();
        packet[counter] = byte;
        counter += 1;
        ourChecksum += byte;
    }

    unsigned char theirChecksum = getByte();
    if ((ourChecksum & 0xFF) == theirChecksum) {
        processPacket(packet, packet_length, packet_address);
    } else {
        printf("JVSE: Error Checksum Problem\n");
    }

}
