#ifndef SYSTEM_H
#define SYSTEM_H

// Some useful macro
#define ALIGN(x, a) __ALIGN_MASK(x, (typeof(x))(a)-1)
#define __ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))

// Define some constants that (almost) all other modules need
#define PANIC(msg) panic(msg, __FILE__, __LINE__)
#define ASSERT(b) ((b) ? (void)0 : panic(#b, __FILE__, __LINE__))

// Our kernel now loads at 0xC0000000, so what low memory address such as 0xb800 you used to access, should be LOAD_MEMORY_ADDRESS + 0xb800
#define LOAD_MEMORY_ADDRESS 0xC0000000

#define NULL 0
#define TRUE 1
#define FALSE 0

#define K 1024
#define M (1024 * K)
#define G (1024 * M)

#define KDEBUG 1

#endif