#include "syscall.h"
#include "x86_desc.h"

int pnum = 0;
int  pnumarr[6]  = {0,0,0,0,0,0};
file_t fd[8];
pcb_t* curr_pcb;
static fops_t trfotp;
static fops_t twfotp;
static fops_t rfotp;
static fops_t dfotp;
static fops_t ffotp;

static char filename[32];
static char arg[32];
static dentry_t dentry;
static dentry_t dt;
static uint32_t addrPage;

/*
* int execute(char * buf)
*   Inputs: buf: the name of the program to execute
*   Return Value: 0 if success
*                 -1 if failure
*	  Function: Allows the user to execute their own program
*/
int execute(char * buf){


  // printf("\n%d\n",pnum);
  parse_arg(buf);
  if(checkFile(filename)) return -1;
  if (setPnum()) {
      printf("Too many processes. Execute exit!\n");
      return 0;
  }
  // printf("\n%d\n",pnum );
  setupPage();
  loadFile();
  initFOTP();
  return createPCB();
}

/*
* int open(uint8_t * fname)
*   Inputs: buf: the name of the file to open
*   Return Value: 0 if success
*                 -1 if failure
*	  Function: Allows the user to open any type of file
*/
int open(uint8_t * fname){
  if(read_dentry_by_name(fname, &dt) == -1) return -1;
  return createFD(dt.ftype);

}

/*
* int close(int32_t fdr)
*   Inputs: fdr: File descriptor to close
*   Return Value: 0 if success
*                 -1 if failure
*	  Function: Allows the user to close any open files
*/
int close(int32_t fdr){
  if(fdr >= 2 && fdr <= 8){
    if(fd[fdr].flags == 0) return -1;
    fd[fdr].flags = 0;
    return fd[fdr].fotp.close(fdr);
  }
  return -1;
}

/*
* int halt(uint8_t status)
*   Inputs: status: Hardware uses it
*   Return Value: 0 if success
*                 -1 if failure
*	  Function: Allows the user to halt any executing program
*/
int halt(uint8_t status){

  /* we'll need to go to pid-> parent*/
  pnumarr[pnum] = 0;
  if (pnum < 3) {
    // printf("\nHERE\n");
    execute("shell");
  }
  uint32_t oldesp = curr_pcb->haltesp;
  uint32_t oldebp = curr_pcb->haltebp;
  tss.esp0 = curr_pcb->oldesp;
  int j;
  for(j = 2; j < 8; j++){
    close(j);
  }
  curr_pcb = (pcb_t*)curr_pcb->parent;
  pnum = curr_pcb->pid;
  memcpy(&fd, &(curr_pcb->fd), sizeof(fd));
  // uint32_t * entry_point = (uint32_t *)(EXEC_LOCATION + 24);

  strcpy(filename, (curr_pcb->filename));
  parentPage();
  int i = 0;
  process_t * curr = head;
  int found = 0;
    while(i<3 && head!= NULL){
      if(curr->screen == indicator){
        found = 1;
        break;
      }
      curr = curr->next;
      i++;
    }

  // if (strncmp("shell", filename, 32)) {
        if(!found){
          if (active == 2) {
              process2.next = &process1;
          } else if (active == 1) {
              process1.next = &process1;
          } else if (active == 0) {
              process1.next = NULL;
              head = NULL;
          }
        }
        else{
          curr->pcb = curr_pcb;
        }
  // }

  retToPar(oldesp,oldebp,status);
  return 0;
}

/*
* void parse_arg(char * buf)
*   Inputs: buf: the buffer to parse
*   Return Value: none
*	  Function: Parses the arguements passed into execute
*/
void parse_arg(char * buf){
  int i = 0;
  int c = 0;
  char* curr = filename;
  for (;i<32;i++,c++){
    while(buf[c]!='\0' && buf[c]!=' '){
      curr[i] = buf[c];
      i++;
      c++;
    }
    curr[i] = '\0';
    if(buf[c] == ' '){
      curr = arg;
      i = -1;
    }
    else if(curr == filename){
      arg[0] = '\0';
      break;
    }
  }
    //
    // printf("arg: %s    filename: %s buf: %s \n", arg, filename, buf);

}

/*
* int checkFile()
*   Inputs: none
*    Return Value: 0 if success
*                 -1 if failure
*	  Function: Checks if the file is a valid executable
*/
int checkFile(char * fname){
  // reads the first 4 bytes to see if it is an executable
  if(read_dentry_by_name((uint8_t*)fname, &dentry)) return -1;
  uint8_t buf[4];
  read_data(dentry.inode,0,buf,4);
  if(buf[0] == 0x7f && buf[1] == 0x45 && buf[2] == 0x4c && buf[3] == 0x46) return 0;
  return -1;
}

