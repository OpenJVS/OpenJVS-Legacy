#include "Netboot.h"

pthread_t thread_id;
int running = 1;

int initNetboot() {
    return 0;
}

void runNetboot() {
    pthread_create(&thread_id, NULL, netbootThread, NULL);
    printf("Netboot Module Started\n");
}

void *netbootThread(void * arg) {
    char netbootPath[4096];
    strcat(strcpy(netbootPath, romDirectory), mapName);
    netboot(netbootPath, netbootIP);

}

void closeNetboot() {
	running = 0;
	pthread_join(thread_id, NULL);
}

int netboot(char* filename, char* ipAddress)
{
	printf("Netboot: Filename: %s IP: %s\n", filename, ipAddress);
	struct sockaddr_in naomi_address;
	char *recv_buf;
	INT_32 recv_len;
	INT_32 i;

	if(access(filename, F_OK | R_OK))
	{
		printf("Netboot Error: Game not found or not accessible\n");
		return 1;
	}

	memset(&naomi_address, 0x00, sizeof(struct sockaddr_in));
	if(inet_aton(ipAddress, (struct in_addr *) &naomi_address.sin_addr.s_addr) == 0)
	{
        printf("Netboot Error: Failed to translate IP Address\n");
        return 1;
    }
	naomi_address.sin_family = AF_INET;
	naomi_address.sin_port = htons(port);

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
        printf("Netboot Error: Socket Initialization Failed.\n");
		return 1;
    }

	if (connect(socket_fd, (struct sockaddr *) &naomi_address, sizeof(struct sockaddr_in)) != 0) {
        close(socket_fd);
        printf("Netboot Error: Cannot connect to DIMM\n");
        return 1;
      }

	recv_buf = malloc(MAXDATASIZE);

	recv_len = set_mode_host(recv_buf);
	if(recv_len == -1) {
		free(recv_buf);
		return 1;
	}

	printf("Netboot: Connected to DIMM\n");

	free(recv_buf);

	set_security_keycode(0);

  printf("Netboot: Uploading game...");
	upload_file_dimm(filename);
  printf("Netboot: Upload successfull, restarting host.\n");

	restart_host();


}

/*
**	Description: the recv function to receive packet(s) from the net dimm
**
**	Parameters: [*buffer]
**					pointer to allocated memory of size MAXDATASIZE
**
**	Return: Size of received data in bytes
**
*/
INT_32 read_socket(char *recv_buffer)
{
	INT_32 buf_len = recv(socket_fd, recv_buffer, MAXDATASIZE-1, 0);
	if (buf_len == -1)
	{
		printf("Netboot Error: read_socket\n");
		return -1;
	}

	return buf_len;
}

/*
**	Description: Sets the keycode data, in most cases make this 0 for the magic key (whatever that means)
**
**	Parameters: [data]
**					Keycode data
**
*/
void set_security_keycode(UINT_64 data)
{
	struct __attribute__((__packed__)) packet_struct		/* My system at time of programming this is 64-bit aligned so we're packing the struct */
	{
		UINT_32 opcode;
		UINT_64 data;
	} packet;

	packet.opcode = 0x7F000008;
	packet.data = data;

	if (send(socket_fd, &packet, sizeof(struct packet_struct), 0) == -1)
	{
		printf("Netboot Error: sending in set_security_keycode\n");
	}
}

/*
**	Description:
**
**	Parameters: [*recv_buffer]
**					Pointer to allocated memory of MAXDATASIZE
**
**	Return: Size of received data in bytes
**
*/
INT_32 set_mode_host(char *recv_buffer)
{
	struct packet_struct
	{
		UINT_32 opcode;
		UINT_32 data;
	} packet;

	packet.opcode = 0x07000004;
	packet.data = 1;

	if (send(socket_fd, &packet, sizeof(struct packet_struct), 0) == -1)
	{
		printf("Netboot Error: sending in set_mode_host\n");
		return -1;
	}

	return read_socket(recv_buffer);
}

/*
**	Description: Send information about the crc and length to the net dimm
**
**	Parameters: [crc]
**					crc32 of the game data
**				[length]
**					size of the game data
**
*/
void set_information_dimm(UINT_32 crc, UINT_32 length)
{
	struct packet_struct
	{
		UINT_32 opcode;
		UINT_32 crc;
		UINT_32 len;
		UINT_32 packet_data;
	} packet;

	packet.opcode = 0x1900000C;
	packet.crc = crc;
	packet.len = length;
	packet.packet_data = 0;

	if (send(socket_fd, &packet, sizeof(struct packet_struct), 0) == -1)
	{
		printf("Netboot Error: sending in set_information_dimm\n");
	}
}

