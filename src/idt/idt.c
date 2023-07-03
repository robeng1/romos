#include "idt.h"
#include "config.h"
#include "memory/memory.h"

struct idt_desc idt_descriptors[ROMOS_TOTAL_INTERRUPTS];
struct idtr_desc desc_ptr;

extern void load_idt(struct idtr_desc* ptr);


void set_idt(int interrupt_no, void *address)
{
  struct idt_desc *desc = &idt_descriptors[interrupt_no];
  desc->offset_1 = (uint32_t)address & 0x0000ffff;
  desc->selector = KERNEL_CODE_SELECTOR;
  desc->zero = 0x00;
  desc->type_attr = 0xEE;
  desc->offset_2 = (uint32_t) address >> 16;
}

void init_idt()
{
  memset(idt_descriptors, 0, sizeof(idt_descriptors));

  desc_ptr.limit = sizeof(idt_descriptors) - 1;
  desc_ptr.base = (uint32_t) idt_descriptors;

  // Load the interrupt descriptor table
  load_idt(&desc_ptr);
}
