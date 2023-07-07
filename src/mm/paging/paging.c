#include "idt/idt.h"
#include "paging.h"
#include "mm/heap/kernel_heap.h" // Include for memory allocation functions
#include "status.h"                  // Include for error codes

// Load the page directory into the processor's control registers (external assembly function)
extern void load_page_directory(uint32_t *directory);

void paging_init()
{
  disable_interrupts();
  enable_paging();
  enable_interrupts();
}

// Global variable to store the current page directory
static uint32_t *current_page_directory = 0;

// Allocate and initialize a new 4GB paging chunk with the specified flags
struct paging_4GB_chunk_t *paging_new_4GB(uint8_t flags)
{
  // Allocate memory for the page directory
  uint32_t *directory = kernel_zalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);

  // Initialize offset variable
  int offset = 0;

  // Loop through each entry in the page directory
  for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
  {
    // Allocate memory for a page table entry
    uint32_t *entry = kernel_zalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);

    // Loop through each entry in the page table
    for (int b = 0; b < PAGING_TOTAL_ENTRIES_PER_TABLE; b++)
    {
      // Calculate the physical address and set it in the page table entry
      entry[b] = (offset + (b * PAGING_PAGE_SIZE)) | flags;
    }

    // Update the offset
    offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);

    // Set the page table entry in the page directory
    directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITEABLE;
  }

  // Allocate memory for the paging 4GB chunk structure
  struct paging_4GB_chunk_t *chunk_4GB = kernel_zalloc(sizeof(struct paging_4GB_chunk_t));

  // Set the page directory entry in the paging 4GB chunk structure
  chunk_4GB->directory_entry = directory;

  // Return the paging 4GB chunk structure
  return chunk_4GB;
}

// Switch the page directory to the provided 4GB chunk
void paging_switch(struct paging_4GB_chunk_t *directory)
{
  // Load the provided page directory into the processor's control registers
  load_page_directory(directory->directory_entry);

  // Update the current page directory pointer
  current_page_directory = directory->directory_entry;
}

// Free the memory occupied by a 4GB paging chunk
void paging_free_4GB(struct paging_4GB_chunk_t *chunk)
{
  // Loop through each entry in the page directory
  for (int i = 0; i < 1024; i++)
  {
    // Get the page directory entry
    uint32_t entry = chunk->directory_entry[i];

    // Get the page table pointer from the entry
    uint32_t *table = (uint32_t *)(entry & 0xFFFFF000);

    // Free the memory occupied by the page table
    kernel_free(table);
  }

  // Free the memory occupied by the page directory and the paging 4GB chunk structure
  kernel_free(chunk->directory_entry);
  kernel_free(chunk);
}

// Get the directory pointer from a 4GB paging chunk
uint32_t *paging_4GB_chunk_get_directory(struct paging_4GB_chunk_t *chunk)
{
  // Return the page directory pointer from the provided paging 4GB chunk
  return chunk->directory_entry;
}

// Check if a given address is aligned to the page size
bool paging_is_aligned(void *addr)
{
  // Calculate the remainder when the address is divided by the page size
  // If the remainder is zero, the address is aligned
  return ((uint32_t)addr % PAGING_PAGE_SIZE) == 0;
}

