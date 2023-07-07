// Include necessary headers
#include "fat16.h"
#include "string/string.h"
#include "disk/disk.h"
#include "disk/stream.h"
#include "mm/heap/kernel_heap.h"
#include "mm/memory.h"
#include "status.h"
#include "kernel/kernel.h"
#include "config.h"
#include <stdint.h>

// Define some constants
#define ROMOS_FAT16_SIGNATURE 0x29      // The FAT16 signature
#define ROMOS_FAT16_FAT_ENTRY_SIZE 0x02 // The size of a FAT entry in FAT16
#define ROMOS_FAT16_BAD_SECTOR 0xFF7    // Indication of a bad sector in FAT16
#define ROMOS_FAT16_UNUSED 0x00         // Indication of an unused sector in FAT16

// Define a type for FAT items
typedef unsigned int FAT_ITEM_TYPE;
#define FAT_ITEM_TYPE_DIRECTORY 0 // Value indicating a directory item type
#define FAT_ITEM_TYPE_FILE 1      // Value indicating a file item type

// Fat directory entry attributes bitmask
#define FAT_FILE_READ_ONLY 0x01    // Read-only file attribute bit
#define FAT_FILE_HIDDEN 0x02       // Hidden file attribute bit
#define FAT_FILE_SYSTEM 0x04       // System file attribute bit
#define FAT_FILE_VOLUME_LABEL 0x08 // Volume label attribute bit
#define FAT_FILE_SUBDIRECTORY 0x10 // Subdirectory attribute bit
#define FAT_FILE_ARCHIVED 0x20     // Archived attribute bit
#define FAT_FILE_DEVICE 0x40       // Device attribute bit
#define FAT_FILE_RESERVED 0x80     // Reserved attribute bit

// Define the structure of the FAT16 extended header
struct fat_header_extended_t
{
  uint8_t drive_number;         // Drive number
  uint8_t win_nt_bit;           // Windows NT specific bit
  uint8_t signature;            // Signature, should be ROMOS_FAT16_SIGNATURE
  uint32_t volume_id;           // Volume ID
  uint8_t volume_id_string[11]; // Volume ID as string
  uint8_t system_id_string[8];  // System ID as string
} __attribute__((packed));      // Ensure the struct is packed with no padding

// Define the structure of the FAT16 header
struct fat_header_t
{
  uint8_t short_jmp_ins[3];    // Short jump instruction for boot sector
  uint8_t oem_identifier[8];   // OEM identifier
  uint16_t bytes_per_sector;   // Bytes per sector
  uint8_t sectors_per_cluster; // Sectors per cluster
  uint16_t reserved_sectors;   // Reserved sectors
  uint8_t fat_copies;          // Number of FAT copies
  uint16_t root_dir_entries;   // Number of root directory entries
  uint16_t number_of_sectors;  // Total number of sectors
  uint8_t media_type;          // Media type
  uint16_t sectors_per_fat;    // Sectors per FAT
  uint16_t sectors_per_track;  // Sectors per track
  uint16_t number_of_heads;    // Number of heads
  uint32_t hidden_setors;      // Number of hidden sectors
  uint32_t sectors_big;        // If number_of_sectors is 0, this value will contain the total sectors count
} __attribute__((packed));     // Ensure the struct is packed with no padding

// Define a helper structure that unifies the primary and extended FAT16 headers
struct fat_h_t
{
  struct fat_header_t primary_header; // Primary header
  union fat_h_e
  {
    struct fat_header_extended_t extended_header; // Extended header
  } shared;
};

