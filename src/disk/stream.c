#include <stdbool.h>             // Include header file for boolean data type
#include <disk/stream.h>         // Include header file for stream-related functionality
#include <mm/heap/kernel_heap.h> // Include header file for kernel heap functionality
#include <common/system.h>       // Include configuration header file
#include <kernel/kernel.h>

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
  int sector = stream->pos / SECTOR_SIZE;
  int offset = stream->pos % SECTOR_SIZE;
  int total_to_read = total;
  bool overflow = (offset + total_to_read) >= SECTOR_SIZE;
  char buf[SECTOR_SIZE];

  if (overflow)
  {
    total_to_read -= (offset + total_to_read) - SECTOR_SIZE;
  }

  int res = disk_read_block(stream->disk, sector, 1, buf);
  if (res < 0)
  {
    goto out;
  }

  for (int i = 0; i < total_to_read; i++)
  {
    *(char *)out++ = buf[offset + i];
  }

  // Adjust the stream
  stream->pos += total_to_read;
  if (overflow)
  {
    res = disk_stream_read(stream, out, total - total_to_read);
  }
out:
  return res;
}

void disk_stream_close(struct disk_stream_t *stream) // Function to close the disk stream
{
  kernel_free(stream); // Free the memory allocated for the stream
}
