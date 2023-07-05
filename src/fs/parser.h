#ifndef PARSER_H
#define PARSER_H

struct path_root
{
  int drive_no;
  struct path_part *first;
};

struct path_part
{
  const char *part;
  struct path_part *next;
};

struct path_root *parser_parse(const char *path, const char *current_directory_path);
void parser_free(struct path_root *root);

#endif