// Define the structure of a FAT16 directory item
struct fat_directory_item_t
{
  uint8_t filename[8];                   // Filename (8.3 format)
  uint8_t ext[3];                        // Extension (8.3 format)
  uint8_t attribute;                     // File attributes
  uint8_t reserved;                      // Reserved byte
  uint8_t creation_time_tenths_of_a_sec; // File creation time (tenths of a second part)
  uint16_t creation_time;                // File creation time
  uint16_t creation_date;                // File creation date
  uint16_t last_access;                  // Date of last access
  uint16_t high_16_bits_first_cluster;   // High 16 bits of the first cluster number (for FAT32, not used in FAT16)
  uint16_t last_mod_time;                // Time of last modification
  uint16_t last_mod_date;                // Date of last modification
  uint16_t low_16_bits_first_cluster;    // Low 16 bits of the first cluster number
  uint32_t filesize;                     // Size of the file
} __attribute__((packed));               // Ensure the struct is packed with no padding

// Define the structure of a FAT16 directory
struct fat_directory_t
{
  struct fat_directory_item_t *item; // Pointer to directory items
  int total;                         // Total number of items in the directory
  int sector_pos;                    // Sector position of the directory
  int ending_sector_pos;             // Ending sector position of the directory
};

// Define a unified FAT item structure
struct fat_item_t
{
  union
  {
    struct fat_directory_item_t *item; // Pointer to a directory item (if this is a file)
    struct fat_directory_t *directory; // Pointer to a directory (if this is a directory)
  };

  FAT_ITEM_TYPE type; // Type of the item (file or directory)
};

// Define the structure of a FAT16 file descriptor
struct fat_file_descriptor_t
{
  struct fat_item_t *item; // Pointer to the associated FAT item
  uint32_t pos;            // Current read/write position in the file
};

// Define private structure for FAT16
struct fat_private_t
{
  struct fat_h_t header;                 // FAT16 header
  struct fat_directory_t root_directory; // Root directory

  // Used to stream data clusters
  struct disk_stream_t *cluster_read_stream;
  // Used to stream the file allocation table
  struct disk_stream_t *fat_read_stream;

  // Used in situations where we stream the directory
  struct disk_stream_t *directory_stream;
};

// Function prototypes
int fat16_resolve(struct disk_t *disk);
void *fat16_open(struct disk_t *disk, struct path_part_t *path, FILE_MODE mode);
int fat16_read(struct disk_t *disk, void *descriptor, uint32_t size, uint32_t nmemb, char *out_ptr);
int fat16_seek(void *private, uint32_t offset, FILE_SEEK_MODE seek_mode);
int fat16_stat(struct disk_t *disk, void *private, struct file_stat_t *stat);
int fat16_close(void *private);

// Filesystem structure for FAT16
struct filesystem_t fat16_fs =
    {
        .resolve = fat16_resolve,
        .open = fat16_open,
        .read = fat16_read,
        .seek = fat16_seek,
        .stat = fat16_stat,
        .close = fat16_close};

// Initialize FAT16 filesystem
struct filesystem_t *fat16_init()
{
  strcpy(fat16_fs.name, "FAT16"); // Set the filesystem name
  return &fat16_fs;               // Return the filesystem struct
}

static void fat16_init_private(struct disk_t *disk, struct fat_private_t *private)
{
  memset(private, 0, sizeof(struct fat_private_t));
  private
      ->cluster_read_stream = new_disk_stream(disk->id);
  private
      ->fat_read_stream = new_disk_stream(disk->id);
  private
      ->directory_stream = new_disk_stream(disk->id);
}

int fat16_sector_to_absolute(struct disk_t *disk, int sector)
{
  return sector * disk->sector_size;
}

int fat16_get_total_items_for_directory(struct disk_t *disk, uint32_t directory_start_sector)
{
  struct fat_directory_item_t item;
  struct fat_directory_item_t empty_item;
  memset(&empty_item, 0, sizeof(empty_item));

  struct fat_private_t *fat_private = disk->fs_private;

  int res = 0;
  int i = 0;
  int directory_start_pos = directory_start_sector * disk->sector_size;
  struct disk_stream_t *stream = fat_private->directory_stream;
  if (disk_stream_seek(stream, directory_start_pos) != ROMOS_ALL_OK)
  {
    res = -EIO;
    goto out;
  }

  while (1)
  {
    if (disk_stream_read(stream, &item, sizeof(item)) != ROMOS_ALL_OK)
    {
      res = -EIO;
      goto out;
    }

    if (item.filename[0] == 0x00)
    {
      // We are done
      break;
    }

    // Is the item unused
    if (item.filename[0] == 0xE5)
    {
      continue;
    }

    i++;
  }

  res = i;

out:
  return res;
}

