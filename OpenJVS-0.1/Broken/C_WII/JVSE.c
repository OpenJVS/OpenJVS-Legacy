#include "JVSE.h"

int main(void) {
	cwiid_wiimote_t *wiimote;		/* wiimote handle */
	struct cwiid_state state;		/* wiimote state */
	bdaddr_t bdaddr = *BDADDR_ANY;;		/* bluetooth device address */

	unsigned char mesg = 0;
	unsigned char led_state = 0;
	unsigned char rpt_mode = 0;
	unsigned char rumble = 0;

	int exit = 0;

	printf("Put Wiimote in discoverable mode now (press 1+2)...\n");
	while (!(wiimote = cwiid_open(&bdaddr, 0))) {
		fprintf(stderr, "Unable to connect to wiimote, try again\n");
	}

	toggle_bit(rpt_mode, CWIID_RPT_STATUS);
	toggle_bit(rpt_mode, CWIID_RPT_IR);
	toggle_bit(rpt_mode, CWIID_RPT_BTN);

	if (cwiid_set_rpt_mode(wiimote, rpt_mode)) {
		fprintf(stderr, "Error setting report mode\n");
	}

	printf("Report Mode Toggled\n");

	serial = open(portName, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
	if (serial < 0) {
		printf ("JVSEC: Failed to connect\n");
		return -1;
	}
	set_interface_attribs (serial, B115200, 0);

	printf("Started JVSE\n");


	while(1) {
		cwiid_get_state(wiimote, &state);
		//print_state(&state);
		getPacket();
		usleep(8);
	}

	if (cwiid_close(wiimote)) {
		fprintf(stderr, "Error on wiimote disconnect\n");
		return -1;
	}

	return 0;
}

int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf("JVSEC: error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
        tty.c_iflag &= ~IGNBRK;
        tty.c_lflag = 0;
        tty.c_oflag = 0;
        tty.c_cc[VMIN]  = 0;
        tty.c_cc[VTIME] = 5;
        tty.c_iflag &= ~(IXON | IXOFF | IXANY);
        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_cflag &= ~(PARENB | PARODD);
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0) {
                printf("JVSEC: Error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void writeEscaped(unsigned char byte) {
	/* Write the byte to the serial buffer adding appropriate escape bytes */
	if(byte == CMD_SYNC || byte == CMD_ESCAPE) {
		unsigned char buffer[] = {CMD_ESCAPE};
		int n = write(serial, buffer, sizeof(buffer));
		usleep (100);
		if (n != 1) {
        printf("Error from write: %d, %d\n", n, errno);
    }
		byte -= 1;
	}

	unsigned char buffer[] = {byte};
	int n = write(serial, buffer, sizeof(buffer));
	usleep(100);
	if (n != 1) {
			printf("JVSEC: Error from write: %d, %d\n", n, errno);
	}
	tcdrain(serial);
}

unsigned char getByte() {
	unsigned char buffer[] = {0x00};
	int n = -1;
	while(n < 1) {
		usleep(100);
		n = read(serial, buffer, 1);
	}

	if (buffer[0] == CMD_ESCAPE) {
		n = -1;
		while(n < 1) {
			usleep(100);
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

void writeString(char* string) {
	for(int i = 0 ; i < strlen(string) ; i++) {
		reply[replyCount] = string[i];
		replyCount++;
	}
}

void writeBytes(unsigned char bytes[], int size) {
	for(int i = 0 ; i < size ; i++) {
		reply[replyCount] = bytes[i];
		replyCount++;
	}
}

void sendReply() {
	if(replyCount > 0) {
		int checksum = BUS_MASTER + replyCount + 2 + STATUS_SUCCESS;

		unsigned char buffer[] = {CMD_SYNC};
		write(serial, buffer, 1);
		usleep(2);

		writeEscaped(BUS_MASTER);
		writeEscaped(replyCount + 2);
		writeEscaped(STATUS_SUCCESS);

		int count = 0;
		while(count < replyCount) {
			checksum += reply[count];
			writeEscaped(reply[count]);
			count++;
		}

		writeEscaped(checksum & 0xFF);

		replyCount = 0;
	}

  tcflush(serial, TCIOFLUSH);
}

void processPacket(unsigned char packet[], int packet_length, int packet_address) {
	if(packet_address == CMD_BROADCAST || packet_address == deviceID) {
		while(packet_length > 0) {
			int command_size = 1;

			if(packet[0] == CMD_RESET) {
				printf("CMD_RESET\n");
				command_size = 2;
				deviceID = -1;
			}
			else if(packet[0] == CMD_SETADDRESS) {
				printf("CMD_SETADDRESS\n");
				command_size = 2;
				deviceID = packet[1];
				writeByte(STATUS_SUCCESS);
			}
			else if(packet[0] == CMD_READID) {
				printf("CMD_READID\n");
				writeByte(STATUS_SUCCESS);
				writeString("DILLEY ENTERPRISESLTD.;I/O BD JVS;837-13551;Ver1.00;98/10");
			}
			else if(packet[0] == CMD_FORMATVERSION) {
				printf("CMD_FORMATVERSION\n");
				writeByte(STATUS_SUCCESS);
				writeByte(0x11);
			}
			else if(packet[0] == CMD_JVSVERSION) {
				printf("CMD_JVSVERSION\n");
				writeByte(STATUS_SUCCESS);
				writeByte(0x20);
			}
			else if(packet[0] == CMD_COMMSVERSION) {
				printf("CMD_COMMSVERSION\n");
				writeByte(STATUS_SUCCESS);
				writeByte(0x10);
			}
			else if(packet[0] == CMD_GETFEATURES) {
				printf("CMD_GETFEATURES\n");
				writeByte(STATUS_SUCCESS);
				unsigned char features[] = { 0x01, 0x02, 0x0B, 0x00,
																		 0x02, 0x02, 0x00, 0x00,
																		 0x03, 0x04, 0x08, 0x00,
																		 0x06, 0x08, 0x08, 0x02,
																		 0x00
																	 };
				writeBytes(features, sizeof(features));
			}
			else if(packet[0] == CMD_READSWITCHES) {
				printf("CMD_READSWITCHES\n");
				writeByte(STATUS_SUCCESS);
				writeByte(0x00);
				writeByte(0x00);
				writeByte(0x00);
				writeByte(0x00);
				writeByte(0x00);
				command_size = 3;
			}
			else if(packet[0] == CMD_WRITEGPIO1) {
				printf("CMD_WRITEGPIO1\n");
				command_size = 2 + packet[1];
				writeByte(STATUS_SUCCESS);
			}
			else if(packet[0] == CMD_SETMAINBOARDID) {
				printf("CMD_MAINBOARDID\n");
				printf("ID:");
				int counter = 1;
				while(packet[counter] != 0x00 && counter <= packet_length) {
					printf("%02hhX:", packet[counter]);
					counter++;
				}
				command_size = counter;
				printf("\n");
				writeByte(STATUS_SUCCESS);
			}
			else if(packet[0] == CMD_READCOIN) {
				printf("CMD_READCOIN\n");
				writeByte(STATUS_SUCCESS);
				writeByte(0x00);
				writeByte(0x00);
				writeByte(0x00);
				writeByte(0x00);
				command_size = 2;
			}
			else if(packet[0] == CMD_READANALOG) {
				printf("CMD_READANALOG\n");
				command_size = 2;
				writeByte(STATUS_SUCCESS);
				writeByte(mot_x & 0xFF);writeByte(0x00);writeByte(mot_y & 0xFF);writeByte(0x00);
				writeByte(mot_x & 0xFF);writeByte(0x00);writeByte(mot_y & 0xFF);writeByte(0x00);
			}
			else if(packet[0] == CMD_READSCREENPOS) {
				printf("CMD_READSCREENPOS\n");
				command_size = 2;
				writeByte(STATUS_SUCCESS);
				writeByte(0x00);
				writeByte(mot_x & 0xFF);
				writeByte(0x00);
				writeByte(mot_y & 0xFF);
			}
			else {
				printf("CMD_UNKNOWN %02hhX \n", packet[0]);
			}

			packet_length -= command_size;
			packet += command_size;
		}
		sendReply();
	}
}

void getPacket() {
	int ourChecksum = 0;

	while(getByte() != CMD_SYNC);

	unsigned char packet_address = getByte();
	ourChecksum += packet_address;

	unsigned char packet_length = getByte() - 1;
	ourChecksum += packet_length + 1;

	unsigned char packet[packet_length];

	int counter = 0;
	while(counter < packet_length) {
		unsigned char byte = getByte();
		packet[counter] = byte;
		counter += 1;
		ourChecksum += byte;
	}

	unsigned char theirChecksum = getByte();
	if((ourChecksum & 0xFF) == theirChecksum) {
		processPacket(packet, packet_length, packet_address);
	} else {
		printf("JVSE: Error Checksum Problem\n");
	}

}

void print_state(struct cwiid_state *state) {
	float tmpx = 0;
	float tmpy = 0;
	float count = 0;
	for (int i = 0; i < CWIID_IR_SRC_COUNT; i++) {
		if (state->ir_src[i].valid) {
			//printf("(%d,%d) ", state->ir_src[i].pos[CWIID_X], state->ir_src[i].pos[CWIID_Y]);
			count++;
			tmpx += state->ir_src[i].pos[CWIID_X];
			tmpy += state->ir_src[i].pos[CWIID_Y];
		}

	}
	float n_x = ((tmpx / count) / 1000) * 255;
	float n_y = ((tmpy / count) / 1000) * 255;
	mot_x = 255 - (int)n_x;
	mot_y = (int)n_y;
	printf("%d, %d\n", mot_x, mot_y);
}
