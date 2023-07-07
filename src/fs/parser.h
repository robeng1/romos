#ifndef PARSER_H
#define PARSER_H

struct path_root_t
{
  int drive_no;
  struct path_part_t *first;
};

struct path_part_t
{
  const char *part;
  struct path_part_t *next;
};

struct path_root_t *parser_parse(const char *path, const char *current_directory_path);
void parser_free(struct path_root_t *root);

#endif