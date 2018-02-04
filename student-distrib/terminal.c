#include "terminal.h"

char tbuffer[129];
int tbuf;
int tflag;
int dflag = 1;

/*
* int terminal_read(int32_t fd, void *buffer, int32_t n)
*   Inputs: int32_t fd, void *buffer, int32_t n
*   Return Value: 0 on success
* Function: Reads the terminal and puts it in a buffer
*/
int terminal_read(int32_t fd, void *buffer, int32_t n){
  // while(indicator!=head->screen){}
  char* buf = (char*)buffer;
  tbuf = 0;
  tflag = 1;
  while(tflag){
    // sti();
  };

  // cli();
  int i;
  for(i = 0; i < 129; i++){
    buf[i] = 0;
  }
  if(tbuf < 128)
    tbuffer[tbuf] = '\n';
  else{
    tbuffer[128] = '\n';
    tbuf = 127;
  }
  if(tbuf > n){
    for(i = 0; i < n; i++){
      buf[i] = tbuffer[i];
    }
    buf[n] = '\n';
    return n;
  }
  for(i = 0; i < tbuf; i++){
    buf[i] = tbuffer[i];
  }
  buf[i] = '\n';

  return tbuf+1;
}

/*
* terminal_write(int32_t fd, const void *buffer, int32_t n)
*   Inputs:int32_t fd, const void *buffer, int32_t n
*   Return Value: 0 on success, -1 on failure
* Function: Writing to terminal, returns 0 on success
*/
int terminal_write(int32_t fd, const void *buffer, int32_t n){
  // if(indicator!=head->screen) return 0;
  while(indicator!=head->screen){}
  // if(indicator == head->screen){
  //   video_mem = (char *)VIDEO;
  // }
  // else if(head->screen == 1){
  //   video_mem = (char *)SCREEN1_VIR;
  // }
  // else if(head->screen == 2){
  //   video_mem = (char *)SCREEN2_VIR;
  // }
  // else if(head->screen == 3){
  //   video_mem = (char *)SCREEN3_VIR;
  // }
  char* buf = (char*)buffer;
  int i;
  for (i = 0; i < n; i++) {
      putc(buf[i]);
  }
  return 0;
}

/*
*terminal_invalid_read(int32_t fd, void *buf, int32_t n)
*   Inputs:int32_t fd, void *buf, int32_t n
*   Return Value: -1 on failure
* Function: used in syscalls, returns -1
*/
int terminal_invalid_read(int32_t fd, void *buf, int32_t n){
  return -1;
}

/*
*terminal_invalid_write(int32_t fd, void *buf, int32_t n)
*   Inputs:int32_t fd, void *buf, int32_t n
*   Return Value: -1 on failure
* Function: used in syscalls, returns -1
*/
int terminal_invalid_write(int32_t fd, const void *buf, int32_t n){
  return -1;
}

/*
* int terminal_open(const int8_t * filename)
*   Inputs:const int8_t * filename
*   Return Value: -1 on failure
* Function: used in syscalls, returns -1
*/
int terminal_open(const int8_t * filename){
  return -1;
}

/*
* int terminal_close(int32_t fd, void *buffer, int32_t n)
*   Inputs:int32_t fd, void *buffer, int32_t n
*   Return Value: -1 on failure
* Function: used in syscalls, returns -1
*/
int terminal_close(int32_t fd){
  return -1;
}
