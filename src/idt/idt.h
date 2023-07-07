#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct interrupt_frame_t;

typedef void *(*isr80h_cmd_t)(struct interrupt_frame_t *frame);
typedef void (*interrupt_callback_t)();

struct idt_entry_t
{
  uint16_t offset_1; // Offset bits 0 - 15
  uint16_t selector; // Selector thats in our GDT
  uint8_t zero;      // Does nothing, unused set to zero
  uint8_t type_attr; // Descriptor type and attributes
  uint16_t offset_2; // Offset bits 16-31
} __attribute__((packed));

struct idt_ptr_t
{
  uint16_t limit; // Size of descriptor table -1
  uint32_t base;  // Base address of the start of the interrupt descriptor table
} __attribute__((packed));

struct interrupt_frame_t
{
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t reserved;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  uint32_t ip;
  uint32_t cs;
  uint32_t flags;
  uint32_t esp;
  uint32_t ss;
} __attribute__((packed));

void init_idt();
extern void enable_interrupts();
extern void disable_interrupts();

void isr80h_register_command(int id, isr80h_cmd_t command);
int idt_register_interrupt_callback(int interrupt, interrupt_callback_t interrupt_callback);

#endif
