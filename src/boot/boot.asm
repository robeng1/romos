ORG 0x7c00             ; Set the origin of the code to memory address 0x7C00
BITS 16                ; Specify that the code is 16-bit

CODE_SEG equ gdt_code - gdt_start  ; Calculate the offset of the code segment in the GDT
DATA_SEG equ gdt_data - gdt_start  ; Calculate the offset of the data segment in the GDT

jmp short start      ; Jump to the 'start' label (relative jump)
nop                  ; No operation (placeholder)

; FAT16 Header
oem_identifier              db 'ROMOS   '       ; OEM identifier: A string identifying the OEM or operating system
bytes_per_sector            dw 0x200            ; Bytes per sector: The number of bytes in each sector of the disk
sectors_per_cluster         db 0x80             ; Sectors per cluster: The number of sectors grouped together as a cluster
reserved_sectors            dw 200              ; Reserved sectors: The number of sectors reserved for the boot sector and other purposes
fat_copies                  db 0x02             ; Number of FAT copies: The number of copies of the File Allocation Table (FAT)
root_dir_entries            dw 0x40             ; Number of root directory entries: The maximum number of entries in the root directory
num_sectors                 dw 0x00             ; Total number of sectors: The total number of sectors on the disk (0 for large disks)
media_type                  db 0xF8             ; Media type: The type of media (e.g., floppy disk, hard disk)
sectors_per_fat             dw 0x100            ; Sectors per FAT: The number of sectors occupied by each copy of the FAT
sectors_per_track           dw 0x20             ; Sectors per track: The number of sectors per track
num_of_heads                dw 0x40             ; Number of heads: The number of heads or read/write surfaces
hidden_sectors              dd 0x00             ; Number of hidden sectors: The number of sectors preceding the partition
sectors_big                 dd 0x773594         ; Total number of sectors (for large disks): The total number of sectors on the disk for large disks

; Extended BPB (Dos 4.0)
drive_number                db 0x80             ; Drive number: The drive number or unit number
win_nt_bit                  db 0x00             ; Windows NT bit: Reserved for Windows NT operating systems
signature                   db 0x29             ; Signature: A signature byte indicating the presence of an extended BPB
volume_id                   dd 0xD105           ; Volume ID: A unique identifier for the volume
volume_id_string            db 'ROMOS BOOT '    ; Volume ID string: A string representing the volume ID
system_id_string            db 'FAT16   '       ; System ID string: A string representing the system ID



start:
    jmp 0x00:init        ; Jump to the 'init' label at memory address 0x7C0

init:
    cli                  ; Clear interrupts (disable interrupts)
    mov ax, 0x00         ; Move the value 0x7C0 into the AX register (set segment registers)
    mov ds, ax           ; Move the value in AX into the DS register
    mov es, ax           ; Move the value in AX into the ES register
    mov ss, ax           ; Move the value in AX into the SS register
    mov sp, 0x7c00       ; Move the value 0x7C00 into the SP register (stack pointer)
    sti                  ; Enable interrupts (enable interrupts)

.load_protected:
    cli                  ; Clear interrupts (disable interrupts)
    lgdt [gdt_descriptor] ; Load the GDT descriptor into GDTR (Global Descriptor Table Register)
    mov eax, cr0         ; Move the value of CR0 (Control Register 0) into EAX
    or eax, 0x1          ; Set the first bit of EAX to enable protected mode
    mov cr0, eax         ; Move the value in EAX back to CR0
    jmp CODE_SEG:load_kernel  ; Far jump to the protected mode code segment

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
load_kernel:
    mov eax, 1 ; Start reading from sector 1 because sector 2 is used for the bootloader
    mov ecx, 100 ; Number of sectors to load
    mov edi, 0x0100000 ; Destination address in memory to load the sectors
    call ata_lba_read ; Call the function to read sectors using ATA LBA (Logical Block Addressing) method
    jmp CODE_SEG:0x0100000 ; Jump to the loaded code(the kernel) at memory address 0x0100000

; Function to read sectors using ATA LBA (Logical Block Addressing) method
ata_lba_read:
    mov ebx, eax ; Backup the LBA (Logical Block Address)

    ; Send the highest 8 bits of the LBA to the hard disk controller
    shr eax, 24 ; Shift right the LBA value by 24 bits to get the highest 8 bits
    or eax, 0xE0 ; Set the highest 3 bits of the LBA to select the master drive
    mov dx, 0x1F6 ; I/O port address for sending the command
    out dx, al ; Send the value in AL (highest 8 bits of the LBA) to the I/O port
    ; Finished sending the highest 8 bits of the LBA

    ; Send the total sectors to read
    mov eax, ecx ; Move the number of sectors to read into EAX
    mov dx, 0x1F2 ; I/O port address for sending the total sectors
    out dx, al ; Send the value in AL (total sectors) to the I/O port
    ; Finished sending the total sectors to read

    ; Send more bits of the LBA
    mov eax, ebx ; Restore the backup LBA
    mov dx, 0x1F3 ; I/O port address for sending more bits of the LBA
    out dx, al ; Send the value in AL (8-15 bits of the LBA) to the I/O port
    ; Finished sending more bits of the LBA

    ; Send more bits of the LBA
    mov dx, 0x1F4 ; I/O port address for sending more bits of the LBA
    mov eax, ebx ; Restore the backup LBA
    shr eax, 8 ; Shift right the LBA value by 8 bits to get the next 8 bits
    out dx, al ; Send the value in AL (16-23 bits of the LBA) to the I/O port
    ; Finished sending more bits of the LBA

    ; Send upper 16 bits of the LBA
    mov dx, 0x1F5 ; I/O port address for sending the upper 16 bits of the LBA
    mov eax, ebx ; Restore the backup LBA
    shr eax, 16 ; Shift right the LBA value by 16 bits to get the upper 16 bits
    out dx, al ; Send the value in AL (24-31 bits of the LBA) to the I/O port
    ; Finished sending upper 16 bits of the LBA

    mov dx, 0x1f7 ; I/O port address for sending the read command
    mov al, 0x20 ; Value 0x20 indicating the read command
    out dx, al ; Send the read command to the I/O port

; Read all sectors into memory
.next_sector:
    push ecx ; Preserve ECX value (number of sectors to read)

; Checking if we need to read
.try_again:
    mov dx, 0x1f7 ; I/O port address for reading the status register
    in al, dx ; Read the value from the I/O port into AL
    test al, 8 ; Test the value in AL for bit 3 (BSY - Busy)
    jz .try_again ; Jump back to .try_again label if BSY bit is set (device is busy)

    ; We need to read 256 words (512 bytes) at a time
    mov ecx, 256 ; Set ECX to 256
    mov dx, 0x1F0 ; I/O port address for reading the data register
    rep insw ; Repeat the input operation of a word (16 bits) from the I/O port
    pop ecx ; Restore the original ECX value
    loop .next_sector ; Repeat the loop for the remaining sectors
    ; End of reading sectors into memory
    ret ; Return from the function

times 510-($ - $$) db 0  ; Fill the remaining bytes in the boot sector with zeros to make it 512 bytes

dw 0xAA55               ; Boot signature: 0xAA55, indicating a valid boot sector
