#ifndef DISK_STREAM_H
#define DISK_STREAM_H

#include "disk.h"

struct disk_stream_t
{
  int pos;
  struct disk_t *disk;
};

struct disk_stream_t *new_disk_stream(int disk_id);
int disk_stream_seek(struct disk_stream_t *stream, int pos);
int disk_stream_read(struct disk_stream_t *stream, void *out, int total);
void disk_stream_close(struct disk_stream_t *stream);

#endif