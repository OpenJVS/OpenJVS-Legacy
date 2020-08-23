import serial
import threading

# Set the USB Address here
USB_UART_ADDRESS = '/dev/tty.SLAB_USBtoUART'

# List of all the JVS standard commands
CMD_BROADCAST = 0xFF
CMD_ESCAPE = 0xD0
CMD_SYNC = 0xE0
CMD_RESET = 0xF0
CMD_SETADDRESS = 0xF1
CMD_SETMETHOD = 0xF2
CMD_READID = 0x10
CMD_FORMATVERSION = 0x11
CMD_JVSVERSION = 0x12
CMD_COMMSVERSION = 0x13
CMD_GETFEATURES = 0x14
CMD_SETMAINBOARDID = 0x15
CMD_READSWITCHES = 0x20
CMD_READCOIN = 0x21
CMD_READANALOG = 0x22
CMD_READROTARY = 0x23
CMD_READKEYCODE = 0x24
CMD_READSCREENPOS = 0x25
CMD_READGPIO = 0x26
CMD_WRITEPAYOUTREMAINING = 0x2E
CMD_RESEND = 0x2F
CMD_WRITECOINSUBTRACT = 0x30
CMD_WRITEPAYOUT = 0x31
CMD_WRITEGPIO1 = 0x32
CMD_WRITEANALOG = 0x33
CMD_WRITECHAR = 0x34
CMD_WRITECOINADDED = 0x35
CMD_WRITEPAYOUTSUBTRACT = 0x36
CMD_WRITEGPIOBYTE = 0x37
CMD_WRITEGPIOBIT = 0x38
BUS_MASTER	= 0x00
STATUS_SUCCESS = 0x01

