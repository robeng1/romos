[BITS 32]

global _start
extern c_start
extern sys_exit

section .asm

_start:
    call c_start
    call sys_exit
    ret