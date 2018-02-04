/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
#define FULL_MASK 0xFF
#define SLAVE_IRQ_OFF 8

/*
* void i8259_init()
*   Inputs: void
*   Return Value: void
*	Function: sets up the i8259 chip to handle interrupts
*/
void
i8259_init(void)
{
    //unsigned long flags;
    //cli_and_save(flags);

    /* Set control words for Master PIC */
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_8259_PORT_A);
    outb(ICW3_MASTER, MASTER_8259_PORT_A);
    outb(ICW4, MASTER_8259_PORT_A);

    /* Set control words for Slave PIC */
    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_8259_PORT_A);
    outb(ICW3_SLAVE, SLAVE_8259_PORT_A);
    outb(ICW4, SLAVE_8259_PORT_A);

    outb(FULL_MASK, MASTER_8259_PORT_A);
    outb(FULL_MASK, SLAVE_8259_PORT_A);

    enable_irq(ICW3_SLAVE);
    //restore_flags(flags);
}

/*
* void enable_irq()
*   Inputs: irq_num -- the interrupt request number
*   Return Value: none
*	Function: enables the interrupt given by irq_num on the PIC
*/
void
enable_irq(uint32_t irq_num)
{
    /* IRQ 0-7 is master, 8-15 is slave */
     uint16_t port;
     uint8_t value;

     if(irq_num < 8) {
         port = MASTER_8259_PORT_A;
     } else {
         port = SLAVE_8259_PORT_A;
         irq_num -= 8;
     }

     value = inb(port) ^ (1 << irq_num);
     outb(value, port);
}


/*
* void disable_irq()
*   Inputs: irq_num -- the interrupt request number
*   Return Value: none
*	Function: disables the interrupt given by irq_num on the PIC
*/
void
disable_irq(uint32_t irq_num)
{
   uint16_t port;
   uint8_t value;

   if(irq_num < SLAVE_IRQ_OFF) {
       port = MASTER_8259_PORT_A;
   } else {
       port = SLAVE_8259_PORT_A;
       irq_num -= SLAVE_IRQ_OFF;
   }
   value = inb(port) ^ ~(1 << irq_num);
   outb(port, value);
}

/*
* void send_eoi()
*   Inputs: irq_num -- the interrupt request number
*   Return Value: none
*	Function: sends eoi signal to PIC after interrupt has been serviced
*/
void
send_eoi(uint32_t irq_num)
{
    if (irq_num < SLAVE_IRQ_OFF) {
        outb(EOI|irq_num, MASTER_8259_PORT);
    } else if (irq_num < 16) {
        outb(EOI|(irq_num-SLAVE_IRQ_OFF), SLAVE_8259_PORT);
        outb((EOI|ICW3_SLAVE), MASTER_8259_PORT);
    }
}
