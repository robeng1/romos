#include "file.h"                    // Include the "file.h" header file
#include "common/system.h"                  // Include the "config.h" header file
#include "mm/memory.h"           // Include the "memory/memory.h" header file
#include "mm/heap/kernel_heap.h" // Include the "memory/heap/kernel_heap.h" header file
#include "string/string.h"           // Include the "string/string.h" header file
#include "disk/disk.h"               // Include the "disk/disk.h" header file
#include "fat/fat16.h"               // Include the "fat/fat16.h" header file
                  // Include the "status.h" header file
#include "kernel/kernel.h"                  // Include the "kernel.h" header file

struct filesystem_t *filesystems[MAX_FILESYSTEMS];                // Array of pointers to filesystems
struct file_descriptor_t *file_descriptors[MAX_FILE_DESCRIPTORS]; // Array of pointers to file descriptors

// Function to get a pointer to a free filesystem slot
static struct filesystem_t **fs_get_free_filesystem()
{
  int i = 0;
  for (i = 0; i < MAX_FILESYSTEMS; i++)
  {
    if (filesystems[i] == 0)
    {
      return &filesystems[i];
    }
  }

  return 0;
}

// Function to insert a filesystem into the filesystems array
void fs_insert_filesystem(struct filesystem_t *filesystem)
{
  struct filesystem_t **fs;
  fs = fs_get_free_filesystem();
  if (!fs)
  {
    print("Problem inserting filesystem");
    while (1)
    {
    }
  }

  *fs = filesystem;
}

// Function to load the static filesystems
static void fs_static_load()
{
  fs_insert_filesystem(fat16_init());
}

// Function to load the filesystems
void fs_load()
{
  memset(filesystems, 0x00, sizeof(filesystems));
  fs_static_load();
}

// Function to initialize the filesystem
void fs_init()
{
  memset(file_descriptors, 0x00, sizeof(file_descriptors));
  fs_load();
}


// Function to free a file descriptor
static void file_free_descriptor(struct file_descriptor_t *desc)
{
  file_descriptors[desc->index - 1] = 0x00;
  kernel_free(desc);
}

// Function to create a new file descriptor
static int file_new_descriptor(struct file_descriptor_t **desc_out)
{
  int res = -ENOMEM;
  for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++)
  {
    if (file_descriptors[i] == 0)
    {
      struct file_descriptor_t *desc = kernel_zalloc(sizeof(struct file_descriptor_t));
      // Descriptors start at 1
      desc->index = i + 1;
      file_descriptors[i] = desc;
      *desc_out = desc;
      res = 0;
      break;
    }
  }

  return res;
}

// Function to get a file descriptor by its index
static struct file_descriptor_t *file_get_descriptor(int fd)
{
  if (fd <= 0 || fd >= MAX_FILE_DESCRIPTORS)
  {
    return 0;
  }

  // Descriptors start at 1
  int index = fd - 1;
  return file_descriptors[index];
}

// Function to resolve the filesystem for a disk
struct filesystem_t *fs_resolve(struct disk_t *disk)
{
  
  struct filesystem_t *fs = 0;
  for (int i = 0; i < MAX_FILESYSTEMS; i++)
  {
    if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0)
    {
      fs = filesystems[i];
      break;
    }
  }
  return fs;
}

// Function to convert a mode string to a FILE_MODE enum value
FILE_MODE file_get_mode_by_string(const char *str)
{
  FILE_MODE mode = FILE_MODE_INVALID;
  if (strncmp(str, "r", 1) == 0)
  {
    mode = FILE_MODE_READ;
  }
  else if (strncmp(str, "w", 1) == 0)
  {
    mode = FILE_MODE_WRITE;
  }
  else if (strncmp(str, "a", 1) == 0)
  {
    mode = FILE_MODE_APPEND;
  }
  return mode;
}

// Function to open a file
int fopen(const char *filename, const char *mode_str)
{
  int res = 0;
  struct path_root_t *root_path = parser_parse(filename, NULL);
  if (!root_path)
  {
    res = -EINVARG;
    goto out;
  }

  // We cannot have just a root path 0:/ 0:/test.txt
  if (!root_path->first)
  {
    res = -EINVARG;
    goto out;
  }

  // Ensure the disk we are reading from exists
  struct disk_t *disk = disk_get(root_path->drive_no);
  if (!disk)
  {
    res = -EIO;
    goto out;
  }

  if (!disk->filesystem)
  {
    res = -EIO;
    goto out;
  }

  FILE_MODE mode = file_get_mode_by_string(mode_str);
  if (mode == FILE_MODE_INVALID)
  {
    res = -EINVARG;
    goto out;
  }

  void *descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);
  if (ISERR(descriptor_private_data))
  {
    res = ERROR_I(descriptor_private_data);
    goto out;
  }

  struct file_descriptor_t *desc = 0;
  res = file_new_descriptor(&desc);
  if (res < 0)
  {
    goto out;
  }
  desc->filesystem = disk->filesystem;
  desc->private = descriptor_private_data;
  desc->disk = disk;
  res = desc->index;

out:
  // fopen shouldnt return negative values
  if (res < 0)
    res = 0;

  return res;
}

// Function to get the file stat
int fstat(int fd, struct file_stat_t *stat)
{
  int res = 0;
  struct file_descriptor_t *desc = file_get_descriptor(fd);
  if (!desc)
  {
    res = -EIO;
    goto out;
  }

  res = desc->filesystem->stat(desc->disk, desc->private, stat);
out:
  return res;
}

// Function to close a file
int fclose(int fd)
{
  int res = 0;
  struct file_descriptor_t *desc = file_get_descriptor(fd);
  if (!desc)
  {
    res = -EIO;
    goto out;
  }

  res = desc->filesystem->close(desc->private);
  if (res == ALL_OK)
  {
    file_free_descriptor(desc);
  }
out:
  return res;
}

// Function to set the file position indicator
int fseek(int fd, int offset, FILE_SEEK_MODE whence)
{
  int res = 0;
  struct file_descriptor_t *desc = file_get_descriptor(fd);
  if (!desc)
  {
    res = -EIO;
    goto out;
  }

  res = desc->filesystem->seek(desc->private, offset, whence);
out:
  return res;
}

// Function to read data from a file
int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd)
{
  int res = 0;
  if (size == 0 || nmemb == 0 || fd < 1)
  {
    res = -EINVARG;
    goto out;
  }

  struct file_descriptor_t *desc = file_get_descriptor(fd);
  if (!desc)
  {
    res = -EINVARG;
    goto out;
  }

  res = desc->filesystem->read(desc->disk, desc->private, size, nmemb, (char *)ptr);
out:
  return res;
}
