#ifndef FILE_H
#define FILE_H

#include "parser.h" // Include the "parser.h" header file
#include <stdint.h> // Include the <stdint.h> standard library header file

typedef unsigned int FILE_SEEK_MODE; // Define the FILE_SEEK_MODE type as an unsigned int
enum
{
  SEEK_SET, // Enum constant SEEK_SET
  SEEK_CUR, // Enum constant SEEK_CUR
  SEEK_END  // Enum constant SEEK_END
};

typedef unsigned int FILE_MODE; // Define the FILE_MODE type as an unsigned int
enum
{
  FILE_MODE_READ,   // Enum constant FILE_MODE_READ
  FILE_MODE_WRITE,  // Enum constant FILE_MODE_WRITE
  FILE_MODE_APPEND, // Enum constant FILE_MODE_APPEND
  FILE_MODE_INVALID // Enum constant FILE_MODE_INVALID
};

enum
{
  FILE_STAT_READ_ONLY = 0b00000001 // Bit flag FILE_STAT_READ_ONLY set to 1
};

typedef unsigned int FILE_STAT_FLAGS; // Define the FILE_STAT_FLAGS type as an unsigned int

struct disk_t; // Forward declaration of the struct disk

// Structure representing file stat information
struct file_stat_t
{
  FILE_STAT_FLAGS flags; // File stat flags
  uint32_t filesize;     // File size
};

// Function pointer types for file system operations
typedef void *(*FS_OPEN_FUNCTION)(struct disk_t *disk, struct path_part_t *path, FILE_MODE mode);
typedef int (*FS_READ_FUNCTION)(struct disk_t *disk, void *private, uint32_t size, uint32_t nmemb, char *out);
typedef int (*FS_RESOLVE_FUNCTION)(struct disk_t *disk);
typedef int (*FS_CLOSE_FUNCTION)(void *private);
typedef int (*FS_SEEK_FUNCTION)(void *private, uint32_t offset, FILE_SEEK_MODE seek_mode);
typedef int (*FS_STAT_FUNCTION)(struct disk_t *disk, void *private, struct file_stat_t *stat);

// Structure representing a file system
struct filesystem_t
{
  FS_RESOLVE_FUNCTION resolve; // Function to resolve the file system
  FS_OPEN_FUNCTION open;       // Function to open a file
  FS_READ_FUNCTION read;       // Function to read from a file
  FS_SEEK_FUNCTION seek;       // Function to seek within a file
  FS_STAT_FUNCTION stat;       // Function to retrieve file stat information
  FS_CLOSE_FUNCTION close;     // Function to close a file
  char name[20];               // Name of the file system
};

// Structure representing a file descriptor
struct file_descriptor_t
{
  int index;                       // Descriptor index
  struct filesystem_t *filesystem; // File system associated with the file descriptor
  void *private;                   // Private data for internal file descriptor
  struct disk_t *disk;             // Disk to be used with the file descriptor
};

// Function declarations
void fs_init();                                              // Initialize the file system
int fopen(const char *filename, const char *mode_str);       // Open a file with specified filename and mode
int fseek(int fd, int offset, FILE_SEEK_MODE whence);        // Move the file position indicator to a specific location
int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd); // Read data from a file
int fstat(int fd, struct file_stat_t *stat);                 // Retrieve file stat information
int fclose(int fd);                                          // Close a file

void fs_insert_filesystem(struct filesystem_t *filesystem); // Insert a file system into the available file systems list
struct filesystem_t *fs_resolve(struct disk_t *disk);       // Resolve the file system for a specific disk

#endif
