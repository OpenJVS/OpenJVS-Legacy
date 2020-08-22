import serial

ser = serial.Serial('/dev/ttyUSB1', 38400)

def ack(number):
    ser.write(int(number))
    ser.flush()
#    print "Ack: ", hex(number)

while True:
    count = 0
    serial_buffer = []
    while count < 4:
        serial_buffer.append(ord(ser.read(1)))
        count = count + 1
#    for i in range(0, len(serial_buffer)):
#        print (serial_buffer[i]),":",hex(serial_buffer[i])," ",
#    print ""
    if (serial_buffer[0] ^ serial_buffer[1] ^ serial_buffer[2]) & 0x7F == serial_buffer[3]:
        if serial_buffer == [255, 0, 0, 127]:
            ack(0x11)
        elif serial_buffer[0] == 129:
            ack(0x00)
        else:
            ack(0x00)
    else:
        print "failed checksum"
ser.close()
