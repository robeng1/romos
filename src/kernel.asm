[BITS 32]

global _start
extern kernel_main

    
CODE_SEG equ 0x08  ; Calculate the offset of the code segment in the GDT
DATA_SEG equ 0x10  ; Calculate the offset of the data segment in the GDT

_start:
    mov ax, DATA_SEG     ; Move the offset of the data segment into AX
    mov ds, ax           ; Load DS with the data segment offset
    mov es, ax           ; Load ES with the data segment offset
    mov fs, ax           ; Load FS with the data segment offset
    mov gs, ax           ; Load GS with the data segment offset
    mov ss, ax           ; Load SS with the data segment offset
    mov ebp, 0x00200000  ; Set EBP to 0x00200000
    mov esp, ebp         ; Set ESP to the value in EBP

    ; The A20 line, also known as Address line 20, is a signal line in the x86 architecture that controls the addressing of memory beyond the 1 MB limit. 
    ; In the real mode of operation (16-bit mode), the x86 processor can only directly address 20 bits of memory, allowing access to a maximum of 1 MB of physical memory.
    ; The A20 line determines whether the address bus wraps around after reaching the 1 MB boundary or continues addressing the memory above 1 MB.
    ; By default, the A20 line is disabled in real mode, which means that addresses greater than 1 MB will wrap around to the lower memory region.
    ; Enabling the A20 line is necessary when transitioning from real mode to protected mode, as it allows the processor to access memory beyond 1 MB. 
    ; Fast A20 Gate: On most newer computers starting with the IBM PS/2, the chipset has a FAST A20 option that can quickly enable the A20 line. 
    ; To enable A20 this way, there is no need for delay loops or polling, just 3 simple instructions
    ; ```
    ;     in al, 0x92     ; Read the value from port 0x92 into AL
    ;     or al, 2        ; Set bit 1 of AL
    ;     out 0x92, al    ; Write the value in AL back to port 0x92
    ; ```
    ; These instructions read the value from port 0x92 into the AL register, which is a configuration port for various system functions. 
    ; Bit 1 of the value is then set to enable the A20 line. Finally, the modified value is written back to port 0x92 to enable the A20 line.
    ; Enabling the A20 line is necessary to properly access memory beyond 1 MB in protected mode, allowing the operating system and applications to utilize the full memory capacity of the system.
    ; See https://wiki.osdev.org/A20_Line
    in al, 0x92          ; Read the value from port 0x92 into AL
    or al, 2             ; Set bit 1 of AL
    out 0x92, al         ; Write the value in AL back to port 0x92
    call kernel_main
    jmp $                ; Endless loop (halt execution)

times 512-($ - $$) db 0  ; Fill the remaining bytes in the boot sector with zeros to make it 512 bytes