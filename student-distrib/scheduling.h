#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "lib.h"
#include "syscall.h"
#include "paging.h"
#include "x86_desc.h"
#define SCREEN1_MEM 0x2000000
#define SCREEN2_MEM 0x2001000
#define SCREEN3_MEM 0x2002000

#define SCREEN1_VIR 0x84B9000
#define SCREEN2_VIR 0x84BA000
#define SCREEN3_VIR 0x84BB000

#define MAX_BYTES_BUF 129
#define SIZE_VID_MEM 4000

void pSwitch();
void setCursorandBuffer();
void getCursorandBuffer();
void screen1();
void screen2();
void screen3();
void copyFromVideo();
void copyToVideo();
void init_screen_memory();

typedef struct process_t process_t;

struct process_t {
  struct pcb_t *pcb;
  int screen;
  uint32_t ebp;
  uint32_t esp;
  process_t * next;
};

process_t process1;
process_t process2;
process_t process3;
extern process_t * head;
extern int indicator;
extern int active;
// Alt F1, F2, F3

#endif