int fat16_get_root_directory(struct disk_t *disk, struct fat_private_t *fat_private, struct fat_directory_t *directory)
{
  int res = 0;
  struct fat_directory_item_t *dir = 0x00;
  struct fat_header_t *primary_header = &fat_private->header.primary_header;
  int root_dir_sector_pos = (primary_header->fat_copies * primary_header->sectors_per_fat) + primary_header->reserved_sectors;
  int root_dir_entries = fat_private->header.primary_header.root_dir_entries;
  int root_dir_size = (root_dir_entries * sizeof(struct fat_directory_item_t));
  int total_sectors = root_dir_size / disk->sector_size;
  if (root_dir_size % disk->sector_size)
  {
    total_sectors += 1;
  }

  int total_items = fat16_get_total_items_for_directory(disk, root_dir_sector_pos);

  dir = kernel_zalloc(root_dir_size);
  if (!dir)
  {
    res = -ENOMEM;
    goto err_out;
  }

  struct disk_stream_t *stream = fat_private->directory_stream;
  if (disk_stream_seek(stream, fat16_sector_to_absolute(disk, root_dir_sector_pos)) != ROMOS_ALL_OK)
  {
    res = -EIO;
    goto err_out;
  }

  if (disk_stream_read(stream, dir, root_dir_size) != ROMOS_ALL_OK)
  {
    res = -EIO;
    goto err_out;
  }

  directory->item = dir;
  directory->total = total_items;
  directory->sector_pos = root_dir_sector_pos;
  directory->ending_sector_pos = root_dir_sector_pos + (root_dir_size / disk->sector_size);
out:
  return res;

err_out:
  if (dir)
  {
    kernel_free(dir);
  }

  return res;
}

int fat16_resolve(struct disk_t *disk)
{
  int res = 0;
  struct fat_private_t *fat_private = kernel_zalloc(sizeof(struct fat_private_t));
  fat16_init_private(disk, fat_private);

  disk->fs_private = fat_private;
  disk->filesystem = &fat16_fs;

  struct disk_stream_t *stream = new_disk_stream(disk->id);
  if (!stream)
  {
    res = -ENOMEM;
    goto out;
  }

  if (disk_stream_read(stream, &fat_private->header, sizeof(fat_private->header)) != ROMOS_ALL_OK)
  {
    res = -EIO;
    goto out;
  }

  if (fat_private->header.shared.extended_header.signature != 0x29)
  {
    res = -EFSNOTUS;
    goto out;
  }

  if (fat16_get_root_directory(disk, fat_private, &fat_private->root_directory) != ROMOS_ALL_OK)
  {
    res = -EIO;
    goto out;
  }

out:
  if (stream)
  {
    disk_stream_close(stream);
  }

  if (res < 0)
  {
    kernel_free(fat_private);
    disk->fs_private = 0;
  }
  return res;
}

void fat16_to_proper_string(char **out, const char *in, size_t size)
{
  int i = 0;
  while (*in != 0x00 && *in != 0x20)
  {
    **out = *in;
    *out += 1;
    in += 1;
    // We cant process anymore since we have exceeded the input buffer size
    if (i >= size - 1)
    {
      break;
    }
    i++;
  }

  **out = 0x00;
}

void fat16_get_full_relative_filename(struct fat_directory_item_t *item, char *out, int max_len)
{
  memset(out, 0x00, max_len);
  char *out_tmp = out;
  fat16_to_proper_string(&out_tmp, (const char *)item->filename, sizeof(item->filename));
  if (item->ext[0] != 0x00 && item->ext[0] != 0x20)
  {
    *out_tmp++ = '.';
    fat16_to_proper_string(&out_tmp, (const char *)item->ext, sizeof(item->ext));
  }
}

