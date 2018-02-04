#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "lib.h"
#include "syscall.h"

extern int fidx;
extern unsigned int FSYSTEM;



/* The struct where data from the filesystem is stored */
typedef struct dentry_t {
    uint8_t * name;
    uint32_t ftype;
    uint32_t inode;
    uint32_t reserved[6];
} dentry_t;

typedef struct d2entry_t {
    uint8_t name[32];
    uint32_t ftype;
    uint32_t inode;
    uint32_t reserved[6];
} d2entry_t;

typedef struct boot_t {
    uint32_t ndentries;
    uint32_t inodes;
    uint32_t dblocks;
    uint32_t reserved[13];
    d2entry_t dentries[63];
} boot_t;

typedef struct inode_t {
    uint32_t length;
    uint32_t dblock[1023];
} inode_t;

typedef struct block_t {
    uint8_t data[4096];
} block_t;

typedef struct filesystem_t {
  boot_t * bb;
  inode_t * inodes;
  block_t * blocks;
} filesystem_t;


int file_read(int32_t fdr, void *buf, int32_t n);
int file_write(int32_t fdr, const void *buf, int32_t n);
int file_open(const int8_t* filename);
int file_close(int32_t fdr);

/* Searches the file image by name and fills up a directory entry struct */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);

/* Searches the file image by index and fills up a directory entry struct */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);

/* Reads data from a specfic inode and keeps it in a buffer */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

int32_t directory_read(int32_t fdr, void *buf, int32_t n);
#endif
