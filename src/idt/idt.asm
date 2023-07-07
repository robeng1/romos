section .asm ; Defines the section of the assembly code

extern no_interrupt_handler
extern isr80h_handler
extern interrupt_handler

global load_idt
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper
global interrupt_pointer_table

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

no_interrupt:
    pushad
    call no_interrupt_handler
    popad
    iret

%macro interrupt 1
    global int%1
    int%1:
        ; INTERRUPT FRAME START
        ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT
        ; uint32_t ip
        ; uint32_t cs;
        ; uint32_t flags
        ; uint32_t sp;
        ; uint32_t ss;
        ; Pushes the general purpose registers to the stack
        pushad
        ; Interrupt frame end
        push esp
        push dword %1
        call interrupt_handler
        add esp, 8
        popad
        iret
%endmacro

%assign i 0
%rep 512
    interrupt i
%assign i i+1
%endrep

isr80h_wrapper:
    ; INTERRUPT FRAME START
    ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT
    ; uint32_t ip
    ; uint32_t cs;
    ; uint32_t flags
    ; uint32_t sp;
    ; uint32_t ss;
    ; Pushes the general purpose registers to the stack
    pushad
    
    ; INTERRUPT FRAME END

    ; Push the stack pointer so that we are pointing to the interrupt frame
    push esp

    ; EAX holds our command lets push it to the stack for isr80h_handler
    push eax
    call isr80h_handler
    mov dword[tmp_res], eax
    add esp, 8

    ; Restore general purpose registers for user land
    popad
    mov eax, [tmp_res]
    iretd

section .data
; Inside here is stored the return result from isr80h_handler
tmp_res: dd 0


%macro interrupt_array_entry 1
    dd int%1
%endmacro

interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i
%assign i i+1
%endrep