struct fat_directory_item_t *fat16_clone_directory_item(struct fat_directory_item_t *item, int size)
{
  struct fat_directory_item_t *item_copy = 0;
  if (size < sizeof(struct fat_directory_item_t))
  {
    return 0;
  }

  item_copy = kernel_zalloc(size);
  if (!item_copy)
  {
    return 0;
  }

  memcpy(item_copy, item, size);
  return item_copy;
}

static uint32_t fat16_get_first_cluster(struct fat_directory_item_t *item)
{
  return (item->high_16_bits_first_cluster) | item->low_16_bits_first_cluster;
};

static int fat16_cluster_to_sector(struct fat_private_t *private, int cluster)
{
  return private->root_directory.ending_sector_pos + ((cluster - 2) * private->header.primary_header.sectors_per_cluster);
}

static uint32_t fat16_get_first_fat_sector(struct fat_private_t *private)
{
  return private->header.primary_header.reserved_sectors;
}

static int fat16_get_fat_entry(struct disk_t *disk, int cluster)
{
  int res = -1;
  struct fat_private_t *private = disk->fs_private;
  struct disk_stream_t *stream = private->fat_read_stream;
  if (!stream)
  {
    goto out;
  }

  uint32_t fat_table_position = fat16_get_first_fat_sector(private) * disk->sector_size;
  res = disk_stream_seek(stream, fat_table_position * (cluster * ROMOS_FAT16_FAT_ENTRY_SIZE));
  if (res < 0)
  {
    goto out;
  }

  uint16_t result = 0;
  res = disk_stream_read(stream, &result, sizeof(result));
  if (res < 0)
  {
    goto out;
  }

  res = result;
out:
  return res;
}
/**
 * Gets the correct cluster to use based on the starting cluster and the offset
 */
static int fat16_get_cluster_for_offset(struct disk_t *disk, int starting_cluster, int offset)
{
  int res = 0;
  struct fat_private_t *private = disk->fs_private;
  int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
  int cluster_to_use = starting_cluster;
  int clusters_ahead = offset / size_of_cluster_bytes;
  for (int i = 0; i < clusters_ahead; i++)
  {
    int entry = fat16_get_fat_entry(disk, cluster_to_use);
    if (entry == 0xFF8 || entry == 0xFFF)
    {
      // We are at the last entry in the file
      res = -EIO;
      goto out;
    }

    // Sector is marked as bad?
    if (entry == ROMOS_FAT16_BAD_SECTOR)
    {
      res = -EIO;
      goto out;
    }

    // Reserved sector?
    if (entry == 0xFF0 || entry == 0xFF6)
    {
      res = -EIO;
      goto out;
    }

    if (entry == 0x00)
    {
      res = -EIO;
      goto out;
    }

    cluster_to_use = entry;
  }

  res = cluster_to_use;
out:
  return res;
}
static int fat16_read_internal_from_stream(struct disk_t *disk, struct disk_stream_t *stream, int cluster, int offset, int total, void *out)
{
  int res = 0;
  struct fat_private_t *private = disk->fs_private;
  int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
  int cluster_to_use = fat16_get_cluster_for_offset(disk, cluster, offset);
  if (cluster_to_use < 0)
  {
    res = cluster_to_use;
    goto out;
  }

  int offset_from_cluster = offset % size_of_cluster_bytes;

  int starting_sector = fat16_cluster_to_sector(private, cluster_to_use);
  int starting_pos = (starting_sector * disk->sector_size) + offset_from_cluster;
  int total_to_read = total > size_of_cluster_bytes ? size_of_cluster_bytes : total;
  res = disk_stream_seek(stream, starting_pos);
  if (res != ROMOS_ALL_OK)
  {
    goto out;
  }

  res = disk_stream_read(stream, out, total_to_read);
  if (res != ROMOS_ALL_OK)
  {
    goto out;
  }

  total -= total_to_read;
  if (total > 0)
  {
    // We still have more to read
    res = fat16_read_internal_from_stream(disk, stream, cluster, offset + total_to_read, total, out + total_to_read);
  }

out:
  return res;
}

