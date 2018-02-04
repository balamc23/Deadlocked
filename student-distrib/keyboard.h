#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "i8259.h"
#include "lib.h"


/* initializes keyboard */
void keyboard_init();
/* keyboard handler */
void keyboard_handler();
/* gets scancode from keyboard port */
char getScancode();
/* does nothing importand for now */
char getchar();

#endif
