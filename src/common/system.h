#ifndef SYSTEM_H
#define SYSTEM_H
#include <stddef.h>
#include <stdint.h>
// Some useful macro
#define ALIGN(x, a) __ALIGN_MASK(x, (typeof(x))(a)-1)
#define __ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
void panic(const char *message, const char *file, uint32_t line);
#define PANIC(msg) panic(msg, __FILE__, __LINE__)

void panic(const char *message, const char *file, uint32_t line);
#define LOG(msg) log(msg, __FILE__, __LINE__)
#define ASSERT(b) ((b) ? (void)0 : panic(#b, __FILE__, __LINE__))

#define LOAD_MEMORY_ADDRESS 0xC0000000

#define TRUE 1
#define FALSE 0

#define K 1024
#define M (1024 * K)
#define G (1024 * M)

#define KDEBUG 1

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

#define TOTAL_INTERRUPTS 512

// 100MB heap size
#define HEAP_SIZE_BYTES 104857600
#define HEAP_BLOCK_SIZE 4096
#define HEAP_ADDRESS 0x01000000
#define HEAP_TABLE_ADDRESS 0x00007E00

#define SECTOR_SIZE 512

#define MAX_FILESYSTEMS 12
#define MAX_FILE_DESCRIPTORS 512

#define MAX_PATH 108

#define TOTAL_GDT_SEGMENTS 6

#define PROGRAM_VIRTUAL_ADDRESS 0x400000
#define USER_PROGRAM_STACK_SIZE 1024 * 16
#define PROGRAM_VIRTUAL_STACK_ADDRESS_START 0x3FF000
#define PROGRAM_VIRTUAL_STACK_ADDRESS_END PROGRAM_VIRTUAL_STACK_ADDRESS_START - USER_PROGRAM_STACK_SIZE

#define MAX_PROGRAM_ALLOCATIONS 1024
#define MAX_PROCESSES 12

#define USER_DATA_SEGMENT 0x23
#define USER_CODE_SEGMENT 0x1B

#define MAX_ISR80H_COMMANDS 1024

#define KEYBOARD_BUFFER_SIZE 1024

#define ALL_OK 0
#define EIO 1
#define EINVARG 2
#define ENOMEM 3
#define EBADPATH 4
#define EFSNOTUS 5
#define ERDONLY 6
#define EUNIMP 7
#define EISTKN 8
#define EINFORMAT 9

#endif