static int fat16_read_internal(struct disk_t *disk, int starting_cluster, int offset, int total, void *out)
{
  struct fat_private_t *fs_private = disk->fs_private;
  struct disk_stream_t *stream = fs_private->cluster_read_stream;
  return fat16_read_internal_from_stream(disk, stream, starting_cluster, offset, total, out);
}

void fat16_free_directory(struct fat_directory_t *directory)
{
  if (!directory)
  {
    return;
  }

  if (directory->item)
  {
    kernel_free(directory->item);
  }

  kernel_free(directory);
}

void fat16_fat_item_free(struct fat_item_t *item)
{
  if (item->type == FAT_ITEM_TYPE_DIRECTORY)
  {
    fat16_free_directory(item->directory);
  }
  else if (item->type == FAT_ITEM_TYPE_FILE)
  {
    kernel_free(item->item);
  }

  kernel_free(item);
}

struct fat_directory_t *fat16_load_fat_directory(struct disk_t *disk, struct fat_directory_item_t *item)
{
  int res = 0;
  struct fat_directory_t *directory = 0;
  struct fat_private_t *fat_private = disk->fs_private;
  if (!(item->attribute & FAT_FILE_SUBDIRECTORY))
  {
    res = -EINVARG;
    goto out;
  }

  directory = kernel_zalloc(sizeof(struct fat_directory_t));
  if (!directory)
  {
    res = -ENOMEM;
    goto out;
  }

  int cluster = fat16_get_first_cluster(item);
  int cluster_sector = fat16_cluster_to_sector(fat_private, cluster);
  int total_items = fat16_get_total_items_for_directory(disk, cluster_sector);
  directory->total = total_items;
  int directory_size = directory->total * sizeof(struct fat_directory_item_t);
  directory->item = kernel_zalloc(directory_size);
  if (!directory->item)
  {
    res = -ENOMEM;
    goto out;
  }

  res = fat16_read_internal(disk, cluster, 0x00, directory_size, directory->item);
  if (res != ROMOS_ALL_OK)
  {
    goto out;
  }

out:
  if (res != ROMOS_ALL_OK)
  {
    fat16_free_directory(directory);
  }
  return directory;
}
struct fat_item_t *fat16_new_fat_item_for_directory_item(struct disk_t *disk, struct fat_directory_item_t *item)
{
  struct fat_item_t *f_item = kernel_zalloc(sizeof(struct fat_item_t));
  if (!f_item)
  {
    return 0;
  }

  if (item->attribute & FAT_FILE_SUBDIRECTORY)
  {
    f_item->directory = fat16_load_fat_directory(disk, item);
    f_item->type = FAT_ITEM_TYPE_DIRECTORY;
    return f_item;
  }

  f_item->type = FAT_ITEM_TYPE_FILE;
  f_item->item = fat16_clone_directory_item(item, sizeof(struct fat_directory_item_t));
  return f_item;
}

struct fat_item_t *fat16_find_item_in_directory(struct disk_t *disk, struct fat_directory_t *directory, const char *name)
{
  struct fat_item_t *f_item = 0;
  char tmp_filename[ROMOS_MAX_PATH];
  for (int i = 0; i < directory->total; i++)
  {
    fat16_get_full_relative_filename(&directory->item[i], tmp_filename, sizeof(tmp_filename));
    if (istrncmp(tmp_filename, name, sizeof(tmp_filename)) == 0)
    {
      // Found it let's create a new fat_item
      f_item = fat16_new_fat_item_for_directory_item(disk, &directory->item[i]);
    }
  }

  return f_item;
}
struct fat_item_t *fat16_get_directory_entry(struct disk_t *disk, struct path_part_t *path)
{
  struct fat_private_t *fat_private = disk->fs_private;
  struct fat_item_t *current_item = 0;
  struct fat_item_t *root_item = fat16_find_item_in_directory(disk, &fat_private->root_directory, path->part);
  if (!root_item)
  {
    goto out;
  }

