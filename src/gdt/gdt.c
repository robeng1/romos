#include <gdt/gdt.h>
#include <common/system.h>

// Here we define a function that encodes a 'gdt_ptr' structure into a format that the CPU can understand.
void encode_gdt_entry(uint8_t *target, struct gdt_ptr_t source)
{
  // Check the limit to make sure that it can be encoded
  if (source.limit > 0xFFFFF)
  {
    PANIC("GDT cannot encode limits larger than 0xFFFFF");
  }

  // Encode the limit
  target[0] = source.limit & 0xFF;
  target[1] = (source.limit >> 8) & 0xFF;
  target[6] = (source.limit >> 16) & 0x0F;

  // Encode the base
  target[2] = source.base & 0xFF;
  target[3] = (source.base >> 8) & 0xFF;
  target[4] = (source.base >> 16) & 0xFF;
  target[7] = (source.base >> 24) & 0xFF;

  // Encode the access byte
  target[5] = source.access_byte;

  // Encode the flags
  target[6] |= (source.flags << 4);
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
