#include "task.h"
#include "kernel/kernel.h"
#include "status.h"
#include "config.h"
#include "process.h"
#include "mm/heap/kernel_heap.h"
#include "mm/memory.h"
#include "string/string.h"
#include "mm/paging/paging.h"
#include "idt/idt.h"

// The current task that is running
struct task_t *current_task = 0;

// Task linked list
struct task_t *task_tail = 0;
struct task_t *task_head = 0;

int task_init(struct task_t *task, struct process_t *process);

struct task_t *task_current()
{
  return current_task;
}

struct task_t *new_task(struct process_t *process)
{
  int res = 0;
  struct task_t *task = kernel_zalloc(sizeof(struct task_t));
  if (!task)
  {
    res = -ENOMEM;
    goto out;
  }

  res = task_init(task, process);
  if (res != ALL_OK)
  {
    goto out;
  }

  if (task_head == 0)
  {
    task_head = task;
    task_tail = task;
    current_task = task;
    goto out;
  }

  task_tail->next = task;
  task->prev = task_tail;
  task_tail = task;

out:
  if (ISERR(res))
  {
    task_free(task);
    return ERROR(res);
  }

  return task;
}

struct task_t *task_get_next()
{
  if (!current_task->next)
  {
    return task_head;
  }

  return current_task->next;
}

static void task_list_remove(struct task_t *task)
{
  if (task->prev)
  {
    task->prev->next = task->next;
  }

  if (task == task_head)
  {
    task_head = task->next;
  }

  if (task == task_tail)
  {
    task_tail = task->prev;
  }

  if (task == current_task)
  {
    current_task = task_get_next();
  }
}

int task_free(struct task_t *task)
{
  paging_free_4GB(task->page_directory);
  task_list_remove(task);

  // Finally free the task data
  kernel_free(task);
  return 0;
}

void task_next()
{
  struct task_t *next_task = task_get_next();
  if (!next_task)
  {
    panic("No more tasks!\n");
  }

  task_switch(next_task);
  task_return(&next_task->registers_state);
}

int task_switch(struct task_t *task)
{
  current_task = task;
  paging_switch(task->page_directory);
  return 0;
}

void task_save_state(struct task_t *task, struct interrupt_frame_t *frame)
{
  task->registers_state.ip = frame->ip;
  task->registers_state.cs = frame->cs;
  task->registers_state.flags = frame->flags;
  task->registers_state.esp = frame->esp;
  task->registers_state.ss = frame->ss;
  task->registers_state.eax = frame->eax;
  task->registers_state.ebp = frame->ebp;
  task->registers_state.ebx = frame->ebx;
  task->registers_state.ecx = frame->ecx;
  task->registers_state.edi = frame->edi;
  task->registers_state.edx = frame->edx;
  task->registers_state.esi = frame->esi;
}

int copy_string_from_task(struct task_t *task, void *virtual_addr, void *physical_addr, int max)
{
  if (max >= PAGING_PAGE_SIZE)
  {
    return -EINVARG;
  }

  int res = 0;
  char *tmp = kernel_zalloc(max);
  if (!tmp)
  {
    res = -ENOMEM;
    goto out;
  }

  uint32_t *task_directory = task->page_directory->directory_entry;
  uint32_t old_entry = paging_get(task_directory, tmp);
  paging_map(task->page_directory, tmp, tmp, PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
  paging_switch(task->page_directory);
  strncpy(tmp, virtual_addr, max);
  switch_to_kernel_page();

  res = paging_set(task_directory, tmp, old_entry);
  if (res < 0)
  {
    res = -EIO;
    goto out_free;
  }

  strncpy(physical_addr, tmp, max);

out_free:
  kernel_free(tmp);
out:
  return res;
}
void task_current_save_state(struct interrupt_frame_t *frame)
{
  if (!task_current())
  {
    panic("No current task to save\n");
  }

  struct task_t *task = task_current();
  task_save_state(task, frame);
}

int switch_to_task_page()
{
  user_registers();
  task_switch(current_task);
  return 0;
}

int task_page_task(struct task_t *task)
{
  user_registers();
  paging_switch(task->page_directory);
  return 0;
}

void task_run_first_ever_task()
{
  if (!current_task)
  {
    panic("task_run_first_ever_task(): No current task exists!\n");
  }

  task_switch(task_head);
  task_return(&task_head->registers_state);
}

int task_init(struct task_t *task, struct process_t *process)
{
  memset(task, 0, sizeof(struct task_t));
  // Map the entire 4GB address space to its self
  task->page_directory = paging_new_4GB(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
  if (!task->page_directory)
  {
    return -EIO;
  }

  task->registers_state.ip = PROGRAM_VIRTUAL_ADDRESS;
  // if (process->filetype == PROCESS_FILETYPE_ELF)
  // {
  //   task->registers.ip = elf_header(process->elf_file)->e_entry;
  // }

  task->registers_state.ss = USER_DATA_SEGMENT;
  task->registers_state.cs = USER_CODE_SEGMENT;
  task->registers_state.esp = PROGRAM_VIRTUAL_STACK_ADDRESS_START;

  task->process = process;

  return 0;
}

void *task_get_stack_item(struct task_t *task, int index)
{
  void *result = 0;

  uint32_t *sp_ptr = (uint32_t *)task->registers_state.esp;

  // Switch to the given tasks page
  task_page_task(task);

  result = (void *)sp_ptr[index];

  // Switch back to the kernel page
  switch_to_kernel_page();

  return result;
}

void *task_virtual_address_to_physical(struct task_t *task, void *virtual_address)
{
  return paging_get_physical_address(task->page_directory->directory_entry, virtual_address);
}