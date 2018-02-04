#include "scheduling.h"

int indicator = 1;
int fts_1 = 0;
int fts_2 = 0;
int fts_3 = 0;
int active = 1;

static int screen1x = 0;
static int screen1y = 0;

static int screen2x = 0;
static int screen2y = 0;

static int screen3x = 0;
static int screen3y = 0;

static int tbuf1 = 0;
static int tbuf2 = 0;
static int tbuf3 = 0;

static char buffer1[MAX_BYTES_BUF];
static char buffer2[MAX_BYTES_BUF];
static char buffer3[MAX_BYTES_BUF];

process_t * head = NULL;

/*
* void pSwitch()
*   Inputs: none
*   Return Value: none
*	Function: Switches between active processes
*/
void pSwitch(){
	process_t * old = head;
	head = head->next;
	pnum = head->pcb->pid;
	setupPage();

  tss.esp0 = head->pcb->oldesp;
	processSwitch((head->pcb->oldebp),head->pcb->oldesp, &(old->pcb->oldebp), &(old->pcb->oldesp));
}

/*
* void init_screen_memory()
*   Inputs: none
*   Return Value: none
*	Function: Sets up PDT for three terminals and starts the first terminal
*/
void init_screen_memory(){
	setupPagingDirectoryTable(RESERV_CODE_2, VIDEO,1);
	setupPagingDirectoryTable(RESERV_CODE_2, SCREEN2_MEM,2);
	setupPagingDirectoryTable(RESERV_CODE_2, SCREEN3_MEM,3);
	screen1();
}

/*
* void screen1()
*   Inputs: none
*   Return Value: none
*	Function: ALT_F1 takes you to first terminal, switching video memory
*/
void screen1() {
	// printf("\nscreen 1\n");
	cli();
	getCursorandBuffer();
	copyFromVideo();
	indicator = 1;
	setCursorandBuffer();
	update_cursor(screen1x, screen1y);
	// printf("\nindicator:%d fts_1:%d \n", indicator, fts_1);
	if(!fts_1){
		// printf("here2");
		// active++;
		fts_1 = 1;

		reset_screen();
		setupPagingDirectoryTable(RESERV_CODE_2, VIDEO, indicator);
		flushTLB();
		execute("shell");
		return;
		// printf("here");
	}
	// setupPagingDirectoryTable(RESERV_CODE_2, VIDEO, indicator);
	copyToVideo();

	flushTLB();
	sti();
}

/*
* void screen2()
*   Inputs: none
*   Return Value: none
*	Function: ALT_F2 takes you to second terminal, switching video memory
*/
void screen2(){
	// printf("\nhi from screen 2\n");
	cli();
	getCursorandBuffer();
	copyFromVideo();
	indicator = 2;
	setCursorandBuffer();
	update_cursor(screen2x, screen2y);
	if(!fts_2){
		// active++;
		fts_2 = 1;
		reset_screen();
		setupPagingDirectoryTable(RESERV_CODE_2, VIDEO, indicator);
		flushTLB();
		execute("shell");
		return;
	}
	// setupPagingDirectoryTable(RESERV_CODE_2, VIDEO, indicator);
	copyToVideo();

	flushTLB();
	sti();
}

/*
* void screen3()
*   Inputs: none
*   Return Value: none
*	Function: ALT_F3 takes you to second terminal, switching video memory
*/
void screen3(){
	// printf("\nhi from screen 3\n");
	cli();
	getCursorandBuffer();
	copyFromVideo();
	indicator = 3;
	setCursorandBuffer();
	update_cursor(screen3x, screen3y);
	if(!fts_3){
		// active++;
		fts_3 = 1;
		reset_screen();
		setupPagingDirectoryTable(RESERV_CODE_2, VIDEO, indicator);
		flushTLB();
		execute("shell");
		return;
	}
	// setupPagingDirectoryTable(RESERV_CODE_2, VIDEO, indicator);
	copyToVideo();

	flushTLB();
	sti();

}

