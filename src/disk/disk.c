#include "disk.h"      // Include header file for disk-related functionality
#include "io/io.h"     // Include header file for I/O operations
#include "config.h"    // Include configuration header file
#include "status.h"    // Include header file for status codes
#include "mm/memory.h" // Include header file for memory operations

struct disk_t disk; // Declare a disk structure named "disk"

int disk_read_sector(int lba, int total, void *buf)
{
  write_byte(0x1F6, (lba >> 24) | 0xE0);          // Write the high 8 bits of the LBA to the disk controller
  write_byte(0x1F2, total);                       // Write the total number of sectors to read
  write_byte(0x1F3, (unsigned char)(lba & 0xff)); // Write the low 8 bits of the LBA to the disk controller
  write_byte(0x1F4, (unsigned char)(lba >> 8));   // Write the next 8 bits of the LBA to the disk controller
  write_byte(0x1F5, (unsigned char)(lba >> 16));  // Write the next 8 bits of the LBA to the disk controller
  write_byte(0x1F7, 0x20);                        // Send the read command to the disk controller

  unsigned short *ptr = (unsigned short *)buf; // Create a pointer to the buffer as an unsigned short pointer
  for (int b = 0; b < total; b++)
  {
    // Wait for the buffer to be ready
    char c = read_byte(0x1F7); // Read the status register of the disk controller
    while (!(c & 0x08))
    {
      c = read_byte(0x1F7); // Read the status register again until the buffer is ready
    }

    // Copy from hard disk to memory
    for (int i = 0; i < 256; i++)
    {
      *ptr = read_word(0x1F0); // Read a word (16 bits) from the data register of the disk controller
      ptr++;                   // Increment the pointer to the next memory location
    }
  }
  return 0; // Return 0 to indicate success
}

void disk_search_and_init()
{
  memset(&disk, 0x00, sizeof(disk));   // Set the disk structure to all zeros
  disk.type = DISK_TYPE_REAL;          // Set the disk type to "real" in the disk structure
  disk.sector_size = SECTOR_SIZE;      // Set the sector size in the disk structure
  disk.id = 0;                         // Set the disk ID in the disk structure
  disk.filesystem = fs_resolve(&disk); // Resolve the filesystem for the disk
}

struct disk_t *disk_get(int index)
{
  if (index != 0)
    return 0; // If the provided index is not 0, return NULL

  return &disk; // Return a pointer to the disk structure
}

int disk_read_block(struct disk_t *idisk, unsigned int lba, int total, void *buf)
{
  if (idisk != &disk)
  {
    return -EIO; // If the provided disk structure is not the same as the global disk, return an error
  }

  return disk_read_sector(lba, total, buf); // Call disk_read_sector with the provided parameters
}
