[BITS 32]

section .asm

global print:function
global sys_getkey:function
global sys_malloc:function
global sys_free:function
global sys_putchar:function
global sys_process_load_start:function
global sys_process_get_arguments:function 
global sys_system:function
global sys_exit:function

print:
    push ebp
    mov ebp, esp
    push dword[ebp+8]
    mov eax, 0 ; Command print
    int 0x80
    add esp, 4
    pop ebp
    ret

sys_getkey:
    push ebp
    mov ebp, esp
    mov eax, 1 ; Command getkey
    int 0x80
    pop ebp
    ret

sys_putchar:
    push ebp
    mov ebp, esp
    mov eax, 2 ; Command putchar
    push dword [ebp+8] ; Variable "c"
    int 0x80
    add esp, 4
    pop ebp
    ret

sys_malloc:
    push ebp
    mov ebp, esp
    mov eax, 3 ; Command malloc (Allocates memory for the process)
    push dword[ebp+8] ; Variable "size"
    int 0x80
    add esp, 4
    pop ebp
    ret

sys_free:
    push ebp
    mov ebp, esp
    mov eax, 4 ; Command 4 free (Frees the allocated memory for this process)
    push dword[ebp+8] ; Variable "ptr"
    int 0x80
    add esp, 4
    pop ebp
    ret

sys_process_load_start:
    push ebp
    mov ebp, esp
    mov eax, 5 ; Command 5 process load start ( stars a process )
    push dword[ebp+8] ; Variable "filename"
    int 0x80
    add esp, 4
    pop ebp
    ret

sys_system:
    push ebp
    mov ebp, esp
    mov eax, 6 ; Command 6 process_system ( runs a system command based on the arguments)
    push dword[ebp+8] ; Variable "arguments"
    int 0x80
    add esp, 4
    pop ebp
    ret

sys_process_get_arguments:
    push ebp
    mov ebp, esp
    mov eax, 7 ; Command 7 Gets the process arguments
    push dword[ebp+8] ; Variable arguments
    int 0x80
    add esp, 4
    pop ebp
    ret

sys_exit:
    push ebp
    mov ebp, esp
    mov eax, 8 ; Command 8 process exit
    int 0x80
    pop ebp
    ret