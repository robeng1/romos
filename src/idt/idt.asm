section .asm ; Defines the section of the assembly code

global load_idt ; Makes the symbol "load_idt" globally accessible
global enable_interrupts
global disable_interrupts

enable_interrupts:
    sti ; Enable interrupts by setting the interrupt flag (IF) in the EFLAGS register
    ret ; Return from the function, popping the return address from the stack and transferring control back

disable_interrupts:
    cli ; Disable interrupts by clearing the interrupt flag (IF) in the EFLAGS register
    ret ; Return from the function, popping the return address from the stack and transferring control back

load_idt: ; Start of the "load_idt" function
    push ebp ; Preserve the value of the base pointer (ebp) by pushing it onto the stack
    mov ebp, esp ; Set up a new base pointer (ebp) by copying the current stack pointer (esp)

    mov ebx, [ebp+8] ; Move the value at [ebp+8] (first function argument) into the ebx register
    lidt [ebx] ; Load the Interrupt Descriptor Table (IDT) using the value in ebx

    pop ebp ; Restore the previous base pointer value by popping it from the stack
    ret ; Return from the function, popping the return address from the stack and transferring control back
