#ifndef PIT_H
#define PIT_H
#include "i8259.h"
#include "lib.h"
#include "scheduling.h"

extern int quantum;

#define MAXPIT 1193180


void pit_init();
void pit_write(int freq);
void pit_handler();

#endif
