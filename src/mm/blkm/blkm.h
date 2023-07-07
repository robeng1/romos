#ifndef BLKM_H
#define BLKM_H
#include <stdint.h>
#include <stddef.h>

void mem_init(uint8_t *mem_start, uint8_t *bitmap_start);
void *mem_alloc(size_t size);
void *mem_zalloc(size_t size);
void mem_free(void *ptr, size_t size);

#endif // BLKM_H
