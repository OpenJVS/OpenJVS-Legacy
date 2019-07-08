#include "OpenJVS.h"

void intHandler(int dummy) {
    closeKeyboard();
    closeMouse();
#ifdef WII_INCLUDE
    closeWii();
#endif
#ifdef FFB_INCLUDE
    closeFFB();
#endif
    closeController();
    close(serial);
    GPIOUnexport(sync_pin);
    exit(0);
}

int main( int argc, char* argv[]) {
    /* Setup signal handlers */
    signal(SIGINT, intHandler);

    /* Print out information */
    printf("OpenJVS Emulator %d.%d (Beta)\n", majorVersion, minorVersion);
    printf("(C) Robert Dilley 2018\n\n");


    if(argc > 1) {
        strcpy(mapName, argv[1]);
        printf("Using Map %s\n", mapName);
    }

    if(argc > 2) {
        netboot_enable = 0;
    }

    initConfig();

    if(debug_mode) {
    	printf("Info: Debug mode enabled\n");
    }

    if (initKeyboard() == 0) {
        runKeyboard();
    }

    if (initMouse() == 0) {
        runMouse();
    }

    if(initController() == 0) {
        runController();
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

    if(netboot_enable && initNetboot() == 0) {
	    runNetboot();
    }

	/* GPIO SYNC PINS */
	if (GPIOExport(sync_pin) == -1) {
		printf("Warning: Sync pin %d not available\n", sync_pin);
  }

	syncFloat();

  /* Setup the serial interface here */
  serial = open(portName, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);

  if (serial < 0) {
      printf("Failed to open RS485 Dongle file descriptor\n");
      return -1;
  }
  set_interface_attribs(serial, B115200);

  /* Init the modules here */
  initControl();

  printf("OpenJVS Started\n");

  /* Run the system forever */
  while (1) {
      getPacket();
  }

  return 0;
}

void debug(char * string) {
  if(debug_mode) {
    printf("%s", string);
  }
}

/* Write the byte to the serial buffer adding appropriate escape bytes */
void writeEscaped(unsigned char byte) {
    if (byte == CMD_SYNC || byte == CMD_ESCAPE) {
        unsigned char buffer[] = {
            CMD_ESCAPE
        };
        int n = write(serial, buffer, sizeof(buffer));
        if (n != 1) {
            printf("Error from write: %d, %d\n", n, errno);
        }
        byte -= 1;
    }

    unsigned char buffer[] = {
        byte
    };

    int n = write(serial, buffer, sizeof(buffer));
    if (n != 1) {
        printf("Error from write: %d, %d\n", n, errno);
    }
}

unsigned char getByte() {
    unsigned char buffer[] = {
        0x00
    };
    int n = -1;
    while (n < 1) {
        n = read(serial, buffer, 1);
    }

    if (buffer[0] == CMD_ESCAPE) {
        n = -1;
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

    /* Warning: This may break, we should check. */
    tcflush(serial, TCIOFLUSH);
    tcdrain(serial);
}

void processPacket(unsigned char packet[], int packet_length, int packet_address) {
    if (packet_address == CMD_BROADCAST || packet_address == deviceID || allDeviceMode) {
        while (packet_length > 0) {
            int command_size = 1;
            switch (packet[0]) {
              case CMD_RESET:
                  debug("CMD_RESET\n");
                  command_size = 2;
                  deviceID = -1;
                  syncFloat();
                  break;
              case CMD_SETADDRESS:
                  debug("CMD_SETADDRESS\n");
                  command_size = 2;
                  deviceID = packet[1];
                  writeByte(STATUS_SUCCESS);
                  syncGround();
                  break;
              case CMD_READID:
                  debug("CMD_READID\n");
                  writeByte(STATUS_SUCCESS);
                  writeString("namco ltd.;JYU-PCB;Ver1.00;JPN,2Coins 2Guns");
                  break;
              case CMD_FORMATVERSION:
                  debug("CMD_FORMATVERSION\n");
                  writeByte(STATUS_SUCCESS);
                  writeByte(0x11);
                  break;
              case CMD_JVSVERSION:
                  debug("CMD_JVSVERSION\n");
                  writeByte(STATUS_SUCCESS);
                  writeByte(0x20);
                  break;
              case CMD_COMMSVERSION:
                  debug("CMD_COMMSVERSION\n");
                  writeByte(STATUS_SUCCESS);
                  writeByte(0x10);
                  break;
              case CMD_GETFEATURES:
                  debug("CMD_GETFEATURES\n");
                  writeByte(STATUS_SUCCESS);
                  unsigned char features[] = {
                    0x01,players,12,0x00,
                    0x02,0x02,0x00, 0x00,
		    0x06,16,16,2,
                    0x12, 16, 0x00, 0x00,
                    0x00

		  };
                  writeBytes(features, sizeof(features));
                  break;
              case CMD_READSWITCHES:
                  debug("CMD_READSWITCHES\n");
                  writeByte(STATUS_SUCCESS);
                  writeByte(reverse(systemSwitches));
                  if (packet[1] != players || packet[2] != bytesPerPlayer) {
                      printf("Warning: Switch request differs from offered no. of players\n");
                  }
                  for (int i = 0; i < packet[1] * packet[2]; i++) {
                      writeByte(reverse(playerSwitches[i]));
                  }
                  command_size = 3;
                  break;
              case CMD_WRITEGPIO1:
                  debug("CMD_WRITEGPIO1\n");
                  command_size = 2 + packet[1];
                  writeByte(STATUS_SUCCESS);
                  break;
              case CMD_WRITEGPIOBYTE:
                  debug("CMD_WRITEGPIOBYTE\n");
                  command_size = 3;
                  writeByte(STATUS_SUCCESS);
                  break;
              case CMD_WRITEGPIOBIT:
                  debug("CMD_WRITEGPIOBIT\n");
                  command_size = 3;
                  writeByte(STATUS_SUCCESS);
                  break;
              case CMD_SETMAINBOARDID:
                  debug("CMD_MAINBOARDID\n");
                  int counter = 1;
                  while (packet[counter] != 0x00 && counter <= packet_length) {
                      boardID[counter - 1] = packet[counter];
                      counter++;
                  }
                  boardID[counter - 1] = 0x00;
                  command_size = counter;
                  printf("Main ID written\n");
                  writeByte(STATUS_SUCCESS);
                  break;
              case CMD_READCOIN:
                  debug("CMD_READCOIN\n");
                  writeByte(STATUS_SUCCESS);
                  writeByte(0x00);
                  writeByte(coin);
                  writeByte(0x00);
                  writeByte(0x00);
                  command_size = 2;
                  break;
              case CMD_READANALOG:
                  debug("CMD_READANALOG\n");
                  command_size = 2;
                  writeByte(STATUS_SUCCESS);
                  for (int i = 0; i < packet[1]; i++) {
                      writeByte(analogue[i]);
                      writeByte(0x00);
                  }
                  break;

              case CMD_READGPIO:
                  debug("CMD_READGENERAL\n");
                  command_size = 2;
                  writeByte(STATUS_SUCCESS);
                  for (int i = 0; i < packet[1]; i++) {
                      writeByte(0x00);
                  }
                  break;
              case CMD_READROTARY:
                  debug("CMD_READROTARY\n");
                  command_size = 2;
                  writeByte(STATUS_SUCCESS);
                  for (int i = 0; i < packet[1]; i++) {
                      writeByte(rotary[i]);
                      writeByte(0x00);
                  }
                  break;
              case CMD_READSCREENPOS:
                  debug("CMD_READSCREENPOS\n");
                  command_size = 2;
                  writeByte(STATUS_SUCCESS);
		  writeByte(0xFF);
                  writeByte(0x00);
                  writeByte(0xFF);
                  writeByte(0x00);
                  break;
              case CMD_WRITECOINSUBTRACT:
                  command_size = 4;
                  writeByte(STATUS_SUCCESS);
                  coin -= packet[3];
                  break;
              default:
                  printf("CMD_UNKNOWN %02hhX\n", packet[0]);
            }

            packet_length -= command_size;
            packet += command_size;
        }
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
        printf("Error: Checksum Problem\n");
    }

}

void syncFloat() {
	if (GPIODirection(sync_pin, IN) == -1) {
		if(debug_mode)
			printf("Warning: Failed to float sync pin %d\n", sync_pin);
	} else {
		if(debug_mode)
			printf("Float sync pin\n");
	}
}

void syncGround() {
	if (GPIODirection(sync_pin, OUT) == -1 || GPIOWrite(sync_pin, 0) == -1) {
		if(debug_mode)
	        	printf("Warning: Failed to sink sync pin %d\n", sync_pin);
	} else {
		if(debug_mode)
			printf("Ground sync pin\n");
	}
}