// Get the directory and table indexes for a virtual address
int paging_get_indexes(void *virtual_address, uint32_t *directory_index_out, uint32_t *table_index_out)
{
  int res = 0;

  // Check if the virtual address is aligned to the page size
  if (!paging_is_aligned(virtual_address))
  {
    // If not aligned, return an error code
    res = -EINVARG;
    goto out;
  }

  // Calculate the directory and table indexes
  *directory_index_out = ((uint32_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
  *table_index_out = ((uint32_t)virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);

out:
  return res;
}

// Align a given address to the next page boundary
void *paging_align_address(void *ptr)
{
  // Calculate the remainder when the address is divided by the page size
  uint32_t remainder = (uint32_t)ptr % PAGING_PAGE_SIZE;

  // If the remainder is non-zero, align the address to the next page boundary
  if (remainder)
  {
    return (void *)((uint32_t)ptr + PAGING_PAGE_SIZE - remainder);
  }

  // If already aligned, return the address as is
  return ptr;
}

// Align a given address to the lower page boundary
void *paging_align_to_lower_page(void *addr)
{
  // Calculate the page-aligned address by subtracting the remainder when divided by the page size
  uint32_t _addr = (uint32_t)addr;
  _addr -= (_addr % PAGING_PAGE_SIZE);
  return (void *)_addr;
}

// Map a virtual address to a physical address in the provided directory with the given flags
int paging_map(struct paging_4GB_chunk_t *directory, void *virtual_addr, void *physical_addr, int flags)
{
  // Check if the virtual and physical addresses are aligned to the page size
  if (((unsigned int)virtual_addr % PAGING_PAGE_SIZE) || ((unsigned int)physical_addr % PAGING_PAGE_SIZE))
  {
    // If either address is not aligned, return an error code
    return -EINVARG;
  }

  // Set the virtual address to the specified physical address in the page directory
  return paging_set(directory->directory_entry, virtual_addr, (uint32_t)physical_addr | flags);
}

// Map a range of virtual addresses to physical addresses in the provided directory with the given flags
int paging_map_range(struct paging_4GB_chunk_t *directory, void *virtual_addr, void *physical_addr, int count, int flags)
{
  int res = 0;

  // Loop through each page in the range
  for (int i = 0; i < count; i++)
  {
    // Map the virtual address to the physical address with the specified flags
    res = paging_map(directory, virtual_addr, physical_addr, flags);
    if (res < 0)
      break;

    // Increment the virtual and physical addresses by the page size
    virtual_addr += PAGING_PAGE_SIZE;
    physical_addr += PAGING_PAGE_SIZE;
  }

  // Return the result
  return res;
}

// Map a range of virtual addresses to physical addresses in the provided directory with the given flags
int paging_map_virtual_to_physical_addresses(struct paging_4GB_chunk_t *directory, void *virtual_addr, void *physical_addr, void *physical_end_addr, int flags)
{
  int res = 0;

  // Check if the virtual address is aligned to the page size
  if ((uint32_t)virtual_addr % PAGING_PAGE_SIZE)
  {
    // If not aligned, return an error code
    res = -EINVARG;
    goto out;
  }

  // Check if the physical addresses are aligned to the page size
  if ((uint32_t)physical_addr % PAGING_PAGE_SIZE || (uint32_t)physical_end_addr % PAGING_PAGE_SIZE)
  {
    // If not aligned, return an error code
    res = -EINVARG;
    goto out;
  }

  // Loop through each page in the range
  while (physical_addr < physical_end_addr)
  {
    // Map the virtual address to the physical address with the specified flags
    res = paging_map(directory, virtual_addr, physical_addr, flags);
    if (res < 0)
      break;

    // Increment the virtual and physical addresses by the page size
    virtual_addr += PAGING_PAGE_SIZE;
    physical_addr += PAGING_PAGE_SIZE;
  }

out:
  // Return the result
  return res;
}

// Set the value of a virtual address in the specified directory to the given value
int paging_set(uint32_t *directory, void *virtual_addr, uint32_t val)
{
  uint32_t directory_index;
  uint32_t table_index;

  // Get the directory and table indexes for the virtual address
  int res = paging_get_indexes(virtual_addr, &directory_index, &table_index);
  if (res < 0)
    return res;

  // Get the page table pointer from the directory entry
  uint32_t *table = (uint32_t *)(directory[directory_index] & 0xFFFFF000);

  // Set the value in the page table entry
  table[table_index] = val;

  // Return success
  return 0;
}

// Get the value at a virtual address in the specified directory
uint32_t paging_get(uint32_t *directory, void *virtual_addr)
{
  uint32_t directory_index;
  uint32_t table_index;

  // Get the directory and table indexes for the virtual address
  int res = paging_get_indexes(virtual_addr, &directory_index, &table_index);
  if (res < 0)
    return 0;

  // Get the page table pointer from the directory entry
  uint32_t *table = (uint32_t *)(directory[directory_index] & 0xFFFFF000);

  // Get the value from the page table entry
  return table[table_index];
}

// Get the physical address corresponding to a virtual address in the specified directory
void *paging_get_physical_address(uint32_t *directory, void *virtual_addr)
{
  uint32_t directory_index;
  uint32_t table_index;

  // Get the directory and table indexes for the virtual address
  int res = paging_get_indexes(virtual_addr, &directory_index, &table_index);
  if (res < 0)
    return NULL;

  // Get the page table pointer from the directory entry
  uint32_t *table = (uint32_t *)(directory[directory_index] & 0xFFFFF000);

  // Get the physical address from the page table entry
  uint32_t physical_addr = table[table_index] & 0xFFFFF000;

  // Calculate the physical address by adding the page offset
  return (void *)(physical_addr + ((uint32_t)virtual_addr & 0x00000FFF));
}
