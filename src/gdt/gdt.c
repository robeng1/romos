#include "gdt.h"    // This line includes the gdt.h header file, which contains the definitions of the 'gdt_entry' and 'gdt_ptr' structures.
#include "kernel.h" // This line includes the kernel.h header file, which may contain other important definitions or functions for the OS kernel, such as 'panic' function.

// Here we define a function that encodes a 'gdt_ptr' structure into a format that the CPU can understand.
void encode_gdt_entry(uint8_t *target, struct gdt_ptr_t source)
{
  // Check whether the limit field of the source struct is too large for the processor's GDT limit field,
  // which should be at most 20 bits wide. Panic (an assumed function that stops system execution) if the limit is invalid.
  if ((source.limit > 65536) && ((source.limit & 0xFFF) != 0xFFF))
  {
    panic("encode_gdt_entry: Invalid argument\n");
  }

  // Set the default granularity to 1 byte.
  target[6] = 0x40;
  // If the limit is larger than 65536, we need to scale it down by a factor of 4096 (shift it 12 bits to the right) and set the granularity to 4K.
  if (source.limit > 65536)
  {
    source.limit = source.limit >> 12;
    target[6] = 0xC0;
  }

  // Encode the limit into the GDT entry.
  target[0] = source.limit & 0xFF;
  target[1] = (source.limit >> 8) & 0xFF;
  target[6] |= (source.limit >> 16) & 0x0F; // The 4 least significant bits of the limit are stored in the most significant nibble of the 6th byte.

  // Encode the base address into the GDT entry.
  target[2] = source.base & 0xFF;
  target[3] = (source.base >> 8) & 0xFF;
  target[4] = (source.base >> 16) & 0xFF;
  target[7] = (source.base >> 24) & 0xFF; // The 4 most significant bits of the base are stored in the 7th byte.

  // Set the type of the GDT entry.
  target[5] = source.type;
}

// Here we define a function that converts an array of 'gdt_ptr' structures to an array of 'gdt_entry' structures.
void gdt_ptr_to_gdt(struct gdt_entry_t *gdt_entry, struct gdt_ptr_t *gdt_ptr, int total_entries)
{
  // Loop through each 'gdt_ptr' structure in the array.
  for (int i = 0; i < total_entries; i++)
  {
    // Encode each 'gdt_ptr' structure as a 'gdt_entry' structure, and store the result in the corresponding position of the 'gdt_entry' array.
    encode_gdt_entry((uint8_t *)&gdt_entry[i], gdt_ptr[i]);
  }
}
