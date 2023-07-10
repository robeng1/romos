#ifndef STDLIB_OS_H
#define STDLIB_OS_H
#include <stddef.h>
#include <stdbool.h>

struct command_argument_t
{
  char argument[512];
  struct command_argument_t *next;
};

struct process_arguments_t
{
  int argc;
  char **argv;
};

extern void print(const char *filename);
extern int sys_getkey();
extern void *sys_malloc(size_t size);
extern void sys_free(void *ptr);
extern void sys_putchar(char c);
extern void sys_process_load_start(const char *filename);
extern void sys_process_get_arguments(struct process_arguments_t *arguments);
extern int sys_system(struct command_argument_t *arguments);
extern void sys_exit();

int sys_getkeyblock();
void sys_terminal_readline(char *out, int max, bool output_while_typing);
struct command_argument_t *sys_parse_command(const char *command, int max);
int sys_system_run(const char *command);

#endif