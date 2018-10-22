#include "JVSE.h"

void intHandler(int dummy) {
  closeKeyboard();
  close(serial);
  exit(0);
}

int main(void) {
    /* Setup signal handlers */
    signal(SIGINT, intHandler);

    /* Print out information */
    printf("JVSE: OpenJVS Emulator %d.%d\n", majorVersion, minorVersion);
    printf("JVSE: (C) Robert Dilley 2018\n\n");

    /* Setup the serial interface here */
    serial = open(portName, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
    
    if (serial < 0) {
        printf("JVSE: Failed to connect\n");
        return -1;
    }
    set_interface_attribs(serial, B115200);

    /* Init the modules here */
    initControl();
    initConfig();

    /* Setup some example keys */
    setSystemSwitch(5, 0);
    setSystemSwitch(4, 0);
    setAnalogue(0, 0xFF);
    setPlayerSwitch(0, 1, 0);
    setPlayerSwitch(0, 9, 0);

    /* Run the system forever */
    while (1) {
        getPacket();
    }

    return 0;
}

void debug(char* string) {
	//printf("%s", string);
}

/* Write the byte to the serial buffer adding appropriate escape bytes */
void writeEscaped(unsigned char byte) {
    if (byte == CMD_SYNC || byte == CMD_ESCAPE) {
        unsigned char buffer[] = {
            CMD_ESCAPE
        };
        int n = write(serial, buffer, sizeof(buffer));
        if (n != 1) {
            printf("JVSE: Error from write: %d, %d\n", n, errno);
        }
        byte -= 1;
    }

    unsigned char buffer[] = {
        byte
    };
    int n = write(serial, buffer, sizeof(buffer));
    if (n != 1) {
        printf("JVSE: Error from write: %d, %d\n", n, errno);
    }
    //usleep(10);
    //tcdrain(serial);
}

unsigned char getByte() {
    unsigned char buffer[] = {
        0x00
    };
    int n = -1;
    while (n < 1) {
        //usleep(10);
        n = read(serial, buffer, 1);
    }

    if (buffer[0] == CMD_ESCAPE) {
        n = -1;
        while (n < 1) {
            //usleep(10);
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
    //usleep(100);
    if (replyCount > 0) {
        int checksum = BUS_MASTER + replyCount + 2 + STATUS_SUCCESS;

				/* Write out the sync packet */
        unsigned char buffer[] = {
            CMD_SYNC
        };
        write(serial, buffer, 1);

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

    //tcflush(serial, TCIOFLUSH);
    //tcdrain(serial);
}

void processPacket(unsigned char packet[], int packet_length, int packet_address) {
    if (packet_address == CMD_BROADCAST || packet_address == deviceID || allDeviceMode) {
        while (packet_length > 0) {
            int command_size = 1;

            if (packet[0] == CMD_RESET) {
                debug("CMD_RESET\n");
                command_size = 2;
                deviceID = -1;
            } else if (packet[0] == CMD_SETADDRESS) {
                debug("CMD_SETADDRESS\n");
                command_size = 2;
                deviceID = packet[1];
                writeByte(STATUS_SUCCESS);
            } else if (packet[0] == CMD_READID) {
                debug("CMD_READID\n");
                writeByte(STATUS_SUCCESS);
                writeString("JVSE Emulator;I/O BD JVS;837-13551;Ver1.00;98/10");
            } else if (packet[0] == CMD_FORMATVERSION) {
                debug("CMD_FORMATVERSION\n");
                writeByte(STATUS_SUCCESS);
                writeByte(0x11);
            } else if (packet[0] == CMD_JVSVERSION) {
                debug("CMD_JVSVERSION\n");
                writeByte(STATUS_SUCCESS);
                writeByte(0x20);
            } else if (packet[0] == CMD_COMMSVERSION) {
                debug("CMD_COMMSVERSION\n");
                writeByte(STATUS_SUCCESS);
                writeByte(0x10);
            } else if (packet[0] == CMD_GETFEATURES) {
                debug("CMD_GETFEATURES\n");
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
                debug("CMD_READSWITCHES\n");
                writeByte(STATUS_SUCCESS);
                writeByte(reverse(systemSwitches));
								if(packet[1] != players || packet[2] != bytesPerPlayer) {
									printf("JVSE: Switch request differs from offered no. of players\n");
								}
								for(int i = 0 ; i < packet[1] * packet[2] ; i++) {
										writeByte(reverse(playerSwitches[i]));
								}
                command_size = 3;
            } else if (packet[0] == CMD_WRITEGPIO1) {
                debug("CMD_WRITEGPIO1\n");
                command_size = 2 + packet[1];
                writeByte(STATUS_SUCCESS);
            } else if (packet[0] == CMD_SETMAINBOARDID) {
                debug("CMD_MAINBOARDID\n");
                int counter = 1;
                while (packet[counter] != 0x00 && counter <= packet_length) {
										boardID[counter - 1] = packet[counter];
                    counter++;
                }
								boardID[counter - 1] = 0x00;
                command_size = counter;
                writeByte(STATUS_SUCCESS);
            } else if (packet[0] == CMD_READCOIN) {
                debug("CMD_READCOIN\n");
                writeByte(STATUS_SUCCESS);
                writeByte(0x00);
                writeByte(coin);
                writeByte(0x00);
                writeByte(0x00);
                command_size = 2;
            } else if (packet[0] == CMD_READANALOG) {
                debug("CMD_READANALOG\n");
                command_size = 2;
                writeByte(STATUS_SUCCESS);

								if(packet[1] != analogueChannels) {
									printf("JVSE: Analogue Channel Requests differs\n");
								}

								for(int i = 0 ; i < packet[1] ; i++) {
									writeByte(0x00);
									writeByte(analogue[i]);
								}
            } else if (packet[0] == CMD_READSCREENPOS) {
                debug("CMD_READSCREENPOS\n");
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
        //usleep(100);
        sendReply();
    }
}

void getPacket() {
    int ourChecksum = 0;

    while (getByte() != CMD_SYNC);
    //printf("SYNC\n");
    unsigned char packet_address = getByte();
    ourChecksum += packet_address;

    unsigned char packet_length = getByte() - 1;
    ourChecksum += packet_length + 1;
    //printf("length %d\n", packet_length);
    unsigned char packet[packet_length];

    int counter = 0;
    while (counter < packet_length) {
        unsigned char byte = getByte();
        packet[counter] = byte;
        //printf("byte %d\n", byte);
        counter += 1;
        ourChecksum += byte;
    }

    unsigned char theirChecksum = getByte();
    //printf("check %d %d\n", theirChecksum, (ourChecksum & 0xFF));
    if ((ourChecksum & 0xFF) == theirChecksum) {
        processPacket(packet, packet_length, packet_address);
    } else {
        printf("JVSE: Error Checksum Problem\n");
    }

}