  struct path_part_t *next_part = path->next;
  current_item = root_item;
  while (next_part != 0)
  {
    if (current_item->type != FAT_ITEM_TYPE_DIRECTORY)
    {
      current_item = 0;
      break;
    }

    struct fat_item_t *tmp_item = fat16_find_item_in_directory(disk, current_item->directory, next_part->part);
    fat16_fat_item_free(current_item);
    current_item = tmp_item;
    next_part = next_part->next;
  }
out:
  return current_item;
}

void *fat16_open(struct disk_t *disk, struct path_part_t *path, FILE_MODE mode)
{
  struct fat_file_descriptor_t *descriptor = 0;
  int err_code = 0;
  if (mode != FILE_MODE_READ)
  {
    err_code = -ERDONLY;
    goto err_out;
  }

  descriptor = kernel_zalloc(sizeof(struct fat_file_descriptor_t));
  if (!descriptor)
  {
    err_code = -ENOMEM;
    goto err_out;
  }

  descriptor->item = fat16_get_directory_entry(disk, path);
  if (!descriptor->item)
  {
    err_code = -EIO;
    goto err_out;
  }

  descriptor->pos = 0;
  return descriptor;

err_out:
  if (descriptor)
    kernel_free(descriptor);

  return ERROR(err_code);
}

static void fat16_free_file_descriptor(struct fat_file_descriptor_t *desc)
{
  fat16_fat_item_free(desc->item);
  kernel_free(desc);
}

int fat16_close(void *private)
{
  fat16_free_file_descriptor((struct fat_file_descriptor_t *)private);
  return 0;
}

int fat16_stat(struct disk_t *disk, void *private, struct file_stat_t *stat)
{
  int res = 0;
  struct fat_file_descriptor_t *descriptor = (struct fat_file_descriptor_t *)private;
  struct fat_item_t *desc_item = descriptor->item;
  if (desc_item->type != FAT_ITEM_TYPE_FILE)
  {
    res = -EINVARG;
    goto out;
  }

  struct fat_directory_item_t *ritem = desc_item->item;
  stat->filesize = ritem->filesize;
  stat->flags = 0x00;

  if (ritem->attribute & FAT_FILE_READ_ONLY)
  {
    stat->flags |= FILE_STAT_READ_ONLY;
  }
out:
  return res;
}

int fat16_read(struct disk_t *disk, void *descriptor, uint32_t size, uint32_t nmemb, char *out_ptr)
{
  int res = 0;
  struct fat_file_descriptor_t *fat_desc = descriptor;
  struct fat_directory_item_t *item = fat_desc->item->item;
  int offset = fat_desc->pos;
  for (uint32_t i = 0; i < nmemb; i++)
  {
    res = fat16_read_internal(disk, fat16_get_first_cluster(item), offset, size, out_ptr);
    if (ISERR(res))
    {
      goto out;
    }

    out_ptr += size;
    offset += size;
  }

  res = nmemb;
out:
  return res;
}

int fat16_seek(void *private, uint32_t offset, FILE_SEEK_MODE seek_mode)
{
  int res = 0;
  struct fat_file_descriptor_t *desc = private;
  struct fat_item_t *desc_item = desc->item;
  if (desc_item->type != FAT_ITEM_TYPE_FILE)
  {
    res = -EINVARG;
    goto out;
  }

  struct fat_directory_item_t *ritem = desc_item->item;
  if (offset >= ritem->filesize)
  {
    res = -EIO;
    goto out;
  }

  switch (seek_mode)
  {
  case SEEK_SET:
    desc->pos = offset;
    break;

  case SEEK_END:
    res = -EUNIMP;
    break;

  case SEEK_CUR:
    desc->pos += offset;
    break;

  default:
    res = -EINVARG;
    break;
  }
out:
  return res;
}