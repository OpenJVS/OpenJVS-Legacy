import evdev

if __name__ == '__main__':
     X = 0
     Y = 0
     X_MAX = 32767
     Y_MAX = 32767
     DEVICE = None

     DEVICES = [evdev.InputDevice(fn) for fn in evdev.list_devices()]

     for d in DEVICES:
       if 'Tablet' in d.name or 'Absolute' in d.name or 'AimTrak' in d.name:
             DEVICE = d
             print('Found %s at %s...' % (d.name, d.fn))
             break



     if DEVICE:
         caps = DEVICE.capabilities(verbose=False)
         print "Buttons:", " ".join([ str(x) for x in caps.get(evdev.ecodes.EV_KEY, [])])
         print "Axes:", " ".join([ str(trash) for (axis, trash) in caps.get(evdev.ecodes.EV_ABS, []) ])

         for event in DEVICE.read_loop():
             if event.type == evdev.ecodes.EV_KEY:
                 print "Butotn Pressed"
             if event.type == evdev.ecodes.EV_ABS:
                 if event.code == evdev.ecodes.ABS_X:
                     X = int(100 * float(event.value) / float(X_MAX))
                 if event.code == evdev.ecodes.ABS_Y:
                     Y = int(100 * float(event.value) / float(Y_MAX))
                 print('X=%d Y=%d' % (X, Y))
