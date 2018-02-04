.text

.globl keyboard_wrapper
.globl rtc_wrapper
.globl pit_wrapper
.globl loadPageDirectory
.globl enablePaging
.globl flushTLB
.globl retToPar
.globl contextSwitch
.globl Save
.globl processSwitch
.globl syscall_link
# .globl retToParSaveAll

# saves regs, calls keyboard handler code, and pops regs
keyboard_wrapper:
    pushal
    call keyboard_handler
    popal
    iret

# saves regs, calls rtc handler code, and pops regs
 rtc_wrapper:
    pushal
    call rtc_handler
    popal
    iret

pit_wrapper:
   pushal
   call pit_handler
   popal
   iret

# enables paging by setting cr registers
enablePaging:
    pushl %ebp
    movl %esp, %ebp

    movl 8(%ebp), %eax
    movl %eax,  %cr3

    movl %cr4, %eax
    orl $0x00000010, %eax
    movl %eax, %cr4

    movl %cr0, %eax
    orl $0x80000001, %eax
    movl %eax, %cr0

    movl %ebp, %esp
    popl %ebp
    ret

# Flushes tlb for context switching
flushTLB:
  movl %cr3, %eax
  movl %eax, %cr3
  ret

retToPar:
    movl 12(%esp), %eax
    movl 8(%esp), %ebp
    movl 4(%esp), %esp
    ret

# retToParSaveAll:
#     movl 12(%esp), %eax
#     movl 8(%esp), %ebp
#     movl 4(%esp), %esp
#     ret

Save:
     movl 4(%esp), %ecx
     movl %esp, (%ecx)
     movl 8(%esp), %ecx
     movl %ebp, (%ecx)
     ret

processSwitch:
  movl 16(%esp), %eax
  movl %esp, (%eax)
  movl 12(%esp), %eax
  movl %ebp, (%eax)
  movl 4(%esp), %ebp
  movl 8(%esp), %esp
  ret

# to user mode
contextSwitch:
     movl 8(%esp), %ecx
     movl %esp, (%ecx)
     movl 12(%esp), %ecx
     movl %ebp, (%ecx)

     movl 4(%esp), %ecx
     pushl $0x2B
     movl %esp, %eax
     pushl $138412032
     pushf
     popl %ebx
     orl $0x200, %ebx
     push %ebx
     pushl $0x23
     pushl %ecx
     movl $0x2B, %ecx
     movw %cx, %ds
     iret

# system call linkage, dispatcher function
syscall_link:
  # pushal
  decl %eax
  cmpl $10, %eax
  jae err
  call *jump_table(, %eax, 4)
  iret

err:
  movl $-1, %eax
  iret

 #  Jump Table for system calls, halt, execute, etc
jump_table:
    .long halt_handler, execute_handler, read_handler, write_handler, open_handler, close_handler, getargs_handler, vidmap_handler, set_handler_handler, sigreturn_handler


getargs_handler:
  pushl %ecx
  pushl %ebx
  call getargs
  popl %ebx
  popl %ecx
  ret


vidmap_handler:
  pushl %ebx
  call vidmap
  popl %ebx
  ret


set_handler_handler:
  pushl %ecx
  pushl %ebx
  call set_handler
  popl %ebx
  popl %ecx
  ret

sigreturn_handler:
  call sigreturn
  ret

# System call halt
halt_handler:
  pushl %ebx
  call halt
  popl %ebx
  ret

# System call execute
execute_handler:
  pushl %ebx
  call execute
  popl %ebx
  ret

# System call read
read_handler:
  pushl %edx
  pushl %ecx
  pushl %ebx
  call read
  popl %ebx
  popl %ecx
  popl %edx
  ret

# System call write
write_handler:
  pushl %edx
  pushl %ecx
  pushl %ebx
  call write
  popl %ebx
  popl %ecx
  popl %edx
  ret

# System call open
open_handler:
  pushl %ebx
  call open
  popl %ebx
  ret

# System call close
close_handler:
  pushl %ebx
  call close
  popl %ebx
  ret
