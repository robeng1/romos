#include "stream.h"                  // Include header file for stream-related functionality
#include "memory/heap/kernel_heap.h" // Include header file for kernel heap functionality
#include "config.h"                  // Include configuration header file

#include <stdbool.h> // Include header file for boolean data type

struct disk_stream_t *new_disk_stream(int disk_id) // Function to create a new disk stream
{
  struct disk_t *disk = disk_get(disk_id); // Get the disk corresponding to the provided disk ID
  if (!disk)                               // If disk retrieval fails (disk is not found)
  {
    return 0; // Return NULL to indicate failure
  }

  struct disk_stream_t *stream = kernel_zalloc(sizeof(struct disk_stream_t)); // Allocate memory for a new disk stream structure
  stream->pos = 0;                                                            // Initialize the stream position to 0
  stream->disk = disk;                                                        // Set the disk for the stream
  return stream;                                                              // Return the newly created disk stream
}

int disk_stream_seek(struct disk_stream_t *stream, int pos) // Function to set the position of the disk stream
{
  stream->pos = pos; // Set the stream position to the provided position
  return 0;          // Return 0 to indicate success
}

int disk_stream_read(struct disk_stream_t *stream, void *out, int total) // Function to read data from the disk stream
{
  int sector = stream->pos / ROMOS_SECTOR_SIZE;                  // Calculate the current sector based on the stream position
  int offset = stream->pos % ROMOS_SECTOR_SIZE;                  // Calculate the offset within the current sector
  int total_to_read = total;                                     // Store the total number of bytes to read
  bool overflow = (offset + total_to_read) >= ROMOS_SECTOR_SIZE; // Check if reading beyond the current sector

  char buf[ROMOS_SECTOR_SIZE]; // Create a buffer to store the sector data
  int res = 0;                 // Variable to store the result of read operation

  while (total_to_read > 0) // Loop until all bytes are read
  {
    if (overflow) // If reading beyond the current sector
    {
      total_to_read -= (offset + total_to_read) - ROMOS_SECTOR_SIZE; // Adjust the number of bytes to read
    }

    res = disk_read_block(stream->disk, sector, 1, buf); // Read a block from the disk into the buffer
    if (res < 0)                                         // If read operation fails
    {
      goto out; // Jump to the cleanup code and return the error code
    }

    for (int i = 0; i < total_to_read; i++) // Copy the data from the buffer to the output
    {
      *(char *)out++ = buf[offset + i];
    }

    // Adjust the stream position and variables for the next iteration
    stream->pos += total_to_read;
    total_to_read = total - total_to_read;
    sector = stream->pos / ROMOS_SECTOR_SIZE;
    offset = stream->pos % ROMOS_SECTOR_SIZE;
    overflow = (offset + total_to_read) >= ROMOS_SECTOR_SIZE;
  }

out:
  return res; // Return the result of the read operation
}

void disk_stream_close(struct disk_stream_t *stream) // Function to close the disk stream
{
  kernel_free(stream); // Free the memory allocated for the stream
}
