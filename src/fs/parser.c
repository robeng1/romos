#include "parser.h"                  // Include header file for parser functionality
#include "kernel.h"                  // Include header file for kernel functionality
#include "string/string.h"           // Include header file for string manipulation
#include "memory/heap/kernel_heap.h" // Include header file for kernel heap functionality
#include "memory/memory.h"           // Include header file for memory management
#include "status.h"                  // Include header file for status codes
#include "config.h"                  // Include header file for configuration

static int parser_path_valid_format(const char *filename)
{
  int len = strnlen(filename, ROMOS_MAX_PATH); // Get the length of the filename up to ROMOS_MAX_PATH characters
  return (len >= 3 && isdigit(filename[0]) && memcmp((void *)&filename[1], ":/", 2) == 0);
  // Check if the filename has a valid format: digit:/path
}

static int parser_get_drive_by_path(const char **path)
{
  if (!parser_path_valid_format(*path))
  {
    return -EBADPATH; // If the path is not in a valid format, return an error code
  }

  int drive_no = tonumericdigit(*path[0]); // Get the numeric value of the drive number

  // Add 3 bytes to skip drive number 0:/ 1:/ 2:/
  *path += 3;      // Update the path pointer to skip the drive number and the delimiter
  return drive_no; // Return the drive number
}

static struct path_root *parser_create_root(int drive_number)
{
  struct path_root *path_r = kernel_zalloc(sizeof(struct path_root));
  // Allocate memory for the path_root structure
  path_r->drive_no = drive_number; // Set the drive number in the path_root structure
  path_r->first = 0;               // Set the first path_part pointer to NULL
  return path_r;                   // Return the created path_root structure
}

static const char *parser_get_path_part(const char **path)
{
  char *result_path_part = kernel_zalloc(ROMOS_MAX_PATH);
  // Allocate memory for the path part
  int i = 0;
  while (**path != '/' && **path != 0x00)
  {
    result_path_part[i] = **path; // Copy each character of the path part to the result_path_part
    *path += 1;                   // Move the path pointer forward
    i++;
  }

  if (**path == '/')
  {
    // Skip the forward slash to avoid problems
    *path += 1; // Move the path pointer forward to skip the delimiter
  }

  if (i == 0)
  {
    kernel_free(result_path_part);
    result_path_part = 0;
  }

  return result_path_part; // Return the extracted path part
}

struct path_part *parser_parse_path_part(struct path_part *last_part, const char **path)
{
  const char *path_part_str = parser_get_path_part(path);
  // Get the path part from the path
  if (!path_part_str)
  {
    return 0; // If the path part is empty, return NULL
  }

  struct path_part *part = kernel_zalloc(sizeof(struct path_part));
  // Allocate memory for the path_part structure
  part->part = path_part_str; // Set the path part in the path_part structure
  part->next = 0x00;          // Set the next path_part pointer to NULL

  if (last_part)
  {
    last_part->next = part; // Set the next pointer of the last path_part to the current path_part
  }

  return part; // Return the created path_part structure
}

void parser_free(struct path_root *root)
{
  struct path_part *part = root->first; // Get the first path_part in the path_root
  while (part)
  {
    struct path_part *next_part = part->next; // Get the next path_part in the path_root
    kernel_free((void *)part->part);          // Free the memory allocated for the path part
    kernel_free(part);                        // Free the memory allocated for the path_part structure
    part = next_part;                         // Move to the next path_part
  }

  kernel_free(root); // Free the memory allocated for the path_root structure
}

struct path_root *parser_parse(const char *path, const char *current_directory_path)
{
  int res = 0;
  const char *tmp_path = path;     // Create a temporary pointer to the path
  struct path_root *path_root = 0; // Initialize the path_root pointer to NULL

  if (strlen(path) > ROMOS_MAX_PATH)
  {
    goto out; // If the length of the path exceeds ROMOS_MAX_PATH, exit the function
  }

  res = parser_get_drive_by_path(&tmp_path); // Get the drive number from the path
  if (res < 0)
  {
    goto out; // If getting the drive number failed, exit the function
  }

  path_root = parser_create_root(res); // Create the path_root structure with the drive number
  if (!path_root)
  {
    goto out; // If creating the path_root failed, exit the function
  }

  struct path_part *first_part = parser_parse_path_part(NULL, &tmp_path);
  // Parse the first path part from the path
  if (!first_part)
  {
    goto out; // If parsing the first path part failed, exit the function
  }

  path_root->first = first_part; // Set the first path_part in the path_root
  struct path_part *part = parser_parse_path_part(first_part, &tmp_path);
  while (part)
  {
    part = parser_parse_path_part(part, &tmp_path); // Parse the remaining path parts
  }

out:
  return path_root; // Return the parsed path_root structure
}
