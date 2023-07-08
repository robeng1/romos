#include "process.h"
#include "task/task.h"
#include "task/process.h"
#include "string/string.h"
#include "status.h"
#include "config.h"
#include "kernel/kernel.h"

void *isr80h_proc_cmd_process_load_start(struct interrupt_frame_t *frame)
{
  void *filename_user_ptr = task_get_stack_item(task_current(), 0);
  char filename[MAX_PATH];
  int res = copy_string_from_task(task_current(), filename_user_ptr, filename, sizeof(filename));
  if (res < 0)
  {
    goto out;
  }

  char path[MAX_PATH];
  strcpy(path, "0:/");
  strcpy(path + 3, filename);

  struct process_t *process = 0;
  res = process_load_switch(path, &process);
  if (res < 0)
  {
    goto out;
  }

  task_switch(process->task);
  task_return(&process->task->registers);

out:
  return 0;
}

void *isr80h_proc_cmd_invoke_system_command(struct interrupt_frame_t *frame)
{
  struct command_argument_t *arguments = task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));
  if (!arguments || strlen(arguments[0].argument) == 0)
  {
    return ERROR(-EINVARG);
  }

  struct command_argument_t *root_command_argument = &arguments[0];
  const char *program_name = root_command_argument->argument;

  char path[MAX_PATH];
  strcpy(path, "0:/");
  strncpy(path + 3, program_name, sizeof(path));

  struct process_t *process = 0;
  int res = process_load_switch(path, &process);
  if (res < 0)
  {
    return ERROR(res);
  }

  res = process_inject_arguments(process, root_command_argument);
  if (res < 0)
  {
    return ERROR(res);
  }

  task_switch(process->task);
  task_return(&process->task->registers);

  return 0;
}

void *isr80h_proc_cmd_get_program_arguments(struct interrupt_frame_t *frame)
{
  struct process_t *process = task_current()->process;
  struct process_arguments_t *arguments = task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));

  process_get_arguments(process, &arguments->argc, &arguments->argv);
  return 0;
}

void *isr80h_proc_cmd_exit(struct interrupt_frame_t *frame)
{
  struct process_t *process = task_current()->process;
  process_terminate(process);
  task_next();
  return 0;
}