#include "pit.h"

int quantum = 0;

/*
* void pit_init()
*   Inputs: none
*   Return Value: 0
* Function: enables irq on the PIC
*/
void pit_init(){
    enable_irq(0);
}

/*
* void pit_write(int freq)
*   Inputs: int freq
*   Return Value: 0
* Function: sets frequency at which pit_handler is called
*/
void pit_write(int freq){
  int divisor = MAXPIT / freq;       /* Calculate our divisor */
  outb(0x36, 0x43);             /* Set our command byte 0x36 */
  outb(divisor & 0xFF, 0x40);   /* Set low byte of divisor */
  outb(divisor >> 8, 0x40);     /* Set high byte of divisor */
}

/*
* void pit_handler()
*   Inputs: none
*   Return Value: none
* Function: calls process switch at each quantum
*/
void pit_handler(){
  send_eoi(0);
  quantum++;
  if(head!=NULL && head->next != head){
    // pSwitch();
  // processSwitch(&pSwitch,&(head->pcb->oldesp),&(head->pcb->oldebp));
  }
}
