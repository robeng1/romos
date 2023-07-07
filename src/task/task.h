#ifndef TASK_H
#define TASK_H

#include "config.h"
#include "mm/paging/paging.h"

struct interrupt_frame_t;

// This structure represents the state of the CPU registers.
// It's often used in context switching, where the operating system needs to save the state of the current process so it can be restored later.
// The specific registers and their uses can vary depending on the CPU architecture.
// This structure appears to be for an x86 CPU.
struct registers_state_t
{
  uint32_t edi;   // Destination Index: used in string manipulation and memory operations
  uint32_t esi;   // Source Index: used in string manipulation and memory operations
  uint32_t ebp;   // Base Pointer: points to the base of the stack
  uint32_t ebx;   // Base register: used as a base pointer in memory operations
  uint32_t edx;   // Data register: used in I/O operations and some multiplication/division
  uint32_t ecx;   // Counter register: used in shift/rotate instructions and loops
  uint32_t eax;   // Accumulator register: used in arithmetic operations
  uint32_t ip;    // Instruction Pointer: points to the next instruction to be executed
  uint32_t cs;    // Code Segment: contains the segment of the current instruction pointer
  uint32_t flags; // Flags register: holds the processor flags
  uint32_t esp;   // Stack Pointer: points to the top of the stack
  uint32_t ss;    // Stack Segment: contains the segment of the current stack pointer
};

struct process_t;
struct task_t
{
  /**
   * The page directory of the task
   */
  struct paging_4GB_chunk_t *page_directory;

  // The registers of the task when the task is not running
  struct registers_state_t registers_state;

  // The process of the task
  struct process_t *process;

  // The next task in the linked list
  struct task_t *next;

  // Previous task in the linked list
  struct task_t *prev;
};

struct task_t *new_task(struct process_t *process);
struct task_t *task_current();
struct task_t *task_get_next();
int task_free(struct task_t *task);

int task_switch(struct task_t *task);
int switch_to_task_page();
int task_page_task(struct task_t *task);

void task_run_first_ever_task();

extern void task_return(struct registers_state_t *regs);
extern void restore_registers_state_t(struct registers_state_t *regs);
extern void user_registers();

void task_current_save_state(struct interrupt_frame_t *frame);
int copy_string_from_task(struct task_t *task, void *virtual_addr, void *physical_addr, int max);
void *task_get_stack_item(struct task_t *task, int index);
void *task_virtual_address_to_physical(struct task_t *task, void *virtual_addr);
void task_next();

#endif