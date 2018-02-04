#ifndef IDT_H
#define IDT_H

#include "x86_desc.h"
#include "lib.h"
#include "types.h"
#include "syscall.h"

void idt_init();
void divide_error();
void debug();
void NMI();
void breakpoint();
void overflow();
void BOUND_range_exceeded();
void invalid_opcode();
void device_not_available();
void double_fault();
void coprocessor_segment_overrun();
void invalid_TSS();
void segment_not_present();
void stack_fault();
void general_protection();
void page_fault();
void x87_FPU_floatingpoint();
void alignment_check();
void machine_check();
void SIMD_floatingpoint();
void keyboard_wrapper();
void rtc_wrapper();

#endif