/*
**	Description: Uploads actual chunk of game data
**
**	Parameters: [addr]
**					Offset to indicate how many are send thus far
**				[*buff]
**					Pointer to chunk of data to be send
**				[mark]
**					No f****** idea
**				[buff_size]
**					Size of the data where *buff points at
**
*/
void upload_dimm(UINT_32 addr, char *buff, INT_32 mark, UINT_32 buff_size)
{
	struct __attribute__((__packed__)) packet_struct 		/* Would've packed it even if it was a 32 bit system */
	{
		UINT_32 opcode_with_info;
		UINT_32 packet_data_1;
		UINT_32 packet_data_2;
		UINT_16 packet_data_3;
		char 	game_data[buff_size];						/* Necessary to put this data after the other packet data */
	} packet;

	packet.opcode_with_info = 0x04800000 | (buff_size + 10) | (mark << 16);
	packet.packet_data_1 = 0;
	packet.packet_data_2 = addr;
	packet.packet_data_3 = 0;
	memcpy(packet.game_data, buff, buff_size);

	if (send(socket_fd, &packet, sizeof(struct packet_struct), 0) == -1)
	{
		printf("Netboot Error: sending in upload_dimm\n");
	}
}

/*
**	Description: Upload file to net dimm and send the appropriate information
**
**	Parameters: [gameFile]
**					Character array of gameFile
**
*/
void upload_file_dimm(char* gameFile)
{
	UINT_32 address = 0;
	UINT_32 crc = 0;
	UINT_32 char_read;
	char buff[BUFFER_SIZE];

	FILE * game_file = fopen(gameFile,"rb");

	while(1)
	{
		char_read = fread(buff, sizeof(char), BUFFER_SIZE, game_file);	/* Take a chunk of size BUFFER_SIZE data from the file gameFile */
		if(!char_read)
			break;
		upload_dimm(address, buff, 0, char_read);						/* Upload the chunk of data in buff to the net dimm */
		crc = crc32(crc, buff, char_read);								/* Keep track of 32 bit CRC */
		address += char_read;											/* Keep track of the characters read from file */
	}
	fclose(game_file);

	crc = ~crc;
	upload_dimm(address, "12345678", 1, 9); 	/* Not quite sure what's going on here. I know it's sending
													'1','2','3','4','5','6','7','8','\0' to the net dimm. Why? no idea. */
	set_information_dimm(crc, address);			/* Send over the 32 bit CRC*/
}

/*
**	Description: Restarts the Naomi/Triforce/Chihiro
**
*/
void restart_host()
{
	UINT_32 packet = 0x0A000000;

	if (send(socket_fd, &packet, sizeof(UINT_32), 0) == -1)
	{
		printf("Netboot Error: sending in restart_host\n");
	}
}

/*
**	Description: To set the time limit. Not sure what time limit we're talking about.
**
**	Parameters: [data]
**					The time to set in 32 bits unsigned int.
**
*/
void set_time_limit(UINT_32 data)
{
	struct packet_struct
	{
		UINT_32 opcode;
		UINT_32 data;
	} packet;

	packet.opcode = 0x17000004;
	packet.data = data;

	if (send(socket_fd, &packet, sizeof(struct packet_struct), 0) == -1)
	{
		printf("Netboot Error: sending in set_time_limit\n");
	}
}

