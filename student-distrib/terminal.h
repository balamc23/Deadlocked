#ifndef TERMINAL_H
#define TERMINAL_H

#include "lib.h"
#include "keyboard.h"

int terminal_read(int32_t fd, void *buffer, int32_t n);
int terminal_write(int32_t fd, const void *buffer, int32_t n);
int terminal_invalid_read(int32_t fd, void *buf, int32_t n);
int terminal_invalid_write(int32_t fd, const void *buf, int32_t n);
int terminal_open(const int8_t * filename);
int terminal_close(int32_t fd);

extern char tbuffer[129];
extern int tbuf;
extern int tflag;

#endif
