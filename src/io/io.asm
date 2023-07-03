; This code defines four functions (read_byte, read_word, write_byte, write_word) that perform I/O operations based on the cdecl calling convention. 
; The code follows the standard function prologue and epilogue, preserving the base pointer (ebp) and restoring it before returning from each function. 
; The I/O operations involve reading or writing from ports specified by the function arguments ([ebp+8] and [ebp+12]).
section .asm ; Defines the section of the assembly code

global read_byte
global read_word
global write_byte
global write_word

read_byte:
    push ebp ; Preserve the value of the base pointer (ebp) by pushing it onto the stack
    mov ebp, esp ; Set up a new base pointer (ebp) by copying the current stack pointer (esp)

    xor eax, eax ; Clear the EAX register by XORing it with itself
    mov edx, [ebp+8] ; Move the value at [ebp+8] (first function argument) into the edx register
    in al, dx ; Read a byte from the port specified by the value in edx and store it in the low 8 bits of eax

    pop ebp ; Restore the previous base pointer value by popping it from the stack
    ret ; Return from the function, popping the return address from the stack and transferring control back

read_word:
    push ebp ; Preserve the value of the base pointer (ebp) by pushing it onto the stack
    mov ebp, esp ; Set up a new base pointer (ebp) by copying the current stack pointer (esp)

    xor eax, eax ; Clear the EAX register by XORing it with itself
    mov edx, [ebp+8] ; Move the value at [ebp+8] (first function argument) into the edx register
    in ax, dx ; Read a word from the port specified by the value in edx and store it in the ax register

    pop ebp ; Restore the previous base pointer value by popping it from the stack
    ret ; Return from the function, popping the return address from the stack and transferring control back

write_byte:
    push ebp ; Preserve the value of the base pointer (ebp) by pushing it onto the stack
    mov ebp, esp ; Set up a new base pointer (ebp) by copying the current stack pointer (esp)

    mov eax, [ebp+12] ; Move the value at [ebp+12] (second function argument) into the eax register
    mov edx, [ebp+8] ; Move the value at [ebp+8] (first function argument) into the edx register
    out dx, al ; Write the low 8 bits of eax to the port specified by the value in edx

    pop ebp ; Restore the previous base pointer value by popping it from the stack
    ret ; Return from the function, popping the return address from the stack and transferring control back

write_word:
    push ebp ; Preserve the value of the base pointer (ebp) by pushing it onto the stack
    mov ebp, esp ; Set up a new base pointer (ebp) by copying the current stack pointer (esp)

    mov eax, [ebp+12] ; Move the value at [ebp+12] (second function argument) into the eax register
    mov edx, [ebp+8] ; Move the value at [ebp+8] (first function argument) into the edx register
    out dx, ax ; Write the 16 bits of eax to the port specified by the value in edx

    pop ebp ; Restore the previous base pointer value by popping it from the stack
    ret ; Return from the function, popping the return address from the stack and transferring control back
