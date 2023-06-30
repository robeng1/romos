ORG 0x7c00             ; Set the origin of the code to memory address 0x7C00
BITS 16                ; Specify that the code is 16-bit

CODE_SEG equ gdt_code - gdt_start  ; Calculate the offset of the code segment in the GDT
DATA_SEG equ gdt_data - gdt_start  ; Calculate the offset of the data segment in the GDT

_start:
    jmp short start      ; Jump to the 'start' label (relative jump)
    nop                  ; No operation (placeholder)

times 33 db 0           ; Fill 33 bytes with zeros

start:
    jmp 0x00:init        ; Jump to the 'init' label at memory address 0x7C0

init:
    cli                  ; Clear interrupts (disable interrupts)
    mov ax, 0x00         ; Move the value 0x7C0 into the AX register
    mov ds, ax           ; Move the value in AX into the DS register
    mov es, ax           ; Move the value in AX into the ES register
    mov ss, ax           ; Move the value in AX into the SS register
    mov sp, 0x7c00       ; Move the value 0x7C00 into the SP register
    sti                  ; Enable interrupts (enable interrupts)

.load_protected:
    cli                  ; Clear interrupts (disable interrupts)
    lgdt [gdt_descriptor] ; Load the GDT descriptor into GDTR
    mov eax, cr0         ; Move the value of CR0 into EAX
    or eax, 0x1          ; Set the first bit of EAX to enable protected mode
    mov cr0, eax         ; Move the value in EAX back to CR0
    jmp CODE_SEG:load32  ; Far jump to the protected mode code segment

; GDT (Global Descriptor Table)
gdt_start:
gdt_null:                ; A null segment descriptor with base and limit both set to 0
    dd 0x0
    dd 0x0
    

; GDT Entry for Code Segment (CS)
; Segment limit set to maximum value (64KB)
; Base set to 0 (starts at the beginning of memory)
; Access byte specifies code segment, readable and executable, privilege level 0
gdt_code:
    dw 0xffff            ; Segment limit (first 0-15 bits)
    dw 0                 ; Base (first 0-15 bits)
    db 0                 ; Base (16-23 bits)
    db 0x9a              ; Access byte
    db 0b11001111        ; High 4 bits: Flags, Low 4 bits: Flags
    db 0                 ; Base (24-31 bits)
    

; GDT Entry for Data Segment (DS, SS, ES, FS, GS)
; Segment limit set to maximum value (64KB)
; Base set to 0 (starts at the beginning of memory)
; Access byte specifies data segment, writable, expand-down, privilege level 0
gdt_data:
    dw 0xffff            ; Segment limit (first 0-15 bits)
    dw 0                 ; Base (first 0-15 bits)
    db 0                 ; Base (16-23 bits)
    db 0x92              ; Access byte
    db 0b11001111        ; High 4 bits: Flags, Low 4 bits: Flags
    db 0                 ; Base (24-31 bits)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of the GDT
    dd gdt_start                ; Address of the GDT

[BITS 32]
load32:
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
    
    jmp $                ; Endless loop (halt execution)

times 510-($ - $$) db 0  ; Fill the remaining bytes in the boot sector with zeros to make it 512 bytes

dw 0xAA55               ; Boot signature: 0xAA55, indicating a valid boot sector
