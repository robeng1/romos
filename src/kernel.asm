[BITS 32]  ; Set the assembly code to be executed in 32-bit mode

global _start
extern start_kernel

CODE_SEG equ 0x08  ; Define the offset of the code segment in the GDT
DATA_SEG equ 0x10  ; Define the offset of the data segment in the GDT

_start:
    mov ax, DATA_SEG     ; Move the offset of the data segment into AX
    mov ds, ax           ; Load DS with the data segment offset
    mov es, ax           ; Load ES with the data segment offset
    mov fs, ax           ; Load FS with the data segment offset
    mov gs, ax           ; Load GS with the data segment offset
    mov ss, ax           ; Load SS with the data segment offset
    mov ebp, 0x00200000  ; Set EBP to 0x00200000
    mov esp, ebp         ; Set ESP to the value in EBP

    ; Enable the A20 line
    ; The A20 line, also known as Address line 20, is a signal line in the x86 architecture that controls the addressing of memory beyond the 1 MB limit.
    ; In the real mode of operation (16-bit mode), the x86 processor can only directly address 20 bits of memory, allowing access to a maximum of 1 MB of physical memory.
    ; Enabling the A20 line is necessary when transitioning from real mode to protected mode, as it allows the processor to access memory beyond 1 MB.
    ; By default, the A20 line is disabled in real mode, which means that addresses greater than 1 MB will wrap around to the lower memory region.
    ; Enabling the A20 line is necessary to properly access memory beyond 1 MB in protected mode, allowing the operating system and applications to utilize the full memory capacity of the system.
    ; Read more: https://wiki.osdev.org/A20_Line
    in al, 0x92          ; Read the value from port 0x92 into AL
    or al, 2             ; Set bit 1 of AL to enable the A20 line
    out 0x92, al         ; Write the value in AL back to port 0x92

    ; Remap the master PIC
    ; The Programmable Interrupt Controller (PIC) is responsible for handling interrupts in the x86 architecture.
    ; Remapping the PIC is necessary to prevent conflicts with CPU exceptions and to properly handle interrupt requests (IRQs).
    ; By default, the PIC's interrupt vector offset starts at 0x08, which overlaps with CPU exceptions. Remapping allows us to change the offset to avoid conflicts.
    ; This code remaps the master PIC to interrupt vector offset 0x20 (32 in decimal), leaving the first 32 interrupts for CPU exceptions.
    ; Read more: https://wiki.osdev.org/PIC
    mov al, 00010001b    ; Set the initialization command for the master PIC
    out 0x20, al         ; Send the command to the master PIC port

    mov al, 0x20         ; Set the interrupt vector offset for the master ISR
    out 0x21, al         ; Send the offset to the master PIC port

    mov al, 00000001b    ; Set the initialization command for the master PIC
    out 0x21, al         ; Send the command to the master PIC port
    ; End remap of the master PIC
    sti

    call start_kernel     ; Call the kernel_main function

    jmp $                ; Endless loop (halt execution)

times 512-($ - $$) db 0  ; Fill the remaining bytes in the boot sector with zeros to make it 512 bytes
