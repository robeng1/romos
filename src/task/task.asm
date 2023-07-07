[BITS 32]
section .asm

global restore_registers_state_t
global task_return
global user_registers

; This function is used to return from a task switch.
; It takes a pointer to a registers_state_t structure, which contains the saved state of the task.
task_return:
    mov ebp, esp  ; Save the current stack pointer
    ; The next few instructions are preparing for an iretd instruction, which will return from an interrupt and restore the CPU state.
    ; iretd expects to find the values to restore on the stack in a specific order: SS, ESP, EFLAGS, CS, EIP.

    mov ebx, [ebp+4]  ; Load the pointer to the registers_t structure into EBX
    push dword [ebx+44]  ; Push the SS value onto the stack
    push dword [ebx+40]  ; Push the ESP value onto the stack

    pushf  ; Push the current EFLAGS value onto the stack
    pop eax  ; Pop it into EAX
    or eax, 0x200  ; Set the IF flag (this enables interrupts)
    push eax  ; Push the modified EFLAGS value back onto the stack

    push dword [ebx+32]  ; Push the CS value onto the stack
    push dword [ebx+28]  ; Push the EIP value onto the stack

    ; Set up the data segment registers
    mov ax, [ebx+44]  ; Load the SS value into AX
    mov ds, ax  ; Set DS to the SS value
    mov es, ax  ; Set ES to the SS value
    mov fs, ax  ; Set FS to the SS value
    mov gs, ax  ; Set GS to the SS value

    push dword [ebp+4]  ; Push the pointer to the registers_t structure onto the stack
    call restore_registers_state_t  ; Call the function to restore the general-purpose registers
    add esp, 4  ; Clean up the stack

    iretd  ; Return from the interrupt, which restores the CPU state and switches to the task

; This function restores the general-purpose registers from a registers_state_t structure.
; It takes a pointer to a registers_state_t structure as an argument.
restore_registers_state_t:
    push ebp  ; Save the current base pointer
    mov ebp, esp  ; Set the base pointer to the current stack pointer
    mov ebx, [ebp+8]  ; Load the pointer to the registers_t structure into EBX
    ; The next few instructions restore the general-purpose registers from the structure.
    mov edi, [ebx]  ; Restore EDI
    mov esi, [ebx+4]  ; Restore ESI
    mov ebp, [ebx+8]  ; Restore EBP
    mov edx, [ebx+16]  ; Restore EDX
    mov ecx, [ebx+20]  ; Restore ECX
    mov eax, [ebx+24]  ; Restore EAX
    mov ebx, [ebx+12]  ; Restore EBX
    add esp, 4  ; Clean up the stack
    ret  ; Return from the function

; This function sets the data segment registers to a specific value.
user_registers:
    mov ax, 0x23  ; Load the value 0x23 into AX
    ; The next few instructions set the data segment registers to the value in AX.
    mov ds, ax  ; Set DS to the value in AX
    mov es, ax  ; Set ES to the value in AX
    mov fs, ax  ; Set FS to the value in AX
    mov gs, ax  ; Set GS to the value in AX
    ret  ; Return from the function
