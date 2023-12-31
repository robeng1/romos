#ifndef MEMORY_HEAP_H
#define MEMORY_HEAP_H
#include "common/system.h"
#include <stdint.h>
#include <stddef.h>

#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00

#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_FIRST 0b01000000

typedef uint8_t HEAP_BLOCK_TABLE_ENTRY;

struct heap_table_t
{
  HEAP_BLOCK_TABLE_ENTRY *entries;
  size_t total;
};

struct heap_t
{
  struct heap_table_t *table;

  // Start address of the heap data pool
  void *start_address;
};

int heap_create(struct heap_t *heap, void *ptr, void *end, struct heap_table_t *table);
void *heap_malloc(struct heap_t *heap, size_t size);
void heap_free(struct heap_t *heap, void *ptr);

#endif