/*
* int write(int32_t fdr,const void* buf, int32_t nbytes)
*   Inputs: fdr: File to write to
*           buf: Value to write
*           nbytes: Number of bytes to write
*    Return Value: 0 if success
*                 -1 if failure
*	  Function: Writes the specified number of bytes to file
*/
int write(int32_t fdr,const void* buf, int32_t nbytes) {
  if(buf == NULL || fdr<0 || fdr>7) return -1;
  if(fd[fdr].flags) return ((fd[fdr].fotp.write))(fd[fdr].inode, buf, nbytes);
  else return -1;
 }

 /*
 * int read(int32_t fdr, void* buf, int32_t nbytes)
 *   Inputs: fdr: File to read from
 *           buf: Value to read
 *           nbytes: Number of bytes to read
 *    Return Value: 0 if success
 *                 -1 if failure
 *	  Function: Reads the specified number of bytes from file
 */
int read(int32_t fdr, void* buf, int32_t nbytes) {
  if(buf == NULL || fdr<0 || fdr>7) return -1;
  if(fd[fdr].flags) return ((fd[fdr].fotp.read))(fdr, buf, nbytes);
  else return -1;
 }

/*
* void setupPage()
*   Inputs: None
*    Return Value: Void function
*	  Function: Specifies address of new page
*/
void setupPage(){
  addrPage = KERNEL + (TASK_SIZE * (pnum+1));
  setupPagingDirectory(RESERV_CODE, addrPage,VM_VIRTUAL_OFF);
  flushTLB();
}

/*
* void ParentPage()
*   Inputs: None
*    Return Value: Void function
*	  Function: Sets up parent page
*/
void parentPage() {
    addrPage = KERNEL + (TASK_SIZE * (pnum+1));
    setupPagingDirectory(RESERV_CODE, addrPage, VM_VIRTUAL_OFF);
    flushTLB();
}


/*
* void loadFile()
*   Inputs: None
*    Return Value: Void function
*	  Function: Reads program into location in memory
*/
void loadFile(){
  // uint32_t addr = page_directory[32] & 0xFFF0;
  read_data(dentry.inode,0,(uint8_t*)(EXEC_LOCATION),TASK_SIZE);
  // memcpy((uint8_t *)addrPage,buf,4096);
}


/*
* void initFOTP()
*   Inputs: None
*   Return Value: n/a
*	Function: Given filme system abstractions and file positions, we fill
*   fd file diescriptor for stdin and stdout
*/
void initFOTP(){

  trfotp.open = &terminal_open;
  trfotp.read = &terminal_read;
  trfotp.write = &terminal_invalid_write;
  trfotp.close = &terminal_close;

  twfotp.open = &terminal_open;
  twfotp.read = &terminal_invalid_read;
  twfotp.write = &terminal_write;
  twfotp.close = &terminal_close;

  rfotp.open = &rtc_open;
  rfotp.read = &rtc_read;
  rfotp.write = &rtc_write;
  rfotp.close = &rtc_close;

  dfotp.open = &file_open;
  dfotp.read = &directory_read;
  dfotp.write = &file_write;
  dfotp.close = &file_close;

  ffotp.open = &file_open;
  ffotp.read = &file_read;
  ffotp.write = &file_write;
  ffotp.close = &file_close;

  //stdin
  fd[0].fotp = trfotp;
  fd[0].inode = 0;
  fd[0].fposition = 0;
  fd[0].flags = 1;

  //stdout
  fd[1].fotp = twfotp;
  fd[1].inode = 0;
  fd[1].fposition = 1;
  fd[1].flags = 1;
}


/*
* int createFD(int d)
*   Inputs: RTC or file
*   Return Value: -1 on not success
*	Function: Create file descriptor, e.g. rtc
*/
int createFD(int d){
  fops_t fotp;
  if(!d) fotp = rfotp;
  else if(d == 1) fotp = dfotp;
  else fotp = ffotp;
  int i;
  for(i = 2; i < 8; i++){
    if(!fd[i].flags){
      fd[i].fotp = fotp;
      if(d == 2){
        fd[i].inode = dt.inode;
      }
      else fd[i].inode = 0;
      fd[i].fposition = 0;
      fd[i].flags = 1;
      return i;
    }
  }
  // printf("\nerr\n");
  return -1;
}

