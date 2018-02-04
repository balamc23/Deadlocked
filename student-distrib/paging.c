#include "paging.h"

unsigned int page_directory[page_entries] __attribute__((aligned(page_alignment)));
unsigned int page_table[page_entries] __attribute__((aligned(page_alignment)));
unsigned int page_table_2[page_entries] __attribute__((aligned(page_alignment)));

/*
* void initPaging()
*   Inputs: none
*   Return Value: none
*	Function: sets up table for video memory and directory, then enables paging
*/
void initPaging(){
  fill_table();
  fill_directory();
  enablePaging(page_directory);
}

/*
* void fill_directory()
*   Inputs: none
*   Return Value: none
*	Function: fills out directory entries, defining vidoe memory and kernel pages
*/
void fill_directory(){
  int i;
  page_directory[0] = ((unsigned int)page_table| 1);
  page_directory[1] = (KERNEL | 0x83);
  

  for(i = 2; i < page_entries; i++)
  {
    page_directory[i] = 0x00;
  }
  page_directory[33] = ((unsigned int)page_table_2| 7);
}

/*
* void fill_table()
*   Inputs: none
*   Return Value: none
*	Function: fills out table for vidoe memory
*/
void fill_table(){
  int i;
  for(i = 0; i < page_entries; i++)
  {
    page_table[i] = (i * 0x1000);
    page_table_2[i] = (i * 0x1000);

  }

    page_table[VIDEO_PAGE_OFF] = VIDEO | 1;

}

void setupPagingDirectory(uint32_t rsrved, uint32_t addr, int idx){
  page_directory[idx] = addr | rsrved;
  // page_directory[pnum + 2] = ((KERNEL + (4096 * (pnum+1))) | 0x81);
}

void setupPagingDirectoryTable(uint32_t rsrved, uint32_t addr, int idx){
    page_table_2[VIDEO_PAGE_OFF + idx] = addr | rsrved;
}





// void init_paging(){
//
//
//
//   // int i;
//   // for(i = 0; i < 1024; i++){
//   //   page_table[i] = (i * 0x1000);
//   //   page_directory[i] = 0;
//   // }
//   //
//   //   page_table[VIDEO >> 12] |= 3;
//   //
//   //
//   // page_directory[0] = (unsigned int) page_table | 1;
//   // page_directory[1] = KERNAL | FMB | 3;
// }
