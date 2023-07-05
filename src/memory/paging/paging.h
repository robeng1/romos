#ifndef MEMORY_PAGING_H
#define MEMORY_PAGING_H

#include <stdint.h>  // Include for uint32_t data type
#include <stddef.h>  // Include for size_t data type
#include <stdbool.h> // Include for bool data type

#define PAGING_CACHE_DISABLED 0b00010000  // Cache disabled flag
#define PAGING_WRITE_THROUGH 0b00001000   // Write-through flag
#define PAGING_ACCESS_FROM_ALL 0b00000100 // Accessible from all flag
#define PAGING_IS_WRITEABLE 0b00000010    // Writeable flag
#define PAGING_IS_PRESENT 0b00000001      // Present flag

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024 // Total entries per page table
#define PAGING_PAGE_SIZE 4096               // Page size in bytes

// Structure representing a 4GB chunk of paging
struct paging_4GB_chunk
{
  uint32_t *directory_entry; // Pointer to the page directory entry
};

// Function declarations

// Allocate and initialize a new 4GB paging chunk with the specified flags
struct paging_4GB_chunk *paging_new_4GB(uint8_t flags);

// Switch the page directory to the provided 4GB chunk
void paging_switch(struct paging_4GB_chunk *directory);

// Enable paging (external assembly function)
extern void enable_paging();

// Set the value of a virtual address in the specified directory to the given value
int paging_set(uint32_t *directory, void *virt, uint32_t val);

// Check if a given address is aligned to the page size
bool paging_is_aligned(void *addr);

// Get the directory pointer from a 4GB paging chunk
uint32_t *paging_4GB_chunk_get_directory(struct paging_4GB_chunk *chunk);

// Free the memory occupied by a 4GB paging chunk
void paging_free_4GB(struct paging_4GB_chunk *chunk);

// Map a virtual address to a physical address in the provided directory with the given flags
int paging_map_to(struct paging_4GB_chunk *directory, void *virt, void *phys, void *phys_end, int flags);

// Map a range of virtual addresses to physical addresses in the provided directory with the given flags
int paging_map_range(struct paging_4GB_chunk *directory, void *virt, void *phys, int count, int flags);

// Map a virtual address to a physical address in the provided directory with the given flags
int paging_map(struct paging_4GB_chunk *directory, void *virt, void *phys, int flags);

// Align a given address to the next page boundary
void *paging_align_address(void *ptr);

// Get the value at a virtual address in the specified directory
uint32_t paging_get(uint32_t *directory, void *virt);

// Align a given address to the lower page boundary
void *paging_align_to_lower_page(void *addr);

// Get the physical address corresponding to a virtual address in the specified directory
void *paging_get_physical_address(uint32_t *directory, void *virt);

#endif // PAGING_H
