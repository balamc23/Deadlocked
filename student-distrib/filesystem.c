#include "filesystem.h"

int fidx = 0;
unsigned int FSYSTEM;
/*
* int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
*   Inputs: fname: the name of the file to search by
*           dentry: the directory entry to fill
*   Return Value: 0 if success
*                 -1 if failure
*	  Function: Searches the file image by name and fills up a directory entry struct
*/
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
  if(fname == NULL) return -1;
  // make sure its 32 bytes
  uint8_t tname[32];
  strncpy((int8_t *)tname,(int8_t *)fname,32);
  // get the module address
  int32_t * fsystem = (int32_t *) FSYSTEM;

  int32_t entry_no = *fsystem;
  fsystem += 16;
  // get the number of entries
  int i;
  int flag = 0;
  uint8_t* dir = (uint8_t*) fsystem;
  for(i = 0; i < entry_no; i++){
    // get the different directories
    uint8_t* curDir = dir + 64*i;
    // check if the strings match
    if(!strncmp((int8_t *)curDir, (int8_t *)fname, 32)){
      flag = 1;
      // set the correct dentry things
      dentry->name = curDir;
      dentry->ftype = *(curDir + 32);
      dentry->inode = *(curDir + 36);
      break;
    }
  }
  if(!flag){
     return -1;
    //  printf("\ncouldn't find file\n");
   }

  //  printf("\ncould find file\n");
  return 0;
}

/*
* int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
*   Inputs: index: the index of the directory
*           dentry: the directory entry to fill
*   Return Value: 0 if success
*                 -1 if failure
*	  Function: Searches the file image by index and fills up a directory entry struct
*/
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
  // get the module address
  int32_t * fsystem = (int32_t *) FSYSTEM;
  int32_t entry_no = *fsystem;
  fsystem += 16;
  // get the number of entries
  if(index > entry_no) return -1;
  int i;
  int flag = 0;
  uint8_t* dir = (uint8_t*) fsystem;
  for(i = 0; i < entry_no; i++){
    // get the different directories
    uint8_t* curDir = dir + 64*i;
    // check for correct index
    if(index == i){
      flag = 1;
      // set the correct dentry things
      dentry->name = curDir;
      dentry->ftype = *(curDir + 32);
      dentry->inode = *(curDir + 36);
      break;
    }
  }
  if(!flag) return -1;
  return 0;
}

/*
* int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
*   Inputs: inode: which inode to access
*           offset: offset from which to start reading
*           buf: buffer to fill with data to read data
*           length: the number of bytes to copy
*   Return Value: 0 if end of file is reach
*                 otherwise the number of bytes read
*	  Function: Reads data from a specfic inode and keeps it in a buffer
*/
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    filesystem_t file;
    file.bb = (boot_t *) FSYSTEM;
    file.inodes = (inode_t *) (FSYSTEM + sizeof(boot_t));
    file.blocks = (block_t *) (file.inodes + (file.bb->inodes));

    if(file.bb->inodes < inode || buf == NULL) return -1;
    if(offset >= file.inodes[inode].length) return 0;

    uint32_t blockoff = offset/sizeof(block_t);
    uint32_t dataoff = offset%sizeof(block_t);
    uint32_t bytesread = length;

    if((file.inodes[inode].length - offset) < length){
      bytesread = file.inodes[inode].length - offset;
    }

    int i;
    for(i = 0; i< bytesread;i++,blockoff = offset/sizeof(block_t),dataoff = offset%sizeof(block_t)){
      buf[i] = file.blocks[file.inodes[inode].dblock[blockoff]].data[dataoff];
      offset++;
    }
    return bytesread;
}

int file_read(int32_t fdr, void *buf, int32_t n){
  uint8_t* buff = (uint8_t*) buf;
  int32_t tmp = (read_data(fd[fdr].inode, fd[fdr].fposition, buff, n));
  fd[fdr].fposition+=tmp;
  return tmp;
}
int directory_read(int32_t fdr, void *buf, int32_t n){
  uint8_t* curDir = (uint8_t*)buf;
  dentry_t cur;
  if(read_dentry_by_index(fd[fdr].fposition, &cur)) return 0;
  int len = 32;
  strncpy((int8_t *) curDir, (int8_t *)cur.name,len);
  fd[fdr].fposition+=1;
  return len;
}
int file_write(int32_t fdr,const void *buf, int32_t n){
  return 0;
}
int file_open(const int8_t* filename){
  return 0;
}
int file_close(int32_t fdr){
  return 0;
}
