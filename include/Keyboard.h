#define _BSD_SOURCE
#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int initKeyboard();
void closeKeyboard();

#endif // KEYBOARD_H_
