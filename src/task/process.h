#ifndef PROCESS_H
#define PROCESS_H
#include <stdint.h>
#include <stdbool.h>

#include <task/task.h>
#include <common/system.h>

#define PROCESS_FILETYPE_ELF 0
#define PROCESS_FILETYPE_BINARY 1

typedef unsigned char PROCESS_FILETYPE;

struct process_allocation_t
{
  void *ptr;
  size_t size;
};

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

struct process_t
{
  // The process id
  uint16_t id;

  char filename[MAX_PATH];

  // The main process task
  struct task_t *task;

  // The memory (malloc) allocations of the process
  struct process_allocation_t allocations[MAX_PROGRAM_ALLOCATIONS];

  PROCESS_FILETYPE filetype;

  union
  {
    // The physical pointer to the process memory.
    void *ptr;
    struct elf_file_t *elf_file;
  };

  // The physical pointer to the stack memory
  void *stack;

  // The size of the data pointed to by "ptr"
  uint32_t size;

  struct keyboard_buffer_t
  {
    char buffer[KEYBOARD_BUFFER_SIZE];
    int tail;
    int head;
  } keyboard;

  // The arguments of the process.
  struct process_arguments_t arguments;
};

int process_switch(struct process_t *process);
int process_load_switch(const char *filename, struct process_t **process);
int process_load(const char *filename, struct process_t **process);
int process_load_for_slot(const char *filename, struct process_t **process, int process_slot);
struct process_t *process_current();
struct process_t *process_get(int process_id);
void *process_malloc(struct process_t *process, size_t size);
void process_free(struct process_t *process, void *ptr);

void process_get_arguments(struct process_t *process, int *argc, char ***argv);
int process_inject_arguments(struct process_t *process, struct command_argument_t *root_argument);
int process_terminate(struct process_t *process);

#endif