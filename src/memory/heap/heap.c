#include "heap.h"
#include "kernel.h"
#include "status.h"
#include "memory/memory.h"
#include <stdbool.h>

static int heap_validate_table(void *ptr, void *end, struct heap_table *table)
{
  int res = 0;

  size_t table_size = (size_t)(end - ptr);
  size_t total_blocks = table_size / ROMOS_HEAP_BLOCK_SIZE;
  if (table->total != total_blocks)
  {
    res = -EINVARG;
    goto out;
  }

out:
  return res;
}

static bool heap_validate_alignment(void *ptr)
{
  return ((unsigned int)ptr % ROMOS_HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct heap *heap, void *ptr, void *end, struct heap_table *table)
{
  int res = 0;

  if (!heap_validate_alignment(ptr) || !heap_validate_alignment(end))
  {
    res = -EINVARG;
    goto out;
  }

  memset(heap, 0, sizeof(struct heap));
  heap->start_address = ptr;
  heap->table = table;

  res = heap_validate_table(ptr, end, table);
  if (res < 0)
  {
    goto out;
  }

  size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
  memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

out:
  return res;
}

static uint32_t heap_align_value_to_upper(uint32_t val)
{
  if ((val % ROMOS_HEAP_BLOCK_SIZE) == 0)
  {
    return val;
  }

  val = (val - (val % ROMOS_HEAP_BLOCK_SIZE));
  val += ROMOS_HEAP_BLOCK_SIZE;
  return val;
}

static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry)
{
  return entry & 0x0f;
}

int heap_get_start_block(struct heap *heap, uint32_t total_blocks)
{
  struct heap_table *table = heap->table;

  size_t start = 0; // Starting index of the subarray
  size_t end = 0;   // Ending index of the subarray
  int count = 0;    // Count of free entries

  // Find the initial subarray that satisfies the block size
  while (end < table->total)
  {
    if (heap_get_entry_type(table->entries[end]) != HEAP_BLOCK_TABLE_ENTRY_FREE)
    {
      count++;
    }

    if (count == total_blocks)
    {
      break;
    }

    while (count > total_blocks)
    {
      if (heap_get_entry_type(table->entries[start]) != HEAP_BLOCK_TABLE_ENTRY_FREE)
      {
        count--;
      }
      start++;
    }
    end++;
  }

  if (count == total_blocks)
  {
    return (int)start;
  }
  else
  {
    return -ENOMEM;
  }
}

void *heap_block_to_address(struct heap *heap, int block)
{
  return heap->start_address + (block * ROMOS_HEAP_BLOCK_SIZE);
}

void heap_mark_blocks_taken(struct heap *heap, int start_block, int total_blocks)
{
  int end_block = (start_block + total_blocks) - 1;

  HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
  if (total_blocks > 1)
  {
    entry |= HEAP_BLOCK_HAS_NEXT;
  }

  for (int i = start_block; i <= end_block; i++)
  {
    heap->table->entries[i] = entry;
    entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
    if (i != end_block - 1)
    {
      entry |= HEAP_BLOCK_HAS_NEXT;
    }
  }
}

void *heap_malloc_blocks(struct heap *heap, uint32_t total_blocks)
{
  void *address = 0;

  int start_block = heap_get_start_block(heap, total_blocks);
  if (start_block < 0)
  {
    goto out;
  }

  address = heap_block_to_address(heap, start_block);

  // Mark the blocks as taken
  heap_mark_blocks_taken(heap, start_block, total_blocks);

out:
  return address;
}

void heap_mark_blocks_free(struct heap *heap, int starting_block)
{
  struct heap_table *table = heap->table;
  for (int i = starting_block; i < (int)table->total; i++)
  {
    HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
    table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
    if (!(entry & HEAP_BLOCK_HAS_NEXT))
    {
      break;
    }
  }
}

int heap_address_to_block(struct heap *heap, void *address)
{
  return ((int)(address - heap->start_address)) / ROMOS_HEAP_BLOCK_SIZE;
}

void *heap_malloc(struct heap *heap, size_t size)
{
  size_t aligned_size = heap_align_value_to_upper(size);
  uint32_t total_blocks = aligned_size / ROMOS_HEAP_BLOCK_SIZE;
  return heap_malloc_blocks(heap, total_blocks);
}

void heap_free(struct heap *heap, void *ptr)
{
  heap_mark_blocks_free(heap, heap_address_to_block(heap, ptr));
}