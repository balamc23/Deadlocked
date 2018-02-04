#include "rtc.h"
#include "lib.h"

int current_rtc = 0;
int rate_glob = 0x6;
int read_flag = 0;

/*
* void keyboard_init()
*   Inputs: none
*   Return Value: none
*	Function: initializes rtc, enables IRQ 8 for rtc interrupts, and sets frequency
*/
void rtc_init(){

	char prev;
	/* Initializes the RTC, sets freq to 1024*/
	int rate = RTC_RATE_1024 & DEFAULT_RATE;				// Rate is between 2 and 15
    outb(STATUS_REGISTER_A, RTC_PORT); // set index to register A, disable NMI
	prev = inb(CMOS_PORT);				// get initial value of register A
	outb(STATUS_REGISTER_A, RTC_PORT);		// reset index to A
	outb((prev & 0xF0) | rate, CMOS_PORT); 	// write only our rate to A. Rate is 1024


	outb(STATUS_REGISTER_B, RTC_PORT);		// select register B, and disable NMI
	prev = inb(CMOS_PORT);					// read the current value of register B
	outb(STATUS_REGISTER_B, RTC_PORT);		// set the index again (a read will reset the index to register D)
	outb((prev | 0x40), CMOS_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B

	/* If register C is not read after an irq 8, interrupt will not happen agai*/
	outb(STATUS_REGISTER_C, RTC_PORT);	// select register C
	inb(CMOS_PORT);

 	enable_irq(RTC_VEC);
}


/*
* int rtc_open()
*   Inputs: none
*   Return Value: 0
*	Function: sets 1024 as frequency and returns 0
*/
int rtc_open(const int8_t * filename) {

	char prev;
    int rate = DEFAULT_RATE;				//set rate
    outb(STATUS_REGISTER_A, RTC_PORT); // set index to register A, disable NMI
	prev = inb(CMOS_PORT);				// get initial value of register A
	outb(STATUS_REGISTER_A, RTC_PORT);		// reset index to A
	outb((prev & 0xF0) | rate, CMOS_PORT); 	// write only our rate to A. Rate is 1024

    //return 0;
    outb(STATUS_REGISTER_C, RTC_PORT);	// select register C
	inb(CMOS_PORT);

    return 0;
}

/*
* int rtc_close()
*   Inputs: none
*   Return Value: 0
*	Function: return 0, for close
*/
int rtc_close(int32_t fd) {
    return 0;
}

/*
* int rtc_read()
*   Inputs: none
*   Return Value: 0
*	Function: read 0;
*/

int rtc_read(int32_t fdr, void* buf, int32_t nbytes) {
	read_flag = 0;
	sti();
	while (!read_flag) { }
	cli();
		//printf("a");
	return 0;
}


/*
* void rtc_write()
*   Inputs: none
*   Return Value: none
*	Function: sets rate by arg
*/
int rtc_write(int32_t arg, const void* buf, int32_t nbytes) {

	char prev;
     int rate;				// Rate is between x06 and x0F
		switch(*(int32_t*)buf)
		{
			// depending on the frequency we change the rate value
			case 2: rate = DEFAULT_RATE; break;
			case 4: rate = 0x0E; break;
			case 8: rate = 0x0D; break;
			case 16: rate = 0x0C; break;
			case 32: rate = 0x0B; break;
			case 64: rate = 0x0A; break;
			case 128: rate = 0x09; break;
			case 256: rate = 0x08; break;
			case 512: rate = 0x07; break;
			case 1024: rate = 0x06; break;
			default: return -1;
		}
  outb(STATUS_REGISTER_A, RTC_PORT); // set index to register A, disable NMI
	prev = inb(CMOS_PORT);				// get initial value of register A
	outb(STATUS_REGISTER_A, RTC_PORT);		// reset index to A
	outb((prev & 0xF0) | rate, CMOS_PORT); 	// write only our rate to A. Rate is 1024

	//
	// outb(STATUS_REGISTER_B, RTC_PORT);		// select register B, and disable NMI
	// prev = inb(CMOS_PORT);					// read the current value of register B
	// outb(STATUS_REGISTER_B, RTC_PORT);		// set the index again (a read will reset the index to register D)
	// outb((prev | 0x40), CMOS_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B


    //return 0;
  outb(STATUS_REGISTER_C, RTC_PORT);	// select register C
	inb(CMOS_PORT);
	return 0;
}


/*
* void rtc_handler()
*   Inputs: none
*   Return Value: none
*	Function: sets read_flag to 1, checks rtc interrupt
*/
void rtc_handler() {
			//printf("a\n");
      outb(0x0C, RTC_PORT );
			// inb(STATUS_REGISTER_C);	// select register C
      inb(CMOS_PORT);
      send_eoi(RTC_VEC);
			read_flag=1;


}
