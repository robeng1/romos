#include "kernel_heap.h"
#include "heap.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kernel_heap_init()
{
  int total_table_entries = ROMOS_HEAP_SIZE_BYTES / ROMOS_HEAP_BLOCK_SIZE;
  kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY *)(ROMOS_HEAP_TABLE_ADDRESS);
  kernel_heap_table.total = total_table_entries;

  void *end = (void *)(ROMOS_HEAP_ADDRESS + ROMOS_HEAP_SIZE_BYTES);
  int res = heap_create(&kernel_heap, (void *)(ROMOS_HEAP_ADDRESS), end, &kernel_heap_table);
  if (res < 0)
  {
    // TODO: should panic
    print("Failed to create heap\n");
  }
}

void *kernel_malloc(size_t size)
{
  return heap_malloc(&kernel_heap, size);
}

void *kernel_zalloc(size_t size)
{
  void *ptr = kernel_malloc(size);
  if (!ptr)
    return 0;

  memset(ptr, 0x00, size);
  return ptr;
}

void kernel_free(void *ptr)
{
  heap_free(&kernel_heap, ptr);
}