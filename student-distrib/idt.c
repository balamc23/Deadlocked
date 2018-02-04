#include "idt.h"
#include "keyboard.h"
#include "rtc.h"
//#include "x86_desc.h"

/*
* void idt_init()
*   Inputs: none
*   Return Value: none
*	Function: sets up the idt
*/
void idt_init(){
  int i;

  /*
    Making dpl = 0 for exceptions 0-19
  */
  for (i = 0;i<NUM_VEC;i++){
    if(i<15 || (i>15 &&i<20) || i == 33 || i == 0x80 || i==0x28 || i == 0x20){
      idt[i].present = 1;
    }
    else{
      idt[i].present = 0;
    }
    if(i<45){
      idt[i].dpl = 0;
      idt[i].reserved3 = 0;
    }
    else{
      idt[i].dpl = 3;
      idt[i].reserved3 = 1;
    }
    idt[i].reserved0 = 0;
    idt[i].size = 1;
    idt[i].reserved1 = 1;
    idt[i].reserved2 = 1;
    idt[i].reserved4 = 0;
    idt[i].seg_selector = KERNEL_CS;
  }

  // idt[0x80].dpl = 0x03;
  SET_IDT_ENTRY(idt[0],divide_error);
  SET_IDT_ENTRY(idt[1],debug);
  SET_IDT_ENTRY(idt[2],NMI);
  SET_IDT_ENTRY(idt[3],breakpoint);
  SET_IDT_ENTRY(idt[4],overflow);
  SET_IDT_ENTRY(idt[5],BOUND_range_exceeded);
  SET_IDT_ENTRY(idt[6],invalid_opcode);
  SET_IDT_ENTRY(idt[7],device_not_available);
  SET_IDT_ENTRY(idt[8],double_fault);
  SET_IDT_ENTRY(idt[9],coprocessor_segment_overrun);
  SET_IDT_ENTRY(idt[10],invalid_TSS);
  SET_IDT_ENTRY(idt[11],segment_not_present);
  SET_IDT_ENTRY(idt[12],stack_fault);
  SET_IDT_ENTRY(idt[13],general_protection);
  SET_IDT_ENTRY(idt[14],page_fault);
  SET_IDT_ENTRY(idt[16],x87_FPU_floatingpoint);
  SET_IDT_ENTRY(idt[17],alignment_check);
  SET_IDT_ENTRY(idt[18],machine_check);
  SET_IDT_ENTRY(idt[19],SIMD_floatingpoint);
  SET_IDT_ENTRY(idt[33],keyboard_wrapper);
  SET_IDT_ENTRY(idt[0x20],pit_wrapper);
  SET_IDT_ENTRY(idt[0x28],rtc_wrapper);
  SET_IDT_ENTRY(idt[0x80],syscall_link);


  lidt(idt_desc_ptr);
}

/*
* void divide_error()
*   Inputs: none
*   Return Value: none
*	Function: prints out that divide by 0 error has occurred
*/
void divide_error(){
  printf("Divide by 0 Error?\n");
  halt(0);
}

/*
* void debug()
*   Inputs: none
*   Return Value: none
*	Function: prints out that debug exception has occurred
*/
void debug(){
  printf("Debug Exception?\n");
  halt(0);
}

/*
* void NMI()
*   Inputs: none
*   Return Value: none
*	Function: prints out that NMI exception has occurred
*/
void NMI(){
  printf("NMI Interrupt?\n");
  halt(0);
}

/*
* void breakpoint()
*   Inputs: none
*   Return Value: none
*	Function: prints out that breakpoint exception has occurred
*/
void breakpoint(){
  printf("Breakpoint Exception?\n");
  halt(0);
}

/*
* void overflow()
*   Inputs: none
*   Return Value: none
*	Function: prints out that overflow exception has occurred
*/
void overflow(){
  printf("Overflow Exception?\n");
  halt(0);
}

/*
* void BOUND_range_exceeded()
*   Inputs: none
*   Return Value: none
*	Function: prints out that BOUND range exceeded exception has occurred
*/
void BOUND_range_exceeded(){
  printf("BOUND Range Exceeded Exception?\n");
  halt(0);
}

/*
* void invalid_opcode()
*   Inputs: none
*   Return Value: none
*	Function: prints out that invalid opcode exception has occurred
*/
void invalid_opcode(){
  printf("Invalid Opcode Exception?\n");
  halt(0);
}

/*
* void device_not_available()
*   Inputs: none
*   Return Value: none
*	Function: prints out that device not available exception has occurred
*/
void device_not_available(){
  printf("Device Not Available Exception?\n");
  halt(0);
}

/*
* void double_fault()
*   Inputs: none
*   Return Value: none
*	Function: prints out that double_fault exception has occurred
*/
void double_fault(){
  printf("Double Fault Exception?\n");
  halt(0);
}

/*
* void coprocessor_segment_overrun()
*   Inputs: none
*   Return Value: none
*	Function: prints out that coprocessor segment has been overrun has occurred
*/
void coprocessor_segment_overrun(){
  printf("Coprocessor Segment Overrun?\n");
  halt(0);
}

/*
* void invalid_TSS()
*   Inputs: none
*   Return Value: none
*	Function: prints out that invalid TSS exception has occurred
*/
void invalid_TSS(){
  printf("Invalid TSS Exception?\n");
  halt(0);
}

/*
* void segment_not_present()
*   Inputs: none
*   Return Value: none
*	Function: prints out that segment is not present has occurred
*/
void segment_not_present(){
  printf("Segment Not Present?\n");
  halt(0);
}

/*
* void stack_fault()
*   Inputs: none
*   Return Value: none
*	Function: prints out that stack fault exception has occurred
*/
void stack_fault(){
  printf("Stack Fault Exception?\n");
  halt(0);
}

/*
* void general_protection()
*   Inputs: none
*   Return Value: none
*	Function: prints out that general protection exception has occurred
*/
void general_protection(){
  printf("General Protection Exception?\n");
  halt(0);
}

/*
* void page_fault()
*   Inputs: none
*   Return Value: none
*	Function: prints out that page_fault exception has occurred
*/
void page_fault(){
  printf("Page-Fault Exception?\n");
  halt(0);
}

/*
* void x87_FPU_floatingpoint()
*   Inputs: none
*   Return Value: none
*	Function: prints out that floating point error has occurred
*/
void x87_FPU_floatingpoint(){
  printf("Floating Point Error\n");
  halt(0);
}

/*
* void alignment_check()
*   Inputs: none
*   Return Value: none
*	Function: prints out that alignment check exception has occurred
*/
void alignment_check(){
  printf("Alignment Check Exception?\n");
  halt(0);
}

/*
* void machine_check()
*   Inputs: none
*   Return Value: none
*	Function: prints out that machine check exception has occurred
*/
void machine_check(){
  printf("Machine Check Exception?\n");
  halt(0);
}

/*
* void SIMD_floatingpoint()
*   Inputs: none
*   Return Value: none
*	Function: prints out that floating point exception has occurred
*/
void SIMD_floatingpoint(){
  printf("Floating Point Exception?\n");
  halt(0);
}
