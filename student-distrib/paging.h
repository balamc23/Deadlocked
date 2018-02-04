#ifndef PAGING_H
#define PAGING_H

#include "lib.h"
#define KERNEL 0x400000
#define FMB 0x80
#define VIDEO 0xB8000
#define VIDEO_PAGE_OFF 0xB8
#define page_entries 1024
#define page_alignment 4096




/* sets up paging */
void initPaging();
/* fills page directory */
void fill_directory();
/* fills out page_table */
void fill_table();
/* enables paging */

void setupPagingDirectory(uint32_t rsrved, uint32_t addr, int idx);

void setupPagingDirectoryTable(uint32_t rsrved, uint32_t addr, int idx);

extern void enablePaging(void *);



#endif