/*
* int createPCB()
*   Inputs: None
*   Return Value: 0 on success
*	Function: create entry point for context switching, and move in PCB stack
*/
int createPCB(){

  pcb_t* pcb = (pcb_t*)((KERNELBOT - STACKSIZE*(pnum+1)));;
  memcpy(&(pcb->fd), &fd, sizeof(fd));
  pcb->stack = (KERNELBOT - STACKSIZE*pnum) - 1;

  pcb->parent = (uint32_t)curr_pcb;
  pcb->pid = pnum;
  strcpy(pcb->filename, filename);

  process_t* save;
  curr_pcb = pcb;

  // change stack pointer
  tss.esp0 = pcb->stack;

  // create entry point and jump to it in user
  uint32_t  *entry_point = (uint32_t*)(EXEC_LOCATION + CONTEXTSWITCH_OFF);
  // if (strncmp("shell", filename, 32)) {
  int i = 0;
  process_t * curr = head;
  int found = 0;
    while(i<3 && head!= NULL){
      if(curr->screen == indicator){
        found = 1;
        break;
      }
      curr = curr->next;
      i++;
    }
    if(!found){
        if (active == 1) {
            process1.next = &process1;
            process1.pcb = curr_pcb;
            process1.screen = indicator;
            process1.ebp = KERNELBOT-STACKSIZE*pnum-1;
            process1.esp = KERNELBOT-STACKSIZE*pnum-1;
            head = &process1;
            save = &process1;
        } else if (active == 2) {
            process2.next = &process1;
            process2.pcb = curr_pcb;
            process2.screen = indicator;
            process2.ebp = KERNELBOT-STACKSIZE*pnum-1;
            process2.esp = KERNELBOT-STACKSIZE*pnum-1;
            process1.next = &process2;
            head = &process2;
            save = &process1;
        } else if (active == 3){
          //   process1.next = process2;
            process3.next = &process1;
            process3.pcb = curr_pcb;
            process3.screen = indicator;
            process3.ebp = KERNELBOT-STACKSIZE*pnum-1;
            process3.esp = KERNELBOT-STACKSIZE*pnum-1;
            process2.next = &process3;
            save = head;
            head = &process3;
        }
      }
      else{
        curr->pcb = curr_pcb;
        curr->ebp = KERNELBOT-STACKSIZE*pnum-1;
        curr->esp = KERNELBOT-STACKSIZE*pnum-1;
        save = curr;
        head = curr;

      }
      // printf("\n%x\n",*entry_point);
  // }
  if(!found)contextSwitch(*entry_point,&(save->pcb->oldesp),&(save->pcb->oldebp));

  else{
    Save(&(save->pcb->oldesp),&(save->pcb->oldebp));
    contextSwitch(*entry_point,&(save->pcb->haltesp),&(save->pcb->haltebp));

  }
  return 0;
}

/*
* int getargs(uint8_t* buf, int32_t nbytes)
*   Inputs: uint8_t *buf -  buffer to store argument in
            nbytes       -  nbytes is the limit for size of arg need to check
*   Return Value: 0 on success, else -1
* Function: system call to put argument into buffer, returns 0 if success
*/
int getargs (uint8_t* buf, int32_t nbytes){
  // //
  // printf("buf: %s    bufnum: %d   nbytes: %d\n", arg, sizeof(arg), nbytes);
  // printf("%s\n", arg);
  if (((sizeof(arg)) >= nbytes) || (arg[0] == '\0')) {
    return -1;
  }
  int i = 0;

  while(arg[i] != '\0') {
    buf[i] = arg[i];
    i++;
  }

  //fleg = checkFile(arg);
  // printf("fleg: %d", fleg);
  buf[i] = '\0';
  // printf("buffer is:%s\n", buf);
  return 0;


}

/*
* int vidmap(uint8_t** screen_start)
*   Inputs: uint8_t ** screen_start
*   Return Value: 0 on success
* Function: call maps the text-mode video memory into userspace at set pre-set virtual address
*/
int vidmap (uint8_t** screen_start){

  if (screen_start == NULL || screen_start <(uint8_t**)(EXEC_LOCATION/4) || screen_start >= (uint8_t**)(USER_VIDEO)) {return -1;}
  addrPage = VIDEO;
 // setupPagingDirectoryTable(RESERV_CODE_2, addrPage,0);
  *screen_start = (uint8_t*)(USER_VIDEO+indicator*4096);
 return 0;

}

/*
* int set_handler(int32_t signum, void* handler_address)
*   Inputs: int32_t signum        - specifies which signal's handler to change
            void* handler_address - points to user-level function to run when signal is received
*   Return Value: 0 on success, -1 on failure
* Function: changes default action when signal is received
*/
int set_handler (int32_t signum, void* handler_address){
return -1;

}

/*
* int sigreturn (void)
*   Inputs: none
*   Return Value: returns hardware contxt's eax value
* Function: should copy the hardware context that was on the user-level stack back to processor
*/
int sigreturn (void){
return -1;
}

/*
* int setPnum ()
*   Inputs: none
*   Return Value: returns 0 if successgully found a place in memory
*                         -1 if no place is available
* Function: goes through the physical memory to see if a spot is available
*/
int setPnum(){
  int i;
  if(indicator == 1){
    if(pnumarr[0] == 0){
      pnumarr[0] = 1;
      pnum = 0;
      return 0;
    }
  }
  else if(indicator == 2){
    if(pnumarr[1] == 0){
      pnumarr[1] = 1;
      pnum = 1;
      return 0;
    }
  }
  else if(indicator == 3){
    if(pnumarr[2] == 0){
      pnumarr[2] = 1;
      pnum = 2;
      return 0;
    }
  }
  for(i = 3;i<6;i++){
    if(!pnumarr[i]){
      pnumarr[i] = 1;
      pnum = i;
      return 0;
    }
  }
  return -1;
}
