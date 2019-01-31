int _ZN8LGJCbord4recvEv() {
    ebx = arg_0;
    edi = ebx + 0x44;
    memset(edi, 0x0, 0x80);
    eax = *(ebx + 0x14);
    eax = _lgjSCI::recv(eax, edi);
    ecx = 0x1;
    if (eax <= 0x0) goto loc_821f545;

loc_821f431:
    edx = 0x0;
    asm { prefetcht0 byte [edi] };
    asm { prefetcht0 byte [ebx+0x84] };
    goto loc_821f44c;

loc_821f44c:
    if (*(int8_t *)(edx + ebx + 0x44) != 0xc0) goto loc_821f444;

loc_821f453:
    esi = edx;
    goto loc_821f455;

loc_821f455:
    ecx = 0x3;
    edx = (esi == 0xffffffff ? 0x1 : 0x0) | (esi >= eax ? 0x1 : 0x0);
    if (edx == 0x0) {
            edx = 0x0;
            eax = esi + 0x1;
            ecx = esi + 0x15;
            while (ecx > eax) {
                    edx = edx ^ *(int8_t *)(eax + ebx + 0x44);
                    eax = eax + 0x1;
            }
            edi = *(int8_t *)(esi + ebx + 0x59) & 0xff;
            ecx = 0x4;
            if (edi == edx) {
                    eax = *(int8_t *)(esi + ebx + 0x44) & 0xff;
                    edx = *(int8_t *)(esi + ebx + 0x45) & 0xff;
                    ecx = 0x0;
                    *(int8_t *)(ebx + 0x2e) = eax;
                    eax = *(int8_t *)(esi + ebx + 0x46) & 0xff;
                    *(int8_t *)(ebx + 0x2f) = edx;
                    *(int8_t *)(ebx + 0x30) = eax;
                    edx = *(int8_t *)(esi + ebx + 0x47) & 0xff;
                    eax = *(int8_t *)(esi + ebx + 0x48) & 0xff;
                    *(int8_t *)(ebx + 0x31) = edx;
                    *(int8_t *)(ebx + 0x32) = eax;
                    edx = *(int8_t *)(esi + ebx + 0x49) & 0xff;
                    eax = *(int8_t *)(esi + ebx + 0x4a) & 0xff;
                    *(int8_t *)(ebx + 0x33) = edx;
                    edx = *(int8_t *)(esi + ebx + 0x4b) & 0xff;
                    *(int8_t *)(ebx + 0x34) = eax;
                    eax = *(int8_t *)(esi + ebx + 0x4c) & 0xff;
                    *(int8_t *)(ebx + 0x35) = edx;
                    *(int8_t *)(ebx + 0x36) = eax;
                    edx = *(int8_t *)(esi + ebx + 0x4d) & 0xff;
                    eax = *(int8_t *)(esi + ebx + 0x4e) & 0xff;
                    *(int8_t *)(ebx + 0x37) = edx;
                    *(int8_t *)(ebx + 0x38) = eax;
                    edx = *(int8_t *)(esi + ebx + 0x4f) & 0xff;
                    eax = *(int8_t *)(esi + ebx + 0x50) & 0xff;
                    *(int8_t *)(ebx + 0x39) = edx;
                    edx = *(int8_t *)(esi + ebx + 0x51) & 0xff;
                    *(int8_t *)(ebx + 0x3a) = eax;
                    eax = *(int8_t *)(esi + ebx + 0x52) & 0xff;
                    *(int8_t *)(ebx + 0x3b) = edx;
                    *(int8_t *)(ebx + 0x3c) = eax;
                    edx = *(int8_t *)(esi + ebx + 0x53) & 0xff;
                    eax = *(int8_t *)(esi + ebx + 0x54) & 0xff;
                    *(int8_t *)(ebx + 0x3d) = edx;
                    *(int8_t *)(ebx + 0x3e) = eax;
                    edx = *(int8_t *)(esi + ebx + 0x55) & 0xff;
                    eax = *(int8_t *)(esi + ebx + 0x56) & 0xff;
                    *(int8_t *)(ebx + 0x3f) = edx;
                    edx = *(int8_t *)(esi + ebx + 0x57) & 0xff;
                    *(int8_t *)(ebx + 0x40) = eax;
                    *(int8_t *)(ebx + 0x41) = edx;
                    *(int8_t *)(ebx + 0x42) = *(int8_t *)(esi + ebx + 0x58) & 0xff;
                    *(int8_t *)(ebx + 0x43) = edi;
            }
    }
    goto loc_821f545;

loc_821f545:
    eax = ecx;
    return eax;

loc_821f444:
    esi = 0xffffffff;
    edx = edx + 0x1;
    if (edx > 0x7f) goto loc_821f455;
}