class JVSE(threading.Thread):
    """A class to provide control of an arcade board via the JVS standard.
    Requires a USB to RS485 converter.

    Attributes:
         serial_path: The path to the RS485 converter device.
    """

    def __init__(self, serial_path = USB_UART_ADDRESS):
        """Return a JVS object relaying JVS commands to the device at *serial_path*."""
        threading.Thread.__init__(self)
        self.serial = serial.Serial(serial_path, 115200, timeout=5)
        self.old_reply = []
        self.device_id = 1
        self.running = False
        self.system_key = [0] * 8
        self.player1_key = [0] * 16
        self.player2_key = [0] * 16
        self.coin = 0
        self.posx = 0x00
        self.posy = 0x00

    def increment_coin(self):
        """Increment the amount of coins reported by the first coin slot."""
        self.coin += 1

    def set_key_player1(self, number, value):
        """Set player 1's switchs defined by *number* to either pressed (1) or unpressed (0) using *value*."""
        self.player1_key[number] = value

    def set_key_player2(self, number, value):
        """Set player 2's switchs defined by *number* to either pressed (1) or unpressed (0) using *value*."""
        self.player2_key[number] = value

    def set_key_system(self, number, value):
        """Set the systems switchs defined by *number* to either pressed (1) or unpressed (0) using *value*."""
        self.system_key[number] = value

    def set_pos(self, x, y):
        """Set player 1's lightgun and analogue position defined by *x* and *y*."""
        self.posx = x
        self.posy = y

    def run(self):
        """Start the JVS connection thread."""
        self.running = True
        while self.running:
            self.get_packet()

    def stop(self):
        """Stop the JVS connection thread."""
        self.running = False

    def bits_to_byte(self, bit_list):
        """Convert a list of bits *bit_list*, to a single byte."""
        byte = 0x0
        mult = 1
        for i in range(len(bit_list) -1, -1, -1):
            if bit_list[i] == 1:
                byte += mult
            mult *= 2
        return byte

    def write_escaped(self, byte):
        """Write the byte *byte* to the serial buffer adding appropriate escape bytes."""
        if byte == CMD_SYNC or byte == CMD_ESCAPE:
            self.serial.write(chr(CMD_ESCAPE))
            byte -= 1
        print "Write: ", byte
        self.serial.write(chr(byte))

    def write_reply(self, reply):
        """Write the byte list *reply* to the serial buffer."""
        if len(reply) > 0:
            checksum = BUS_MASTER + (len(reply) + 2) + STATUS_SUCCESS
            self.serial.write(chr(CMD_SYNC))
            self.write_escaped(BUS_MASTER)
            self.write_escaped(len(reply) + 2)
            self.write_escaped(STATUS_SUCCESS)
            for i in range(0, len(reply)):
                checksum += reply[i]
                self.write_escaped(reply[i])
            self.write_escaped(checksum & 0xFF)
            old_reply = reply
            reply = []
            self.serial.flush()


    def get_byte(self):
        """Get a single byte from the serial buffer, removing any escape bytes."""
        byte = self.serial.read(1)
        if len(byte) == 0:
            return self.get_byte()
        byte = ord(byte)
        if byte == CMD_ESCAPE:
            byte = self.serial.read(1)
            if len(byte) == 0:
                # Maybe should be plus 1?
                return self.get_byte() + 1
            else:
                return ord(byte) + 1
        return byte

    def process_packet(self, packet, packet_address):
        """Process the packet *packet* sent to *packet_address* and send the appropriate reply"""
        print "Process Packet: ", packet
        if packet_address == CMD_BROADCAST or packet_address == self.device_id:
            packet_length = len(packet)
            reply = []
            while packet_length > 0:
                command_size = 1

                if packet[0] == CMD_RESET:
                    print "CMD_RESET"
                    command_size = 2
                    self.device_id = -1

                elif packet[0] == CMD_SETADDRESS:
                    print "CMD_SETADDRESS"
                    command_size = 2
                    self.device_id = packet[1]
                    reply += [STATUS_SUCCESS]

                elif packet[0] == CMD_READID:
                    print "CMD_READID"
                    reply += [STATUS_SUCCESS] + map(ord, "SEGA ENTERPRISESLTD.;I/O BD JVS;837-13551;Ver1.00;98/10") + [0x00]

                elif packet[0] == CMD_FORMATVERSION:
                    print "CMD_FORMATVERSION"
                    reply += [STATUS_SUCCESS, 0x11]

                elif packet[0] == CMD_JVSVERSION:
                    print "CMD_JVSVERSION"
                    reply += [STATUS_SUCCESS, 0x20]

                elif packet[0] == CMD_COMMSVERSION:
                    print "CMD_COMMAVERSION"
                    reply += [STATUS_SUCCESS, 0x10]

                elif packet[0] == CMD_GETFEATURES:
                    print "CMD_GETFEATURES"
                    features = [
                                0x01, 0x02, 0x0B, 0x00,
                                0x02, 0x02, 0x00, 0x00,
                                0x03, 0x04, 0x08, 0x00,
                                0x06, 0x08, 0x08, 0x02,
                                0x00
                                ]
                    reply += [STATUS_SUCCESS] + features

                elif packet[0] == CMD_WRITEGPIO1:
                    command_size = 2 + packet[1]
                    reply += [STATUS_SUCCESS]

                elif packet[0] == CMD_SETMAINBOARDID:
                    is_not_blank = 1
                    counter = 1
                    while is_not_blank:
                        if ord(packet[counter]) == 0x00 or ord(packet[counter]) == 0 or counter >= packet_length:
                            is_not_blank = 0
                        else:
                            counter+=1
                    command_size = counter
                    reply += [STATUS_SUCCESS]

                elif packet[0] == CMD_READCOIN:
                    reply += [STATUS_SUCCESS] + [0x00] + [self.coin] + [0x00] + [0x00]
                    command_size = 2

                elif packet[0] == CMD_READSWITCHES:
                    sw = [STATUS_SUCCESS] + [self.bits_to_byte(self.system_key)] + [self.bits_to_byte(self.player1_key[0:8])] + [self.bits_to_byte(self.player1_key[8:16])] + [self.bits_to_byte(self.player2_key[0:8])] + [self.bits_to_byte(self.player2_key[8:16])]
                    reply += sw
                    command_size = 3

                elif packet[0] == CMD_READANALOG:
                    command_size = 2
                    reply += [STATUS_SUCCESS] + [self.posx & 0xFF] + [0x00] + [self.posy & 0xFF] + [0x00] + [self.posx & 0xFF] + [0x00] + [self.posy & 0xFF] + [0x00]

                elif packet[0] == CMD_READSCREENPOS:
                    command_size = 2
                    reply += [STATUS_SUCCESS] + [0x00] + [self.posx & 0xFF] + [0x00] + [self.posy & 0xFF]

                else:
                    print "Error: Packet Unknown: ", hex(packet[0])

                packet_length -= command_size
                packet = packet[command_size:]

            self.write_reply(reply)

    def get_packet(self):
        """Get and process a packet"""
        while self.get_byte() != CMD_SYNC:
            pass

        checksum = 0
        packet = []

        packet_address = self.get_byte()
        checksum += packet_address
        print "Address: ", packet_address

        packet_length = self.get_byte() - 1
        checksum += packet_length + 1
        print "Length: ", packet_length

        while len(packet) < packet_length:
            byte = self.get_byte()
            checksum += byte
            packet += [byte]

        if (checksum & 0xFF) == self.get_byte():
            self.process_packet(packet, packet_address)
        else:
            print "Error: Checksum Problem"
