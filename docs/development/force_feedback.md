# FFB Notes

With regards to ACKs, I think these are the translations.
0x00 - Ready for next command
0x11 - Not Initialized
0x44 - Busy - Not ready for next command


Checksum : xor first 3 bytes and AND with 0x7F
All the time command : FD 0 0 7D
Center command : 80 1 1 0
Roll right : 84 0 60 64
Roll left : 84 1 20 25
Stop Motor : 80 0 0 0
