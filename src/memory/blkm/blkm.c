#include <string.h>
#include "status.h"
#include "blkm.h"

#define MEMORY_SIZE (1024 * 1024 * 1024) // Total memory size (1GB)
#define BLOCK_SIZE (4 * 1024)            // Size of each memory block (4KB)
#define BITMAP_SIZE (MEMORY_SIZE / BLOCK_SIZE)

uint8_t *memory; // The actual memory
uint8_t *bitmap; // The bitmap

// Initialize the memory manager
void mem_init(uint8_t *mem_start, uint8_t *bitmap_start)
{
  memory = mem_start;
  bitmap = bitmap_start;
  memset(bitmap, 0, BITMAP_SIZE / 8); // Clear the bitmap
}

// Allocate a block of memory
void *mem_alloc(size_t size)
{
  uint32_t blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE; // Round up to nearest block
  uint32_t blocks_found = 0;
  uint32_t start_block = 0;

  for (uint32_t i = 0; i < BITMAP_SIZE; i++)
  {
    if ((bitmap[i / 8] & (1 << (i % 8))) == 0)
    { // If the bit is clear
      if (blocks_found == 0)
      {
        start_block = i; // Start of a new free block sequence
      }
      blocks_found++;
      if (blocks_found == blocks_needed)
      { // If we've found enough blocks
        // Check alignment
        if (start_block % blocks_needed == 0)
        {
          // Mark the blocks as in use
          for (uint32_t j = start_block; j < start_block + blocks_needed; j++)
          {
            bitmap[j / 8] |= (1 << (j % 8));
          }
          return &memory[start_block * BLOCK_SIZE]; // Return the memory block
        }
        else
        {
          // Reset blocks_found to search for a new sequence
          blocks_found = 0;
        }
      }
    }
    else
    {
      blocks_found = 0; // Reset the count
    }
  }
  return -ENOMEM; // No free blocks large enough
}

// Free a block of memory
void mem_free(void *ptr, size_t size)
{
  uint32_t blocks_to_free = (size + BLOCK_SIZE - 1) / BLOCK_SIZE; // Round up to nearest block
  uint32_t start_block = ((uint8_t *)ptr - memory) / BLOCK_SIZE;
  // Clear the bits
  for (uint32_t i = start_block; i < start_block + blocks_to_free; i++)
  {
    bitmap[i / 8] &= ~(1 << (i % 8));
  }
}

void *mem_zalloc(size_t size)
{
  void *ptr = mem_alloc(size);
  if (!ptr)
    return 0;

  memset(ptr, 0x00, size);
  return ptr;
}