static UINT_32 crc32_tab[] = {
    0x00000000,
    0x77073096,
    0xee0e612c,
    0x990951ba,
    0x076dc419,
    0x706af48f,
    0xe963a535,
    0x9e6495a3,
    0x0edb8832,
    0x79dcb8a4,
    0xe0d5e91e,
    0x97d2d988,
    0x09b64c2b,
    0x7eb17cbd,
    0xe7b82d07,
    0x90bf1d91,
    0x1db71064,
    0x6ab020f2,
    0xf3b97148,
    0x84be41de,
    0x1adad47d,
    0x6ddde4eb,
    0xf4d4b551,
    0x83d385c7,
    0x136c9856,
    0x646ba8c0,
    0xfd62f97a,
    0x8a65c9ec,
    0x14015c4f,
    0x63066cd9,
    0xfa0f3d63,
    0x8d080df5,
    0x3b6e20c8,
    0x4c69105e,
    0xd56041e4,
    0xa2677172,
    0x3c03e4d1,
    0x4b04d447,
    0xd20d85fd,
    0xa50ab56b,
    0x35b5a8fa,
    0x42b2986c,
    0xdbbbc9d6,
    0xacbcf940,
    0x32d86ce3,
    0x45df5c75,
    0xdcd60dcf,
    0xabd13d59,
    0x26d930ac,
    0x51de003a,
    0xc8d75180,
    0xbfd06116,
    0x21b4f4b5,
    0x56b3c423,
    0xcfba9599,
    0xb8bda50f,
    0x2802b89e,
    0x5f058808,
    0xc60cd9b2,
    0xb10be924,
    0x2f6f7c87,
    0x58684c11,
    0xc1611dab,
    0xb6662d3d,
    0x76dc4190,
    0x01db7106,
    0x98d220bc,
    0xefd5102a,
    0x71b18589,
    0x06b6b51f,
    0x9fbfe4a5,
    0xe8b8d433,
    0x7807c9a2,
    0x0f00f934,
    0x9609a88e,
    0xe10e9818,
    0x7f6a0dbb,
    0x086d3d2d,
    0x91646c97,
    0xe6635c01,
    0x6b6b51f4,
    0x1c6c6162,
    0x856530d8,
    0xf262004e,
    0x6c0695ed,
    0x1b01a57b,
    0x8208f4c1,
    0xf50fc457,
    0x65b0d9c6,
    0x12b7e950,
    0x8bbeb8ea,
    0xfcb9887c,
    0x62dd1ddf,
    0x15da2d49,
    0x8cd37cf3,
    0xfbd44c65,
    0x4db26158,
    0x3ab551ce,
    0xa3bc0074,
    0xd4bb30e2,
    0x4adfa541,
    0x3dd895d7,
    0xa4d1c46d,
    0xd3d6f4fb,
    0x4369e96a,
    0x346ed9fc,
    0xad678846,
    0xda60b8d0,
    0x44042d73,
    0x33031de5,
    0xaa0a4c5f,
    0xdd0d7cc9,
    0x5005713c,
    0x270241aa,
    0xbe0b1010,
    0xc90c2086,
    0x5768b525,
    0x206f85b3,
    0xb966d409,
    0xce61e49f,
    0x5edef90e,
    0x29d9c998,
    0xb0d09822,
    0xc7d7a8b4,
    0x59b33d17,
    0x2eb40d81,
    0xb7bd5c3b,
    0xc0ba6cad,
    0xedb88320,
    0x9abfb3b6,
    0x03b6e20c,
    0x74b1d29a,
    0xead54739,
    0x9dd277af,
    0x04db2615,
    0x73dc1683,
    0xe3630b12,
    0x94643b84,
    0x0d6d6a3e,
    0x7a6a5aa8,
    0xe40ecf0b,
    0x9309ff9d,
    0x0a00ae27,
    0x7d079eb1,
    0xf00f9344,
    0x8708a3d2,
    0x1e01f268,
    0x6906c2fe,
    0xf762575d,
    0x806567cb,
    0x196c3671,
    0x6e6b06e7,
    0xfed41b76,
    0x89d32be0,
    0x10da7a5a,
    0x67dd4acc,
    0xf9b9df6f,
    0x8ebeeff9,
    0x17b7be43,
    0x60b08ed5,
    0xd6d6a3e8,
    0xa1d1937e,
    0x38d8c2c4,
    0x4fdff252,
    0xd1bb67f1,
    0xa6bc5767,
    0x3fb506dd,
    0x48b2364b,
    0xd80d2bda,
    0xaf0a1b4c,
    0x36034af6,
    0x41047a60,
    0xdf60efc3,
    0xa867df55,
    0x316e8eef,
    0x4669be79,
    0xcb61b38c,
    0xbc66831a,
    0x256fd2a0,
    0x5268e236,
    0xcc0c7795,
    0xbb0b4703,
    0x220216b9,
    0x5505262f,
    0xc5ba3bbe,
    0xb2bd0b28,
    0x2bb45a92,
    0x5cb36a04,
    0xc2d7ffa7,
    0xb5d0cf31,
    0x2cd99e8b,
    0x5bdeae1d,
    0x9b64c2b0,
    0xec63f226,
    0x756aa39c,
    0x026d930a,
    0x9c0906a9,
    0xeb0e363f,
    0x72076785,
    0x05005713,
    0x95bf4a82,
    0xe2b87a14,
    0x7bb12bae,
    0x0cb61b38,
    0x92d28e9b,
    0xe5d5be0d,
    0x7cdcefb7,
    0x0bdbdf21,
    0x86d3d2d4,
    0xf1d4e242,
    0x68ddb3f8,
    0x1fda836e,
    0x81be16cd,
    0xf6b9265b,
    0x6fb077e1,
    0x18b74777,
    0x88085ae6,
    0xff0f6a70,
    0x66063bca,
    0x11010b5c,
    0x8f659eff,
    0xf862ae69,
    0x616bffd3,
    0x166ccf45,
    0xa00ae278,
    0xd70dd2ee,
    0x4e048354,
    0x3903b3c2,
    0xa7672661,
    0xd06016f7,
    0x4969474d,
    0x3e6e77db,
    0xaed16a4a,
    0xd9d65adc,
    0x40df0b66,
    0x37d83bf0,
    0xa9bcae53,
    0xdebb9ec5,
    0x47b2cf7f,
    0x30b5ffe9,
    0xbdbdf21c,
    0xcabac28a,
    0x53b39330,
    0x24b4a3a6,
    0xbad03605,
    0xcdd70693,
    0x54de5729,
    0x23d967bf,
    0xb3667a2e,
    0xc4614ab8,
    0x5d681b02,
    0x2a6f2b94,
    0xb40bbe37,
    0xc30c8ea1,
    0x5a05df1b,
    0x2d02ef8d
};

UINT_32 crc32(UINT_32 crc, const void * buf, UINT_32 size) {
    const UINT_8 * p;

    p = (UINT_8 * ) buf;
    crc = crc ^ ~0U;

    while (size--)
        crc = crc32_tab[(crc ^ * p++) & 0xFF] ^ (crc >> 8);

    return crc ^ ~0U;
}
