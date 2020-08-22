# FFB Notes

With regards to ACKs, I think these are the translations.
0x00 - Ready for next command
0x11 - Not Initialized
0x44 - Busy - Not ready for next command


Checksum : xor first 3 bytes and AND with 0x7F
Reset : FF 0 0 7F
All the time command : FD 0 0 7D
set force : 83
Center command : 80 1 1 0
Roll right : 84 0 60 64
Roll left : 84 1 20 25
Auto Center: 84 1 0 5 
Stop Motor : 80 0 0 0
Move Motor : 80 1 1 0
Move all the way to thr right 90 degrees: FC 0 20 5C  

Here's move left with almost no power up to higher power in increments:
84 1 16
84 1 28
84 1 30

Here's right with the same behavior:
84 0 76
84 0 68
84 0 58

As an example:
87 00 4D sets a target point at about 90 degrees right of center.
84 01 00 tells it to go to that point and stay there.

87 01 00 sets target point at center.
84 01 00 tells it to go to that point and stay there.

87 01 1A sets a target point left of center.
84 01 00 tells it to go to that point and stay there.

Things to keep track of:
Notes:
Target position	Translate to MIDI equivalent: 87 to establish position and 84 01 00 to move there. Only needs to be set when there is a change from the previous value, so tracking will be done in a variable and compared to current command. Translation formula needs to determine if target position is right or left of center and then establish a MIDI equivalent, probably through scaling.
Holding force	Translate to MIDI equivalent: 83 - Only needs to be set when there is a change from the previous value, so tracking will be done in a variable and compared to current command. This should end up being a relatively simple translation where I come up with a scaling formula from Namco to MIDI using my best guess about equivalent force. 
Movement speed	Need to translate to MIDI equivalent: 84? - Speed for MIDI seems to be a function of how far away from the target point the movement command is specified... Maybe this can be ignored?
Temp destination	Translate to MIDI move left/right, and then maybe re-center: 84 - I may rely on future commands to re-center, but otherwise the wheel needs to be re-centered after reaching the destination: 84 01 00
The translation formula can be treated the same as the Target Position one. When this value is present in a Namco command, it gets priority over the Target Position.
Kill command: 
FF FF FF 01 00 00 00 00 00 00	Translate to MIDI equivalent: 80 0 0 0
