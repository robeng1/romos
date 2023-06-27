ORG 0x7C00     ; Set the origin of the code to memory address 0x7C00
BITS 16        ; Specify that the code is 16-bit

start:
  mov si, message   ; Move the address of the 'message' string into the SI register
  call print       ; Call the 'print' subroutine to display the message
  jmp $            ; Jump to itself, creating an infinite loop and halting the program

print:
  mov bx, 0        ; Initialize the BX register to zero
.loop:
  lodsb            ; Load the byte at the memory location pointed by SI into the AL register
  cmp al, 0        ; Compare the value in AL with zero (the null terminator)
  je .done         ; If they are equal, jump to '.done' and terminate the loop
  call print_char  ; Call the 'print_char' subroutine to display the character
  jmp .loop        ; Jump back to the beginning of the loop

.done:
  ret              ; Return from the subroutine

print_char:
  mov ah, 0eh      ; Set the AH register to 0x0E (BIOS interrupt code for writing a character)
  int 0x10         ; Trigger interrupt 0x10 to display the character stored in AL
  ret              ; Return from the subroutine

message: db 'Hello World from RomOS!', 0   ; Declare the null-terminated message string

times 510-($ - $$) db 0   ; Fill the remaining bytes in the boot sector with zeros to make it 512 bytes

dw 0xAA55        ; Boot signature: 0xAA55, indicating a valid boot sector
