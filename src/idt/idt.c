#include "idt.h"
#include "config.h"
#include "io/io.h"
#include "kernel/kernel.h"
#include "mm/memory.h"
#include "task/task.h"
#include "task/process.h"
#include "status.h"

struct idt_entry_t idt_descriptors[ROMOS_TOTAL_INTERRUPTS];
struct idt_ptr_t idt_ptr_t;

extern void *interrupt_pointer_table[ROMOS_TOTAL_INTERRUPTS];

static interrupt_callback_t interrupt_callbacks[ROMOS_TOTAL_INTERRUPTS];

static isr80h_cmd_t isr80h_commands[ROMOS_MAX_ISR80H_COMMANDS];

extern void load_idt(struct idt_ptr_t *ptr);

extern void no_interrupt();
extern void isr80h_wrapper();

void no_interrupt_handler()
{
  write_byte(0x20, 0x20);
}

void interrupt_handler(int interrupt, struct interrupt_frame_t *frame)
{
  switch_to_kernel_page();
  if (interrupt_callbacks[interrupt] != 0)
  {
    task_current_save_state(frame);
    interrupt_callbacks[interrupt](frame);
  }

  switch_to_task_page();
  write_byte(0x20, 0x20);
}

void set_idt(int interrupt_no, void *address)
{
  struct idt_entry_t *entry = &idt_descriptors[interrupt_no];
  entry->offset_1 = (uint32_t)address & 0x0000ffff;
  entry->selector = KERNEL_CODE_SELECTOR;
  entry->zero = 0x00;
  entry->type_attr = 0xEE;
  entry->offset_2 = (uint32_t)address >> 16;
}

void idt_handle_exception()
{
  process_terminate(task_current()->process);
  task_next();
}

void idt_clock()
{
  write_byte(0x20, 0x20);

  // Switch to the next task
  task_next();
}

void init_idt()
{
  memset(idt_descriptors, 0, sizeof(idt_descriptors));

  idt_ptr_t.limit = sizeof(idt_descriptors) - 1;
  idt_ptr_t.base = (uint32_t)idt_descriptors;

  for (int i = 0; i < ROMOS_TOTAL_INTERRUPTS; i++)
  {
    set_idt(i, interrupt_pointer_table[i]);
  }

  set_idt(0x80, isr80h_wrapper);

  for (int i = 0; i < 0x20; i++)
  {
    idt_register_interrupt_callback(i, idt_handle_exception);
  }

  idt_register_interrupt_callback(0x20, idt_clock);

  // Load the interrupt descriptor table
  load_idt(&idt_ptr_t);
}

int idt_register_interrupt_callback(int interrupt, interrupt_callback_t interrupt_callback)
{
  if (interrupt < 0 || interrupt >= ROMOS_TOTAL_INTERRUPTS)
  {
    return -EINVARG;
  }

  interrupt_callbacks[interrupt] = interrupt_callback;
  return 0;
}

void isr80h_register_command(int id, isr80h_cmd_t command)
{
  if (id < 0 || id >= ROMOS_MAX_ISR80H_COMMANDS)
  {
    panic("The command is out of bounds\n");
  }

  if (isr80h_commands[id])
  {
    panic("Your attempting to overwrite an existing command\n");
  }

  isr80h_commands[id] = command;
}

void *isr80h_handle_command(int command, struct interrupt_frame_t *frame)
{
  void *result = 0;

  if (command < 0 || command >= ROMOS_MAX_ISR80H_COMMANDS)
  {
    // Invalid command
    return 0;
  }

  isr80h_cmd_t handler = isr80h_commands[command];
  if (!handler)
  {
    return 0;
  }

  result = handler(frame);
  return result;
}

void *isr80h_handler(int command, struct interrupt_frame_t *frame)
{
  void *res = 0;
  switch_to_kernel_page();
  task_current_save_state(frame);
  res = isr80h_handle_command(command, frame);
  switch_to_task_page();
  return res;
}