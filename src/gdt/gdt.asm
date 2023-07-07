[BITS 32]  ; Set the assembly code to be executed in 32-bit mode
section .asm    ; This directive switches to the .asm section of memory, where assembly code is typically stored.

; This directive declares that the gdt_load label is a global label, 
; making it visible to the linker so it can be used from other files.
global gdt_load 

gdt_load:  ; This is a label, which represents the start of the gdt_load function in memory.
    ; Move the value at memory location (esp + 4) into the AX register.
    ; This is typically the first argument passed to the function.
    mov   ax, [esp + 4]

    ; Move the value in AX into the memory location labeled gdt_descriptor.
    mov   [gdt_descriptor], ax

    ; Move the value at memory location (esp + 8) into the EAX register.
    ; This is typically the second argument passed to the function.
    mov   eax, [esp + 8]

    ; Move the value in EAX into the memory location labeled (gdt_descriptor + 2).
    mov   [gdt_descriptor + 2], eax

    ; Load the Global Descriptor Table (GDT) using the address stored in gdt_descriptor.
    lgdt  [gdt_descriptor]

    ; Return from the function.
    ret

section .data  ; This directive switches to the .data section of memory, where global variables and static data are typically stored.

gdt_descriptor:  ; This is a label, which represents the start of the gdt_descriptor structure in memory.
    ; Size: Reserve a 2-byte space in memory (double word) initialized with 0. 
    ; This represents the size of the GDT.
    dw 0x00  

    ; GDT Start Address: Reserve a 4-byte space in memory (double dword) initialized with 0. 
    ; This represents the base address of the GDT.
    dd 0x00  
