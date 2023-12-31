#include "io.h"
#include "task/task.h"
#include "drivers/keyboard/keyboard.h"
#include "kernel/kernel.h"

void *isr80h_io_cmd_print(struct interrupt_frame_t *frame)
{
  void *user_space_msg_buffer = task_get_stack_item(task_current(), 0);
  char buf[1024];
  copy_string_from_task(task_current(), user_space_msg_buffer, buf, sizeof(buf));

  print(buf);
  return 0;
}

void *isr80h_io_cmd_getkey(struct interrupt_frame_t *frame)
{
  char c = keyboard_pop();
  return (void *)((int)c);
}

void *isr80h_io_cmd_putchar(struct interrupt_frame_t *frame)
{
  char c = (char)(int)task_get_stack_item(task_current(), 0);
  term_writechar(c);
  return 0;
}