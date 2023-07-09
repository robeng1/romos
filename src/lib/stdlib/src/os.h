#ifndef STDLIB_OS_H
#define STDLIB_OS_H
#include <stddef.h>
#include <stdbool.h>

struct command_argument
{
  char argument[512];
  struct command_argument *next;
};

struct process_arguments_t
{
  int argc;
  char **argv;
};

void print(const char *filename);
int sys_getkey();

extern void *sys_malloc(size_t size);
extern void sys_free(void *ptr);
extern void sys_putchar(char c);
int sys_getkeyblock();
void sys_terminal_readline(char *out, int max, bool output_while_typing);
extern void sys_process_load_start(const char *filename);
struct command_argument *sys_parse_command(const char *command, int max);
extern void sys_process_get_arguments(struct process_arguments_t *arguments);
extern int sys_system(struct command_argument *arguments);
int sys_system_run(const char *command);
extern void sys_exit();
#endif