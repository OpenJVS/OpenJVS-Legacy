import cwiid, time
from JVSE import JVSE
import threading


print "JVSE Wii Remote Driver Version 1.2"
emulator = JVSE()

failed = 1
while failed is 1:
    try:
        print "Press 1 + 2 on your wii remote."
        wm = cwiid.Wiimote()
        failed = 0
    except:
        failed = 1

time.sleep(1)

print "Wii Connected"
emulator.start()
print "JVSE Started"

wm.rpt_mode = cwiid.RPT_IR | cwiid.RPT_BTN


ax = 0
ay = 0
bx = 0
by = 0

main_x = 0
main_y = 0

running = 1

while running:
    a = wm.state['ir_src'][0]
    b = wm.state['ir_src'][1]

    if wm.state['buttons'] == 0:
        emulator.set_key_player1(6, 0)
        emulator.set_key_player1(7, 1)
        emulator.set_key_player1(0, 0)

    if wm.state['buttons'] == 2:
        #print "exit"
        running = 0

    if wm.state['buttons'] == 1:
        #print "Start"
        emulator.set_key_player1(0, 1)

    if wm.state['buttons'] == 16:
        #print "coin"
        emulator.increment_coin()

    if wm.state['buttons'] == 8:
        #print "shoot"
        emulator.set_key_player1(7, 0)
        emulator.set_key_player1(6, 1)


    if a is not None and b is not None:
        ax, ay = a['pos']
        bx, by = b['pos']
        out = 0
    else:
        out = 1


    avg_x = (ax + bx) / 2
    avg_y = (ay + by) / 2

    main_y = avg_y
    main_x = 1024 - avg_x

    perc_x = (float(main_x) / float(1024)) * 255
    perc_y = (float(main_y) / float(1024)) * 255

    #print perc_x, ":", perc_y

    emulator.set_pos(int(perc_x), int(perc_y))

    time.sleep(.1)
