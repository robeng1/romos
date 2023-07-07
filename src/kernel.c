#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "string/string.h"
#include "fs/file.h"
#include "disk/disk.h"
#include "fs/parser.h"
#include "idt/idt.h"
#include "task/tss.h"
#include "gdt/gdt.h"
#include "config.h"
#include "status.h"
#include "isr80h/isr80h.h"
#include "memory/memory.h"
#include "memory/heap/kernel_heap.h"
#include "memory/paging/paging.h"
#include "memory/blkm/blkm.h"

uint16_t *vram = 0;
uint16_t t_row = 0;
uint16_t t_column = 0;

// takes a character and colour and outputs hex (in the correct endianess) representation for VRAM
uint16_t makechar(char c, char colour)
{
  return (colour << 8) | c;
}

void term_putchar(int row, int col, char c, char colour)
{
  vram[(row * VGA_WIDTH) + col] = makechar(c, colour);
}

void term_writechar(char c, char colour)
{
  if (c == '\n')
  {
    ++t_row;
    t_column = 0;
    return;
  }

  term_putchar(t_row, t_column, c, colour);
  ++t_column;
  if (t_column > VGA_WIDTH)
  {
    ++t_row;
    t_column = 0;
  }
}

void term_init()
{

  vram = (uint16_t *)(STDOUT);
  t_row = 0;
  t_column = 0;

  for (int y = 0; y < VGA_HEIGHT; ++y)
  {
    for (int x = 0; x < VGA_WIDTH; ++x)
    {
      term_putchar(y, x, ' ', 0);
    }
  }
}

void print(const char *str)
{
  size_t len = strlen(str);
  for (size_t i = 0; i < len; i++)
  {
    term_writechar(str[i], 15);
  }
}

void panic(const char *msg)
{
  print(msg);
  while (1)
  {
  }
}


static struct paging_4GB_chunk_t *kernel_chunk = 0;

void switch_to_kernel_page()
{
  kernel_registers();
  paging_switch(kernel_chunk);
}

struct tss_entry_t tss;

struct gdt_entry_t gdt_entries[ROMOS_TOTAL_GDT_SEGMENTS];

struct gdt_ptr_t gdt_ptr[ROMOS_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},                 // NULL Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x9a},           // Kernel code segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92},           // Kernel data segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf8},           // User code segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf2},           // User data segment
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xE9} // TSS Segment
};

void start_kernel()
{
  term_init();
  print("Welcome to RomOS, the only Operating System you'll ever need\n\n\n");
  memset(gdt_entries, 0x00, sizeof(gdt_entries));
  gdt_ptr_to_gdt(gdt_entries, gdt_ptr, ROMOS_TOTAL_GDT_SEGMENTS);

  // Load the gdt
  gdt_load(gdt_entries, sizeof(gdt_entries));

  // Initialize the heap
  kernel_heap_init();

  // Initialize filesystems
  fs_init();

  // Search and initialize the disks
  disk_search_and_init();

  // Initialize the interrupt descriptor table
  init_idt();

  // Setup the TSS
  memset(&tss, 0x00, sizeof(tss));
  tss.esp0 = 0x600000;
  tss.ss0 = KERNEL_DATA_SELECTOR;

  // Load the TSS
  tss_load(0x28);

  // Setup paging
  kernel_chunk = paging_new_4GB(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

  // Switch to kernel paging chunk
  paging_switch(kernel_chunk);

  // Enable paging
  // enable_paging();

  // Register the kernel commands
  isr80h_hookup_commands();

  enable_interrupts();
}