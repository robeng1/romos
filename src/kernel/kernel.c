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
#include "mm/memory.h"
#include "mm/heap/kernel_heap.h"
#include "mm/paging/paging.h"
#include "mm/blkm/blkm.h"

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


struct tss_entry_t kernel_tss;
struct gdt_entry_t gdt_entries[TOTAL_GDT_SEGMENTS];
// This array defines the Global Descriptor Table (GDT) for the system.
// The GDT is a table in memory that defines the characteristics of the various memory segments used by the system, including their base address, limit, and access rights.
// The processor uses the GDT to translate logical addresses into physical addresses.
struct gdt_ptr_t gdt_ptr[TOTAL_GDT_SEGMENTS] = {
    // NULL Segment
    // This segment is not used, but is required as the first entry in the GDT.
    {.base = 0x00, .limit = 0x00000000, .access_byte = 0x00, .flags = 0x00},

    // Kernel Code Segment
    // This segment is used for code that runs in kernel mode.
    // The access byte 0x9A means the segment is present (0x80), it's a code segment (0x10), it's executable (0x08), it's readable (0x02), and it runs in ring 0 (0x00).
    // The flags 0xC means the limit is specified in 4KiB blocks (0x8) and it's a 32-bit segment (0x4).
    {.base = 0x00, .limit = 0xFFFFF, .access_byte = 0x9A, .flags = 0xC},

    // Kernel Data Segment
    // This segment is used for data that is used in kernel mode.
    // The access byte 0x92 means the segment is present (0x80), it's a data segment (0x10), it's writable (0x02), and it runs in ring 0 (0x00).
    // The flags 0xC means the limit is specified in 4KiB blocks (0x8) and it's a 32-bit segment (0x4).
    {.base = 0x00, .limit = 0xFFFFF, .access_byte = 0x92, .flags = 0xC},

    // User Code Segment
    // This segment is used for code that runs in user mode (ring 3).
    // The access byte 0xFA means the segment is present (0x80), it's a code segment (0x10), it's executable (0x08), it's readable (0x02), and it runs in ring 3 (0x60).
    // The flags 0xC means the limit is specified in 4KiB blocks (0x8) and it's a 32-bit segment (0x4).
    {.base = 0x00, .limit = 0xFFFFF, .access_byte = 0xFA, .flags = 0xC},

    // User Data Segment
    // This segment is used for data that is used in user mode (ring 3).
    // The access byte 0xF2 means the segment is present (0x80), it's a data segment (0x10), it's writable (0x02), and it runs in ring 3 (0x60).
    // The flags 0xC means the limit is specified in 4KiB blocks (0x8) and it's a 32-bit segment (0x4).
    {.base = 0x00, .limit = 0xFFFFF, .access_byte = 0xF2, .flags = 0xC},

    // TSS Segment
    // This segment is used for the Task State Segment (TSS).
    // The access byte 0x89 means the segment is present (0x80), it's an available TSS (0x09).
    // The flags 0x0 means the limit is specified in byte units and it's a 16-bit segment.
    {.base = (uint32_t)&kernel_tss, .limit = sizeof(kernel_tss), .access_byte = 0x89, .flags = 0x0}};

void start_kernel()
{
  term_init();
  print("RomOS, the Operating System\n\n\n");
  memset(gdt_entries, 0x00, sizeof(gdt_entries));
  gdt_ptr_to_gdt(gdt_entries, gdt_ptr, TOTAL_GDT_SEGMENTS);
  // Load the gdt
  gdt_load(sizeof(gdt_entries), gdt_entries);

  // Initialize the heap
  kernel_heap_init();

  // Initialize filesystems
  fs_init();

  // Search and initialize the disks
  disk_search_and_init();

  // Initialize the interrupt descriptor table
  init_idt();

  // Setup the TSS
  memset(&kernel_tss, 0x00, sizeof(kernel_tss));
  kernel_tss.esp0 = 0x600000;
  kernel_tss.ss0 = KERNEL_DATA_SELECTOR;
  // Load the TSS
  tss_load(0x28);

  // Setup paging
  kernel_chunk = paging_new_4GB(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
  // Switch to kernel paging chunk
  paging_switch(kernel_chunk);
  // Enable paging
  paging_init();

  // Register the kernel commands
  isr80h_hookup_commands();
}