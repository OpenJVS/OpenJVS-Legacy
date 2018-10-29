#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
//#include <linux/input.h>

int initKeyboard();
void closeKeyboard();
void runKeyboard();

#endif // KEYBOARD_H_
