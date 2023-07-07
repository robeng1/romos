#include "memory.h"
#include "task/task.h"
#include "task/process.h"
#include <stddef.h>

void *isr80h_mem_cmd_malloc(struct interrupt_frame_t *frame)
{
  size_t size = (int)task_get_stack_item(task_current(), 0);
  return process_malloc(task_current()->process, size);
}

void *isr80h_mem_cmd_free(struct interrupt_frame_t *frame)
{
  void *ptr_to_free = task_get_stack_item(task_current(), 0);
  process_free(task_current()->process, ptr_to_free);
  return 0;
}