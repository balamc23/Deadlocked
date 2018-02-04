#ifndef SYSCALL_H
#define SYSCALL_H

#include "lib.h"
#include "filesystem.h"
#include "paging.h"
#include "terminal.h"
#include "rtc.h"
#include "scheduling.h"


#define KERNELBOT 0x800000
#define STACKSIZE 0x8000
#define MASK 0xFE000
#define EXEC_LOCATION 0x08048000
#define TASK_SIZE 1024*4096
#define RESERV_CODE 0x87
#define RESERV_CODE_2 0x7
#define CONTEXTSWITCH_OFF 24
#define VM_VIRTUAL_OFF 32
#define USER_VIDEO 0x084B8000
#define bot 138412032

typedef struct fops_t {
  int (*open)(const int8_t *);
  int (*read)(int32_t,void*,int32_t);
  int (*write)(int32_t,const void*,int32_t);
  int (*close)(int32_t);
}fops_t;

typedef struct file_t {
    fops_t fotp;
    uint32_t inode;
    uint32_t fposition;
    uint32_t flags;
} file_t;


/*
PCB is used to store information we use to switch process.
It's used to switch to parent process (halt) and also
to switch process. It's used in scheduling

PCB holds necessary information which we can't keep globally.
The most information we keep from the PCB is the esp and ebp
We also store filename and fd.

*/
/*  PCB
*  	fd[8]
*	pid		process id number
*	filename	name of the program running
*	stack		address of the location in stack (Kernelbot - stacksize * pnum) .. etc
*	oldebp		for process switching
*	oldesp		for process switching
*	haltebp		ebp for returning to parent
*	haltesp		esp for returning to parent
*/
typedef struct pcb_t {
    file_t fd[8];
    int pid;
    char filename[32];
    uint32_t  stack;
    uint32_t  parent;
    uint32_t oldebp;
    uint32_t oldesp;
    uint32_t haltebp;
    uint32_t haltesp;
} pcb_t;

extern file_t fd[8];
extern int pnumarr[6];
extern int pnum;

int getargs (uint8_t* buf, int32_t nbytes);
int vidmap (uint8_t** screen_start);
int set_handler (int32_t signum, void* handler_address);
int sigreturn (void);


int read(int32_t fdr, void* buf, int32_t nbytes);
int write(int32_t fdr, const void* buf, int32_t nbytes);
int halt(uint8_t status);
int createPCB();
int createFD(int d);
void initFOTP();
int open(uint8_t * fname);
int close(int32_t fdr);
extern int execute(char * buf);
void parse_arg(char * buf);
int checkFile();
void setupPage();
void parentPage();
void loadFile();

extern void syscall_link();
extern void flushTLB();
extern int Save(uint32_t* oldesp, uint32_t* oldebp);
extern int contextSwitch(uint32_t addr, uint32_t* oldesp, uint32_t* oldebp);
extern int processSwitch(uint32_t newebp, uint32_t newesp, uint32_t* oldeip, uint32_t* oldesp);
extern void retToPar(uint32_t oldesp,uint32_t oldebp, uint32_t status);
int setPnum();
// extern void retToParSaveAll(uint32 pcb., uint32_t oldesp,uint32_t oldebp);

#endif
