#include "status.h"
#include "kernel/kernel.h"
#include "task/process.h"
#include "task/task.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/keyboard/classic.h"

static struct keyboard_t *keyboard_list_head = 0;
static struct keyboard_t *keyboard_list_last = 0;

void keyboard_init()
{
  keyboard_insert(classic_init());
}

int keyboard_insert(struct keyboard_t *keyboard)
{
  int res = 0;
  if (keyboard->init == 0)
  {
    res = -EINVARG;
    goto out;
  }

  if (keyboard_list_last)
  {
    keyboard_list_last->next = keyboard;
    keyboard_list_last = keyboard;
  }
  else
  {
    keyboard_list_head = keyboard;
    keyboard_list_last = keyboard;
  }

  res = keyboard->init();
out:
  return res;
}

static int keyboard_get_tail_index(struct process_t *process)
{
  return process->keyboard.tail % sizeof(process->keyboard.buffer);
}

void keyboard_backspace(struct process_t *process)
{
  process->keyboard.tail -= 1;
  int real_index = keyboard_get_tail_index(process);
  process->keyboard.buffer[real_index] = 0x00;
}

void keyboard_set_capslock(struct keyboard_t *keyboard, KEYBOARD_CAPS_LOCK_STATE state)
{
  keyboard->capslock_state = state;
}

KEYBOARD_CAPS_LOCK_STATE keyboard_get_capslock(struct keyboard_t *keyboard)
{
  return keyboard->capslock_state;
}

void keyboard_push(char c)
{
  struct process_t *process = process_current();
  if (!process)
  {
    return;
  }

  if (c == 0)
  {
    return;
  }

  int real_index = keyboard_get_tail_index(process);
  process->keyboard.buffer[real_index] = c;
  process->keyboard.tail++;
}

char keyboard_pop()
{
  if (!task_current())
  {
    return 0;
  }

  struct process_t *process = task_current()->process;
  int real_index = process->keyboard.head % sizeof(process->keyboard.buffer);
  char c = process->keyboard.buffer[real_index];
  if (c == 0x00)
  {
    // Nothing to pop return zero.
    return 0;
  }

  process->keyboard.buffer[real_index] = 0;
  process->keyboard.head++;
  return c;
}