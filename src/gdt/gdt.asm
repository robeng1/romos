section .asm    ; This directive switches to the .asm section of memory, where assembly code is typically stored.

global gdt_load ; This directive declares that the gdt_load label is a global label, making it visible to the linker so it can be used from other files.

gdt_load:  ; This is a label, which represents the start of the gdt_load function in memory.
    mov eax, [esp+4]  ; Move the value at memory address (esp + 4) into the eax register. This is typically the first argument to the function - the base address of the GDT.
    mov [gdt_descriptor + 2], eax  ; Move the value in eax (the base address of the GDT) into memory address (gdt_descriptor + 2). This sets the base address of the GDT in the gdt_descriptor structure.
    mov ax, [esp+8]  ; Move the value at memory address (esp + 8) into the ax register. This is typically the second argument to the function - the size of the GDT.
    mov [gdt_descriptor], ax  ; Move the value in ax (the size of the GDT) into the memory at address gdt_descriptor. This sets the size of the GDT in the gdt_descriptor structure.
    lgdt [gdt_descriptor]  ; Load the base address and limit values from the gdt_descriptor into the GDTR register. This actually sets the GDT that the CPU will use.
    ret  ; Return from the function.

section .data  ; This directive switches to the .data section of memory, where global variables and static data are typically stored.
gdt_descriptor:  ; This is a label, which represents the start of the gdt_descriptor structure in memory.
    dw 0x00  ; Size: Reserve a 2-byte space in memory (double word) initialized with 0. This represents the size of the GDT.
    dd 0x00  ; GDT Start Address: Reserve a 4-byte space in memory (double dword) initialized with 0. This represents the base address of the GDT.