/*
* void copyToVideo()
*   Inputs: none
*   Return Value: none
*	Function: Swaps indicator pages into video memory
*/
void copyToVideo(){
	switch (indicator) {
		case 1 :
			// setupPagingDirectoryTable(RESERV_CODE_2, SCREEN1_MEM, indicator);
			memcpy((char *)VIDEO,(char *)SCREEN1_VIR,SIZE_VID_MEM);
	        break;
	    case 2 :
	    	// setupPagingDirectoryTable(RESERV_CODE_2, SCREEN2_MEM, indicator);

	    	memcpy((char *)VIDEO,(char *)SCREEN2_VIR,SIZE_VID_MEM);
	        break;
	    case 3 :
	    	// setupPagingDirectoryTable(RESERV_CODE_2, SCREEN3_MEM, indicator);
	     	memcpy((char *)VIDEO,(char *)SCREEN3_VIR,SIZE_VID_MEM);
	        break;
	}
	setupPagingDirectoryTable(RESERV_CODE_2, VIDEO, indicator);
}

/*
* void copyFromVideo()
*   Inputs: none
*   Return Value: 0
*	Function: Swaps indicator page from video
*/
void copyFromVideo(){
	switch (indicator) {
		case 1 :
			setupPagingDirectoryTable(RESERV_CODE_2, SCREEN1_MEM, indicator);
			memcpy((char *)SCREEN1_VIR,(char *)VIDEO,SIZE_VID_MEM);
	        break;
	    case 2 :
	    	setupPagingDirectoryTable(RESERV_CODE_2, SCREEN2_MEM, indicator);
	    	memcpy((char *)SCREEN2_VIR,(char *)VIDEO,SIZE_VID_MEM);
	        break;
	    case 3 :
	    	setupPagingDirectoryTable(RESERV_CODE_2, SCREEN3_MEM, indicator);
	     	memcpy((char *)SCREEN3_VIR,(char *)VIDEO,SIZE_VID_MEM);
	        break;
	}

}

/*
* void setCursorandBuffer()
*   Inputs: none
*   Return Value: none
*	Function: The screen_x and screen_y values used by lib.c is updated, buffer used by terminal read is also updated
*/
void setCursorandBuffer(){
	switch (indicator) {
		case 1 :
			screen_x = screen1x;
			screen_y = screen1y;
			tbuf = tbuf1;
			memcpy(tbuffer,buffer1,MAX_BYTES_BUF);
			// int c;
			// for (c = 0; c < 20; ++c)
			// {
			// 	tbuffer[c] = buffer1[c];
			// }
			// printf("tbuffer:%s buffer1:%s indicator: %d", tbuffer, buffer1, indicator);
	        break;
	    case 2 :
	    	screen_x = screen2x;
			screen_y = screen2y;
			tbuf = tbuf2;
			memcpy(tbuffer,buffer2,MAX_BYTES_BUF);
	        break;
	    case 3 :
	     	screen_x = screen3x;
			screen_y = screen3y;
			tbuf = tbuf3;
			memcpy(tbuffer,buffer3,MAX_BYTES_BUF);
	        break;
	}
}

/*
* void getCursorandBuffer()
*   Inputs: none
*   Return Value: none
*	Function: Terminal buffer and x,y location are saved into locally svaed cursor x and y positions, whatever is in buffer on screen is also saved
*/
void getCursorandBuffer(){
	switch (indicator) {
		case 1 :
			screen1x = screen_x;
			screen1y = screen_y;
			tbuf1 = tbuf;
			memcpy(buffer1,tbuffer,MAX_BYTES_BUF);
			// printf("\nbuffer1: %s\n", buffer1);
			// int c;
			// for (c = 0; c < 20; ++c)
			// {
			// 	if(buffer1[c] == '\n') printf("\nBuffer1:Bensin\n");
			// 	else if(buffer1[c] == ' ') printf("\nBuffer1:Benson\n");
			// 	else {
			// 		// printf("\nBuffer1:");
			// 		putc(buffer1[c]);
			// 		// printf("\n");
			// 	}
			// }
	        break;
	    case 2 :
	    	screen2x = screen_x;
			screen2y = screen_y;
			tbuf2 = tbuf;
			memcpy(buffer2,tbuffer,MAX_BYTES_BUF);
						// printf("\nbuffer2: %s\n", buffer2);
	        break;
	    case 3 :
	     	screen3x = screen_x;
			screen3y = screen_y;
			tbuf3 = tbuf;
			memcpy(buffer3,tbuffer,MAX_BYTES_BUF);
					// printf("\nbuffer3: %s\n", buffer3);
	        break;
	}
}
