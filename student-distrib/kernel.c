/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "idt.h"
#include "keyboard.h"
#include "paging.h"
#include "rtc.h"
#include "paging.h"
#include "terminal.h"
#include "filesystem.h"
#include "syscall.h"
#include "scheduling.h"
#include "pit.h"

/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void
entry (unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;

	/* Clear the screen. */
	clear();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf ("Invalid magic number: 0x%#x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;

	/* Print out the flags. */
	printf ("flags = 0x%#x\n", (unsigned) mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		printf ("mem_lower = %uKB, mem_upper = %uKB\n",
				(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		printf ("boot_device = 0x%#x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		printf ("cmdline = %s\n", (char *) mbi->cmdline);

	if (CHECK_FLAG (mbi->flags, 3)) {
		int mod_count = 0;
		int i;
		module_t* mod = (module_t*)mbi->mods_addr;
		while(mod_count < mbi->mods_count) {
			printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
			printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
			printf("First few bytes of module:\n");
			for(i = 0; i<16; i++) {
				printf("0x%x ", *((char*)(mod->mod_start+i)));
			}
			printf("\n");
			FSYSTEM = mod->mod_start;
			mod_count++;
			mod++;
		}
	}
	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
	{
		printf ("Both bits 4 and 5 are set.\n");
		return;
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5))
	{
		elf_section_header_table_t *elf_sec = &(mbi->elf_sec);

		printf ("elf_sec: num = %u, size = 0x%#x,"
				" addr = 0x%#x, shndx = 0x%#x\n",
				(unsigned) elf_sec->num, (unsigned) elf_sec->size,
				(unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6))
	{
		memory_map_t *mmap;

		printf ("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
				(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *) mbi->mmap_addr;
				(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
				mmap = (memory_map_t *) ((unsigned long) mmap
					+ mmap->size + sizeof (mmap->size)))
			printf (" size = 0x%x,     base_addr = 0x%#x%#x\n"
					"     type = 0x%x,  length    = 0x%#x%#x\n",
					(unsigned) mmap->size,
					(unsigned) mmap->base_addr_high,
					(unsigned) mmap->base_addr_low,
					(unsigned) mmap->type,
					(unsigned) mmap->length_high,
					(unsigned) mmap->length_low);
	}

	/* Construct an LDT entry in the GDT */
	{
		seg_desc_t the_ldt_desc;
		the_ldt_desc.granularity    = 0;
		the_ldt_desc.opsize         = 1;
		the_ldt_desc.reserved       = 0;
		the_ldt_desc.avail          = 0;
		the_ldt_desc.present        = 1;
		the_ldt_desc.dpl            = 0x0;
		the_ldt_desc.sys            = 0;
		the_ldt_desc.type           = 0x2;

		SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
		ldt_desc_ptr = the_ldt_desc;
		lldt(KERNEL_LDT);
	}

	/* Construct a TSS entry in the GDT */
	{
		seg_desc_t the_tss_desc;
		the_tss_desc.granularity    = 0;
		the_tss_desc.opsize         = 0;
		the_tss_desc.reserved       = 0;
		the_tss_desc.avail          = 0;
		the_tss_desc.seg_lim_19_16  = TSS_SIZE & 0x000F0000;
		the_tss_desc.present        = 1;
		the_tss_desc.dpl            = 0x0;
		the_tss_desc.sys            = 0;
		the_tss_desc.type           = 0x9;
		the_tss_desc.seg_lim_15_00  = TSS_SIZE & 0x0000FFFF;

		SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

		tss_desc_ptr = the_tss_desc;

		tss.ldt_segment_selector = KERNEL_LDT;
		tss.ss0 = KERNEL_DS;
		tss.esp0 = 0x800000;
		ltr(KERNEL_TSS);
	}



	/* Initialize devices, memory, filesystem, enable device interrupts on the
	 * PIC, any other initialization stuff... */

	reset_screen();
  	idt_init();
 	initPaging();
	i8259_init();
	keyboard_init();
	update_cursor(0,0);
	rtc_init();
	pit_init();
	pit_write(40);

	// beep();

	//beep();
	// int i = 1/0;
	/* Enable interrupts */
	/* Do not enable the following until after you have set up your
	 * IDT correctly otherwise QEMU will triple fault and simple close
	 * without showing you any output */



	//printf("Enabling Interrupts\n");
	sti();


// test rtc write /  read
	// rtc_write(12); // do rtc_write 2 - 15
	// while(1){
	// 	rtc_read();
	// }


  //char buf[128] = "ugaifdugawfuigfauifauiwfaufaaufhfuaiafuwioiafjw";
	// char buf[129];
	// printf("\nHere-1\n");
	// terminal_read(0,buf,20);
	// printf("\nHere1\n");
	// terminal_write(0, buf, 128);
	// // printf("\n");
	// terminal_read(0,buf,128);
	// printf("\nHere2\n");
	// terminal_write(0, buf, 128);
  // // printf("\n");
	// terminal_read(0,buf,100);
	// printf("\nHere3\n");
	// terminal_write(0, buf, 80);
	// printf("\nHere2\n");

	//  unsigned int * a = NULL;
	//  unsigned int b = *a;
	// char x = getchar();
	// printf("%c\n", x);

/* List files tests*/

	// 	dentry_t dentry;
	// 	uint32_t * fsystem = (uint32_t *) FSYSTEM;
	// 	int directory_entries = *(fsystem);
	// 	int i;
	// 	for (i=0;i<directory_entries;i++){
	// 	int ret = read_dentry_by_index(i,&dentry);
	// 	if(ret!=-1){
	// 	 uint32_t * iNode = fsystem + 1024*(dentry.inode+1);
	// 	 printf("Fname: ");
	// 	 int j;
	// 	 for (j = 0; j<32; j++) {
	// 	 	putc(dentry.name[j]);
	// 	 }
	// 	printf("           Ftype: %d   file_size: %d\n", dentry.ftype, *iNode);
	// 	}
	// }


	//TEST -------------------

	// unsigned char name[33] = "shell";
	// dentry_t dentry;
	// int ret = read_dentry_by_name(name,&dentry);
	// // int ret = read_dentry_by_index(2,&dentry);
	// printf("Ret: %d \n", ret);
	//
	// printf("Fname: ");
	// int j;
	// for (j = 0; j<32; j++) {
	// 	putc(dentry.name[j]);
	// }
	// printf("\nFtype: %d\n", dentry.ftype);
	// printf("Inode: %d\n\n", dentry.inode);
	// int incr = 4;
	// unsigned char buf[incr+5];
	// int offset = 0;
	// int i;
	// ret = read_data(dentry.inode, offset, buf, incr);
// 	while (ret){
// 	for (i = 0;i<ret;i++){
// 		putc(buf[i]);
// 	}
// 	offset +=ret;
// 	ret = read_data(dentry.inode, offset, buf, incr);
// }
//
// printf("\nFname: ");
// for (j = 0; j<32; j++) {
// 	putc(dentry.name[j]);
// }
// printf("\n\n");

// TEST -------------

		// while(1){
		// 	char buf[129] = "shell";
		// 	// terminal_read(0,buf,128);
		// 	// buf = "shell";
		//  execute(buf);
		// }
		// char buf[129];
		// terminal_read(0,buf,128);
		// int ret = checkFile(buf);
		// setupPage();
		// printf("%d\n", ret);
		// parse_arg(buf);
		// printf("%s %s %s %s\n", filename, arg1, arg2, arg3);

	// int i;
	// for(i = 0; i < 4600; i++){
	// 	uint8_t * test = read_directory();
	// 	if(test) printf("%s ", test);
	// }

	/* Execute the first program (`shell') ... */
	// execute("shell");
	// screen1();
	init_screen_memory();

	// while(1+1 != 3);
	/* Spin (nicely, so we don't chew up cycles) */
	asm volatile(".1: hlt; jmp .1;");
}
