#ifndef DISK_H
#define DISK_H

#include "fs/file.h"

typedef unsigned int ROMOS_DISK_TYPE;

// Represents a real physical hard disk
#define ROMOS_DISK_TYPE_REAL 0

struct disk_t
{
  ROMOS_DISK_TYPE type;
  int sector_size;

  // The id of the disk
  int id;

  struct filesystem_t *filesystem;

  // The private data of our filesystem
  void *fs_private;
};

void disk_search_and_init();
struct disk_t *disk_get(int index);
int disk_read_block(struct disk_t *idisk, unsigned int lba, int total, void *buf);

#endif